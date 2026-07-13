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
 *    may be used to endorse or promote products derived from this software
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
 * Tests for the affinity-aware X86Cpu constructor, the strict
 * buildFromCore factory, and the hybrid (P/E core) target-selection logic.
 *
 * The hybrid mixed-mask walk cannot be exercised on real hardware here (the lab
 * has no hybrid Intel part) nor under stock QEMU (no per-vCPU CPUID), so the
 * walk + selection policy are tested through X86Cpu::selectHybridTarget with an
 * affinity-aware fake CpuidUtils whose getCoreType() answer depends on which
 * core the injected pin closure last selected. The detection primitives
 * (isHybrid / getCoreType bit parsing) are tested via the gmock MockCpuidUtils.
 */

#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Cpuid/X86Cpu.hh"
#include "Au/Status.hh"
#include "MockTest.hh"

#include <algorithm>
#include <functional>
#include <vector>

#if defined(AU_TARGET_OS_IS_LINUX) && !defined(__CYGWIN__)
#include <sched.h>
#include <unistd.h>
#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif
#elif defined(AU_TARGET_OS_IS_WINDOWS)
#include <Windows.h>
#endif

namespace Au {

/*
 * Test peer: the only type befriended by X86Cpu (under AU_BUILD_TESTS). Gives
 * the white-box hybrid tests access to the private static selectHybridTarget
 * without widening the public/production API surface.
 */
class X86CpuHybridTestPeer
{
  public:
    static CpuNumT selectHybridTarget(const std::vector<CpuNumT>& candidates,
                                      CpuidUtils&                 cUtils,
                                      const std::function<bool(CpuNumT)>& pinFn)
    {
        return X86Cpu::selectHybridTarget(candidates, cUtils, pinFn);
    }
};

} // namespace Au

namespace {

using namespace Au;

// ===================================================================
// Detection primitives: isHybrid() and getCoreType() bit parsing.
// ===================================================================

class CpuidHybridDetection : public MockCpuidBase
{
  protected:
    void SetUp() override { MockCpuidBase::SetUp(); }
};

TEST_F(CpuidHybridDetection, IsHybridTrueWhenLeaf7Edx15Set)
{
    RequestT  leaf7{ 0x7, 0, 0, 0 };
    ResponseT resp{ 0, 0, 0, (1u << 15) }; // EDX bit 15 set
    ON_CALL(mockCpuidUtils, __raw_cpuid(leaf7))
        .WillByDefault(testing::Return(resp));

    EXPECT_TRUE(mockCpuidUtils.isHybrid());
}

TEST_F(CpuidHybridDetection, IsHybridFalseWhenLeaf7Edx15Clear)
{
    RequestT  leaf7{ 0x7, 0, 0, 0 };
    ResponseT resp{ 0, 0, 0, ~(1u << 15) }; // every bit but 15
    ON_CALL(mockCpuidUtils, __raw_cpuid(leaf7))
        .WillByDefault(testing::Return(resp));

    EXPECT_FALSE(mockCpuidUtils.isHybrid());
}

TEST_F(CpuidHybridDetection, GetCoreTypeParsesAtomAndCore)
{
    RequestT leaf1a{ 0x1A, 0, 0, 0 };

    ON_CALL(mockCpuidUtils, __raw_cpuid(leaf1a))
        .WillByDefault(testing::Return(ResponseT{ 0x20u << 24, 0, 0, 0 }));
    EXPECT_EQ(mockCpuidUtils.getCoreType(), AU_X86_CORE_TYPE_ATOM);

    ON_CALL(mockCpuidUtils, __raw_cpuid(leaf1a))
        .WillByDefault(testing::Return(ResponseT{ 0x40u << 24, 0, 0, 0 }));
    EXPECT_EQ(mockCpuidUtils.getCoreType(), AU_X86_CORE_TYPE_CORE);
}

// ===================================================================
// Hybrid walk + selection policy via selectHybridTarget.
//
// An affinity-aware fake: pinFn records the "current" core; getCoreType()
// returns that core's type from a caller-supplied topology map. This reproduces
// the real walk's invariant (CPUID.1A reflects the pinned core) without needing
// real hybrid silicon.
// ===================================================================

class TopologyFake : public CpuidUtils
{
  public:
    explicit TopologyFake(std::map<CpuNumT, Uint32> types)
        : m_types{ std::move(types) }
    {
    }

