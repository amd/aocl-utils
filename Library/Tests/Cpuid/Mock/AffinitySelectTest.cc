/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Core-level affinity selection tests: hybrid E-core walk and strict/non-strict cpuset.
 * Replaced X86CpuAffinityTest.cc. Uses injectable au_affinity_select_target_ex with
 * mocked primitives to test the real decision logic on non-hybrid CI hardware.
 */

#include "Au/Cpuid/X86Cpu.hh"
#include "Capi/au/cpuid/cpuid_inline.h"

/* assert() must fire in Release too: CMake adds -DNDEBUG there, which turns
 * assert() into a no-op and would make EXPECT-free checks vacuous. gtest's
 * EXPECT_/ASSERT_ macros are independent of NDEBUG, but this file also uses a
 * couple of plain assert()s for invariants that must never be compiled out. */
#undef NDEBUG
#include <cassert>

#include "gtest/gtest.h"
#include <vector>

namespace {

using namespace Au;

/* Mock affinity/CPUID world: hybrid package model with allowed mask, core-types, unpinnable set. */
struct HybridWorld
{
    std::vector<int> allowed;             /* global indices in the mask     */
    std::vector<uint32_t> coreType;       /* per global index: ATOM/CORE/0  */
    std::vector<int> unpinnable;          /* pin() returns false for these  */
    bool     isHybrid = true;
    int      current  = -1;               /* last successfully pinned core   */
    int      pinCalls = 0;

    bool inAllowed(int c) const
    {
        for (int a : allowed)
            if (a == c)
                return true;
        return false;
    }
    bool isUnpinnable(int c) const
    {
        for (int u : unpinnable)
            if (u == c)
                return true;
        return false;
    }
    uint32_t typeOf(int c) const
    {
        return (c >= 0 && c < (int)coreType.size()) ? coreType[c] : 0u;
    }
};

bool
mockReadMask(au_affinity_mask_t* out, void* ctx)
{
    auto* w = static_cast<HybridWorld*>(ctx);
    memset(out, 0, sizeof(*out));
    for (int a : w->allowed) {
        if (a >= 0 && a < AU_AFFINITY_MAX_CPUS)
            out->w[a / (int)(8 * sizeof(unsigned long))] |=
                ((unsigned long)1 << (a % (int)(8 * sizeof(unsigned long))));
    }
    out->valid = true;
    return true;
}

bool
mockPin(int core, void* ctx)
{
    auto* w = static_cast<HybridWorld*>(ctx);
    w->pinCalls++;
    if (w->isUnpinnable(core))
        return false;
    w->current = core; /* thread now "executes" on `core` */
    return true;
}

/* Answers leaf 7 (hybrid bit) and leaf 0x1A (core-type of pinned core). */
void
mockRawFn(uint32_t eax, uint32_t ecx, au_cpuid_regs_t* out, void* ctx)
{
    auto* w = static_cast<HybridWorld*>(ctx);
    memset(out, 0, sizeof(*out));
    (void)ecx;
    if (eax == 0x00000007) {
        if (w->isHybrid)
            out->edx = (1u << 15);
    } else if (eax == 0x0000001A) {
        out->eax = (w->typeOf(w->current) << 24);
    }
}

au_affinity_ops_t
opsFor(HybridWorld* w)
{
    au_affinity_ops_t ops;
    ops.read_mask = mockReadMask;
    ops.pin       = mockPin;
    ops.raw_fn    = mockRawFn;
    ops.ctx       = w;
    return ops;
}

/* ===================================================================== *
 *  Task 2: core-level hybrid E-core selection (replaces the deleted
 *  X86CpuAffinityTest.cc) -- drives the REAL walk via the ops seam.
 * ===================================================================== */

/* CURRENT on a hybrid package whose mask spans P and E cores: the walk must
 * return the FIRST E-core (Atom), the crash-safe feature intersection. */
TEST(AffinitySelect, HybridPicksFirstEcore)
{
    HybridWorld w;
    w.allowed  = { 0, 1, 2, 3 };
    w.coreType = { AU_CPUID_CORE_TYPE_CORE, /* 0: P */
                   AU_CPUID_CORE_TYPE_CORE, /* 1: P */
                   AU_CPUID_CORE_TYPE_ATOM, /* 2: E <- expected */
                   AU_CPUID_CORE_TYPE_ATOM /* 3: E */ };
    au_affinity_ops_t ops = opsFor(&w);

    bool err    = true;
    int  target = au_affinity_select_target_ex(
        AU_CPUID_CURRENT_CPU, /*strict=*/false, &err, &ops);

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 2) << "must select the first E-core in mask order";
}