    void    setCurrent(CpuNumT core) { m_current = core; }
    CpuNumT pinned() const { return m_current; }

    // selectHybridTarget only consults getCoreType(); make __raw_cpuid for
    // leaf 0x1A reflect the currently-pinned core. Other leaves are unused.
    ResponseT __raw_cpuid(RequestT& req) override
    {
        if (req.eax == 0x1A) {
            auto   it   = m_types.find(m_current);
            Uint32 type = (it != m_types.end()) ? it->second
                                                : AU_X86_CORE_TYPE_CORE;
            return ResponseT{ type << 24, 0, 0, 0 };
        }
        return ResponseT{ 0, 0, 0, 0 };
    }

  private:
    std::map<CpuNumT, Uint32> m_types;
    CpuNumT                   m_current{ 0 };
};

// Build a pin closure bound to a TopologyFake. allowed = cores that can be
// pinned; pinning to anything else fails (simulates unpinnable cores).
static std::function<bool(CpuNumT)>
makePin(TopologyFake& fake, std::vector<CpuNumT> pinnable)
{
    return [&fake, pinnable](CpuNumT core) {
        if (std::find(pinnable.begin(), pinnable.end(), core) == pinnable.end())
            return false;
        fake.setCurrent(core);
        return true;
    };
}

TEST(X86CpuHybridSelect, MixedMaskPicksFirstECore)
{
    // cores 0,1 = P; cores 2,3 = E. First E-core is 2.
    TopologyFake         fake({ { 0, AU_X86_CORE_TYPE_CORE },
                                { 1, AU_X86_CORE_TYPE_CORE },
                                { 2, AU_X86_CORE_TYPE_ATOM },
                                { 3, AU_X86_CORE_TYPE_ATOM } });
    std::vector<CpuNumT> candidates{ 0, 1, 2, 3 };

    CpuNumT chosen = X86CpuHybridTestPeer::selectHybridTarget(
        candidates, fake, makePin(fake, candidates));

    EXPECT_EQ(chosen, 2u);
    EXPECT_EQ(fake.pinned(), 2u); // left pinned to its choice
}

TEST(X86CpuHybridSelect, AllPCoresFallsBackToFirstCandidate)
{
    TopologyFake fake(
        { { 4, AU_X86_CORE_TYPE_CORE }, { 5, AU_X86_CORE_TYPE_CORE } });
    std::vector<CpuNumT> candidates{ 4, 5 };

    CpuNumT chosen = X86CpuHybridTestPeer::selectHybridTarget(
        candidates, fake, makePin(fake, candidates));

    EXPECT_EQ(chosen, 4u); // no E-core -> first candidate
}

TEST(X86CpuHybridSelect, AllECoresPicksFirst)
{
    TopologyFake fake(
        { { 0, AU_X86_CORE_TYPE_ATOM }, { 1, AU_X86_CORE_TYPE_ATOM } });
    std::vector<CpuNumT> candidates{ 0, 1 };

    CpuNumT chosen = X86CpuHybridTestPeer::selectHybridTarget(
        candidates, fake, makePin(fake, candidates));

    EXPECT_EQ(chosen, 0u);
}

TEST(X86CpuHybridSelect, SkipsUnpinnableCoresAndStillFindsECore)
{
    // core 1 is an E-core but cannot be pinned (e.g. transient cpuset race);
    // core 3 is the next E-core and is pinnable.
    TopologyFake         fake({ { 0, AU_X86_CORE_TYPE_CORE },
                                { 1, AU_X86_CORE_TYPE_ATOM },
                                { 2, AU_X86_CORE_TYPE_CORE },
                                { 3, AU_X86_CORE_TYPE_ATOM } });
    std::vector<CpuNumT> candidates{ 0, 1, 2, 3 };
    std::vector<CpuNumT> pinnable{ 0, 2, 3 }; // 1 omitted

    CpuNumT chosen = X86CpuHybridTestPeer::selectHybridTarget(
        candidates, fake, makePin(fake, pinnable));

    EXPECT_EQ(chosen, 3u);
}

TEST(X86CpuHybridSelect, NoPinnableCoresFallsBackToFirstCandidate)
{
    TopologyFake         fake({ { 7, AU_X86_CORE_TYPE_ATOM } });
    std::vector<CpuNumT> candidates{ 7 };

    CpuNumT chosen = X86CpuHybridTestPeer::selectHybridTarget(
        candidates, fake, makePin(fake, /*pinnable*/ {}));

    EXPECT_EQ(chosen, 7u); // could not pin anything -> first candidate
}

// ===================================================================
// Real integration on this (AMD) host: buildFromCore + graceful degrade,
// using the process's actual affinity. Linux only.
// ===================================================================

#if defined(AU_TARGET_OS_IS_LINUX) && !defined(__CYGWIN__)

namespace {
    // Lowest and a definitely-out-of-mask core for the current thread.
    CpuNumT firstAllowedCore(const cpu_set_t& mask)
    {
        for (CpuNumT i = 0; i < CPU_SETSIZE; ++i)
            if (CPU_ISSET(i, &mask))
                return i;
        return 0;
    }
} // namespace

class X86CpuAffinityIntegration : public ::testing::Test
{
  protected:
    cpu_set_t original{};
    pid_t     tid{};

    void SetUp() override
    {
        tid = gettid();
        CPU_ZERO(&original);
        ASSERT_EQ(sched_getaffinity(tid, sizeof(original), &original), 0);
    }

    void TearDown() override
    {
        // Always restore, even if a test pinned us.
        sched_setaffinity(tid, sizeof(original), &original);
    }
};

TEST_F(X86CpuAffinityIntegration, BuildFromCoreSucceedsForAllowedCore)
{
    CpuNumT core = firstAllowedCore(original);
    auto    sor  = X86Cpu::buildFromCore(core);

    ASSERT_TRUE(sor.ok()) << sor.status().message();
    // The constructed object is usable.
    (void)(*sor).isAMD();
}

TEST_F(X86CpuAffinityIntegration, BuildFromCoreCurrentCpuNumAlwaysOk)
{
    auto sor = X86Cpu::buildFromCore(AU_CURRENT_CPU_NUM);
    EXPECT_TRUE(sor.ok()) << sor.status().message();
}

TEST_F(X86CpuAffinityIntegration, BuildFromCoreRejectsCoreOutsideMask)
{
    // Restrict affinity to a single core, then request a different one.
    CpuNumT   only = firstAllowedCore(original);
    cpu_set_t restricted;
    CPU_ZERO(&restricted);
    CPU_SET(only, &restricted);
    ASSERT_EQ(sched_setaffinity(tid, sizeof(restricted), &restricted), 0);

    // Find a core NOT in the restricted set (the restricted set is just
    // {only}).
    CpuNumT outside = (only == 0) ? 1u : 0u;
    // Only meaningful if the machine actually has that core in the original
    // mask or it is simply not in our restricted set (it isn't, by
    // construction).
    auto sor = X86Cpu::buildFromCore(outside);

    EXPECT_FALSE(sor.ok());
    EXPECT_EQ(sor.status().code(), InvalidArgumentError().code());
}

TEST_F(X86CpuAffinityIntegration, ConstructorDegradesInsteadOfThrowing)
{
    // Restrict to one core, ask for an out-of-mask core: must NOT throw, must
    // produce usable data, and must leave the (restricted) affinity intact.
    CpuNumT   only = firstAllowedCore(original);
    cpu_set_t restricted;
    CPU_ZERO(&restricted);
    CPU_SET(only, &restricted);
    ASSERT_EQ(sched_setaffinity(tid, sizeof(restricted), &restricted), 0);

    CpuNumT outside = (only == 0) ? 1u : 0u;
    EXPECT_NO_THROW({
        X86Cpu cpu{ outside };
        (void)cpu.isAMD();
    });

    // Affinity restored to the restricted set (constructor restores its mask).
    cpu_set_t after;
    CPU_ZERO(&after);
    ASSERT_EQ(sched_getaffinity(tid, sizeof(after), &after), 0);
    EXPECT_TRUE(CPU_ISSET(only, &after));
}

#endif // Linux

// ===================================================================
// Real integration on Windows: buildFromCore + graceful degrade, driving the
// Win32 affinity path (GetThreadGroupAffinity / SetThreadAffinityMask). These
// mirror the Linux X86CpuAffinityIntegration cases so both OS constructor paths
// get exercised on their native hardware.
// ===================================================================

#if defined(AU_TARGET_OS_IS_WINDOWS)

namespace {
    // Lowest set bit (first allowed core) of a Win32 affinity mask.
    CpuNumT firstAllowedCoreWin(KAFFINITY mask)
    {
        for (CpuNumT i = 0; i < sizeof(KAFFINITY) * 8; ++i)
            if (mask & (static_cast<KAFFINITY>(1) << i))
                return i;
        return 0;
    }
} // namespace

class X86CpuAffinityIntegrationWin : public ::testing::Test
{
  protected:
    KAFFINITY original{ 0 };
    bool      haveMask{ false };