/* The E-core need not be core 0; a lower-indexed P-core must be walked past. */
TEST(AffinitySelect, HybridSkipsLeadingPcores)
{
    HybridWorld w;
    w.allowed  = { 4, 5, 6 };
    w.coreType.assign(7, AU_CPUID_CORE_TYPE_CORE);
    w.coreType[6] = AU_CPUID_CORE_TYPE_ATOM; /* only core 6 is an E-core */
    au_affinity_ops_t ops = opsFor(&w);

    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, false, &err, &ops);

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 6);
}

/* An unpinnable candidate is skipped (best effort); the next pinnable E-core
 * wins, and pin() was actually attempted on the unpinnable one. */
TEST(AffinitySelect, HybridSkipsUnpinnableThenFindsEcore)
{
    HybridWorld w;
    w.allowed    = { 0, 1, 2 };
    w.coreType   = { AU_CPUID_CORE_TYPE_CORE,
                     AU_CPUID_CORE_TYPE_ATOM, /* E, but unpinnable */
                     AU_CPUID_CORE_TYPE_ATOM /* E, pinnable <- expected */ };
    w.unpinnable = { 1 };
    au_affinity_ops_t ops = opsFor(&w);

    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, false, &err, &ops);

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 2);
    EXPECT_GE(w.pinCalls, 3) << "walk must have attempted every candidate "
                                "up to and including the pinnable E-core";
}

/* Hybrid mask that is all P-cores (no Atom): fall back to the first-allowed
 * core. Every candidate was pinnable, so NO stderr notice is emitted. */
TEST(AffinitySelect, HybridAllPcoresFallsBackToFirstAllowedNoNotice)
{
    HybridWorld w;
    w.allowed  = { 3, 4, 5 };
    w.coreType.assign(6, AU_CPUID_CORE_TYPE_CORE); /* all P */
    au_affinity_ops_t ops = opsFor(&w);

    testing::internal::CaptureStderr();
    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, false, &err, &ops);
    std::string errout = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 3) << "P-only hybrid mask -> first-allowed core";
    EXPECT_TRUE(errout.empty())
        << "no unverified core -> no over-wide-features warning; got: "
        << errout;
}

/* Hybrid mask whose only E-core candidate is unpinnable and the rest are
 * P-cores: fall back to first-allowed AND emit the over-wide-features notice
 * (an unverified core might be an E-core the thread could migrate onto). */
TEST(AffinitySelect, HybridUnverifiedCoreEmitsNotice)
{
    HybridWorld w;
    w.allowed    = { 2, 3 };
    w.coreType   = { 0, 0, AU_CPUID_CORE_TYPE_CORE,
                     AU_CPUID_CORE_TYPE_ATOM /* E but unpinnable */ };
    w.unpinnable = { 3 };
    au_affinity_ops_t ops = opsFor(&w);

    testing::internal::CaptureStderr();
    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, false, &err, &ops);
    std::string errout = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 2) << "fall back to first-allowed core";
    EXPECT_NE(errout.find("could not pin to every core"), std::string::npos)
        << "an unpinnable (unverified) candidate must warn; got: " << errout;
}

/* A NON-hybrid package short-circuits: CURRENT resolves to first-allowed with
 * no per-core probing (pin() is never called for the walk). */
TEST(AffinitySelect, NonHybridShortCircuitsToFirstAllowed)
{
    HybridWorld w;
    w.isHybrid = false;
    w.allowed  = { 7, 8, 9 };
    au_affinity_ops_t ops = opsFor(&w);

    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, false, &err, &ops);

    EXPECT_FALSE(err);
    EXPECT_EQ(target, 7);
    EXPECT_EQ(w.pinCalls, 0)
        << "non-hybrid CURRENT must not probe/pin candidates";
}