    void SetUp() override
    {
        GROUP_AFFINITY ga{};
        if (GetThreadGroupAffinity(GetCurrentThread(), &ga)) {
            original = ga.Mask;
            haveMask = true;
        }
    }

    void TearDown() override
    {
        // Restore the original thread mask if we changed it.
        if (haveMask)
            SetThreadAffinityMask(GetCurrentThread(),
                                  static_cast<DWORD_PTR>(original));
    }
};

TEST_F(X86CpuAffinityIntegrationWin, BuildFromCoreSucceedsForAllowedCore)
{
    ASSERT_TRUE(haveMask);
    CpuNumT core = firstAllowedCoreWin(original);
    auto    sor  = X86Cpu::buildFromCore(core);

    ASSERT_TRUE(sor.ok()) << sor.status().message();
    (void)(*sor).isAMD();
}

TEST_F(X86CpuAffinityIntegrationWin, BuildFromCoreCurrentCpuNumAlwaysOk)
{
    auto sor = X86Cpu::buildFromCore(AU_CURRENT_CPU_NUM);
    EXPECT_TRUE(sor.ok()) << sor.status().message();
}

TEST_F(X86CpuAffinityIntegrationWin, BuildFromCoreRejectsCoreOutsideMask)
{
    ASSERT_TRUE(haveMask);
    // Restrict the thread to a single core, then request a different one.
    CpuNumT only = firstAllowedCoreWin(original);
    ASSERT_NE(SetThreadAffinityMask(GetCurrentThread(),
                                    static_cast<DWORD_PTR>(1) << only),
              0u);

    CpuNumT outside = (only == 0) ? 1u : 0u;
    auto    sor     = X86Cpu::buildFromCore(outside);

    EXPECT_FALSE(sor.ok());
    EXPECT_EQ(sor.status().code(), InvalidArgumentError().code());
}

TEST_F(X86CpuAffinityIntegrationWin, ConstructorDegradesInsteadOfThrowing)
{
    ASSERT_TRUE(haveMask);
    // Restrict to one core, ask for an out-of-mask core: must NOT throw and
    // must produce usable data.
    CpuNumT only = firstAllowedCoreWin(original);
    ASSERT_NE(SetThreadAffinityMask(GetCurrentThread(),
                                    static_cast<DWORD_PTR>(1) << only),
              0u);

    CpuNumT outside = (only == 0) ? 1u : 0u;
    EXPECT_NO_THROW({
        X86Cpu cpu{ outside };
        (void)cpu.isAMD();
    });
}

#endif // Windows

} // namespace