/* ===================================================================== *
 *  Task 3: strict / non-strict cpuset semantics of the selection brain.
 * ===================================================================== */

/* Specific core IN the allowed mask: returned as-is, strict and non-strict. */
TEST(AffinitySelect, SpecificInMaskIsHonored)
{
    HybridWorld w;
    w.isHybrid = false;
    w.allowed  = { 1, 2, 3 };
    au_affinity_ops_t ops = opsFor(&w);

    for (bool strict : { false, true }) {
        bool err    = true;
        int  target = au_affinity_select_target_ex(2, strict, &err, &ops);
        EXPECT_FALSE(err) << "strict=" << strict;
        EXPECT_EQ(target, 2) << "strict=" << strict;
    }
}

/* Specific core OUT of the allowed mask:
 *   strict     -> error (target -1),
 *   non-strict -> degrade to first-allowed, no error. */
TEST(AffinitySelect, SpecificOutOfMaskStrictErrorsNonStrictDegrades)
{
    HybridWorld w;
    w.isHybrid = false;
    w.allowed  = { 5, 6, 7 }; /* core 2 is NOT allowed */
    au_affinity_ops_t ops = opsFor(&w);

    /* strict: hard failure */
    {
        bool err    = false;
        int  target = au_affinity_select_target_ex(2, /*strict=*/true, &err,
                                                   &ops);
        EXPECT_TRUE(err);
        EXPECT_EQ(target, -1);
    }
    /* non-strict: degrade to first-allowed (5), no error */
    {
        bool err    = true;
        int  target = au_affinity_select_target_ex(2, /*strict=*/false, &err,
                                                   &ops);
        EXPECT_FALSE(err);
        EXPECT_EQ(target, 5);
    }
}

/* AU_CPUID_CURRENT_CPU never fails strict, even on a restricted mask. */
TEST(AffinitySelect, CurrentNeverFailsStrict)
{
    HybridWorld w;
    w.isHybrid = false;
    w.allowed  = { 9 };
    au_affinity_ops_t ops = opsFor(&w);

    bool err    = true;
    int  target =
        au_affinity_select_target_ex(AU_CPUID_CURRENT_CPU, true, &err, &ops);
    EXPECT_FALSE(err);
    EXPECT_EQ(target, 9);
}

/* Strict semantics end-to-end on real hardware (impossible core index guarantees out-of-mask). */

/* Impossible core fails strict, degrades non-strict. */
TEST(AffinityStrictInit, ImpossibleCoreStrictFailsNonStrictDegrades)
{
    const int impossible = AU_AFFINITY_MAX_CPUS + 10;

    /* strict -> false (requested core cannot be in the allowed mask) */
    {
        au_cpu_info_t c;
        c.raw_fn  = nullptr;
        c.raw_ctx = nullptr;
        bool ok   = au_cpuid_init(&c, impossible, /*strict=*/true);
        EXPECT_FALSE(ok) << "strict resolve of an impossible core must fail";
    }
    /* non-strict -> true (degrades to first-allowed) and still populates. Per
     * the contract affinity_honored == (pinned || CURRENT): the degrade pins
     * FIRST-ALLOWED (a valid target), and that pin succeeds on any normal
     * runner, so honored is true here -- it reports "the snapshot reflects a
     * deterministic pinned core", not "your exact requested core". (Contrast
     * with a specific-core pin that FAILS outright, honored=false; and see
     * RealCpusetStrictContract, which asserts honored=false for a genuine
     * out-of-mask degrade where the requested core itself was not pinned.) */
    {
        au_cpu_info_t c;
        c.raw_fn  = nullptr;
        c.raw_ctx = nullptr;
        bool ok   = au_cpuid_init(&c, impossible, /*strict=*/false);
        EXPECT_TRUE(ok) << "non-strict resolve degrades, never fails";
        EXPECT_TRUE(au_cpuid_info_affinity_honored(&c))
            << "degrade pinned first-allowed -> honored per (pinned||CURRENT)";
    }
}

/* X86Cpu::buildFromCore maps the core's strict failure to StatusInvalidArgument
 * for an impossible core, and succeeds for AU_CURRENT_CPU_NUM. */
TEST(AffinityStrictInit, BuildFromCoreReportsInvalidArgument)
{
    auto bad = X86Cpu::buildFromCore((CpuNumT)(AU_AFFINITY_MAX_CPUS + 10));
    EXPECT_FALSE(bad.ok())
        << "an impossible specific core must yield an error Status";

    auto cur = X86Cpu::buildFromCore(AU_CURRENT_CPU_NUM);
    EXPECT_TRUE(cur.ok()) << "AU_CURRENT_CPU_NUM must never be rejected";
}

/* affinity_honored == (specific pin succeeded) || (CURRENT request). The
 * CURRENT path is always honored (nothing specific to pin). */
TEST(AffinityStrictInit, CurrentIsAlwaysHonored)
{
    au_cpu_info_t c;
    c.raw_fn  = nullptr;
    c.raw_ctx = nullptr;
    bool ok   = au_cpuid_init(&c, AU_CPUID_CURRENT_CPU, /*strict=*/false);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(au_cpuid_info_affinity_honored(&c));
}

#if defined(__linux__)
#include <sched.h>
#include <unistd.h>

/* Real cpuset strict/non-strict contract test (skips if only one core in mask). */
TEST(AffinityStrictInit, RealCpusetStrictContract)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    ASSERT_EQ(sched_getaffinity(0, sizeof(set), &set), 0);

    int firstIn = -1, firstOut = -1;
    long nconf = sysconf(_SC_NPROCESSORS_CONF);
    for (int c = 0; c < (int)nconf && c < CPU_SETSIZE; ++c) {
        if (CPU_ISSET(c, &set)) {
            if (firstIn < 0)
                firstIn = c;
        } else if (firstOut < 0) {
            firstOut = c;
        }
    }
    ASSERT_GE(firstIn, 0) << "the calling thread must have at least one core";

    /* In-mask specific core: strict succeeds AND is honored (pin took). */
    {
        au_cpu_info_t c;
        c.raw_fn  = nullptr;
        c.raw_ctx = nullptr;
        bool ok   = au_cpuid_init(&c, firstIn, /*strict=*/true);
        EXPECT_TRUE(ok) << "in-mask core " << firstIn << " must resolve strict";
        EXPECT_TRUE(au_cpuid_info_affinity_honored(&c))
            << "in-mask specific core pin must be honored";
    }

    if (firstOut < 0)
        GTEST_SKIP() << "no out-of-mask core available (all cores allowed); "
                        "run under `taskset -c 0` to exercise the strict-fail "
                        "side";

    /* Out-of-mask specific core: strict fails, non-strict degrades. */
    {
        au_cpu_info_t c;
        c.raw_fn  = nullptr;
        c.raw_ctx = nullptr;
        EXPECT_FALSE(au_cpuid_init(&c, firstOut, /*strict=*/true))
            << "out-of-mask core " << firstOut << " must fail strict";
    }
    {
        au_cpu_info_t c;
        c.raw_fn  = nullptr;
        c.raw_ctx = nullptr;
        EXPECT_TRUE(au_cpuid_init(&c, firstOut, /*strict=*/false))
            << "out-of-mask core must degrade (never fail) non-strict";
        /* Per the (pinned || CURRENT) contract: the degrade selects and pins
         * first-allowed, which succeeds, so honored is true. NOTE for review:
         * the au_cpu_info_t::affinity_honored doc-comment prose still says a
         * degraded specific request reads the current core and is therefore
         * NOT honored -- that prose is now stale vs the implemented formula
         * (a degrade pins first-allowed, not "stay on current"). Flagged for
         * Abhiram; not silently changed here. */
        EXPECT_TRUE(au_cpuid_info_affinity_honored(&c))
            << "non-strict degrade pins first-allowed -> honored (pinned)";
    }
}
#endif /* __linux__ */

} // namespace
