/*
 * Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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

#include "Au/Config.h"
#include "Au/Cpuid/CpuidUtils.hh"
#include "X86RawData.hh"

#include <cstdio>
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
#include <direct.h>
#include <io.h>
#endif

namespace Au {

#if defined(AU_TARGET_OS_IS_LINUX) && !defined(__CYGWIN__)
namespace {
    /*
     * Lowest set bit of the affinity mask. A running thread always has at least
     * one allowed core, so the mask is guaranteed non-empty; the trailing
     * return is defensive only.
     */
    CpuNumT getFirstCore(const cpu_set_t& mask)
    {
        for (CpuNumT i = 0; i < CPU_SETSIZE; ++i) {
            if (CPU_ISSET(i, &mask))
                return i;
        }
        return 0; // unreachable; defensive
    }

    /* Allowed cores of a mask, ascending. */
    std::vector<CpuNumT> maskToCandidates(const cpu_set_t& mask)
    {
        std::vector<CpuNumT> cores;
        for (CpuNumT i = 0; i < CPU_SETSIZE; ++i) {
            if (CPU_ISSET(i, &mask))
                cores.push_back(i);
        }
        return cores;
    }

    bool pinTo(pid_t tid, CpuNumT core)
    {
        cpu_set_t probe;
        CPU_ZERO(&probe);
        CPU_SET(core, &probe);
        return sched_setaffinity(tid, sizeof(probe), &probe) == 0;
    }
} // namespace
#elif defined(AU_TARGET_OS_IS_WINDOWS)
namespace {
    // Bits in a single processor group's affinity mask (KAFFINITY is 64-bit on
    // x64). Core indices used below are GROUP-RELATIVE (0..63 within a group),
    // paired with the group number they belong to.
    constexpr CpuNumT kAffinityBits = sizeof(KAFFINITY) * 8;

    /*
     * Read the calling thread's group affinity WITHOUT mutating scheduling.
     * GetThreadGroupAffinity reports the thread's current processor group and
     * its group-relative mask directly, unlike the SetThreadAffinityMask
     * set-and-restore trick which transiently widens (and could leave) the
     * thread on a different mask. We must keep the whole GROUP_AFFINITY, not
     * just the mask: on machines with more than 64 logical CPUs (e.g. large
     * EPYC/Xeon parts with 128-512 threads) the mask alone is group-ambiguous,
     * and pinning/restoring by mask via SetThreadAffinityMask (which is
     * relative to the thread's *current* group) targets the wrong group.
     * Returns false if the query fails; *ga is untouched in that case.
     */
    bool readThreadAffinity(GROUP_AFFINITY* ga)
    {
        GROUP_AFFINITY tmp{};
        if (!GetThreadGroupAffinity(GetCurrentThread(), &tmp))
            return false;
        *ga = tmp;
        return true;
    }

    /* Lowest set bit of the mask. Mask is guaranteed non-empty for a running
     * thread; the zero return is defensive only. */
    CpuNumT getFirstCore(KAFFINITY mask)
    {
        for (CpuNumT i = 0; i < kAffinityBits; ++i) {
            if (mask & (static_cast<KAFFINITY>(1) << i))
                return i;
        }
        return 0; // unreachable; defensive
    }

    /* Group-relative allowed cores of a mask, ascending. */
    std::vector<CpuNumT> maskToCandidates(KAFFINITY mask)
    {
        std::vector<CpuNumT> cores;
        for (CpuNumT i = 0; i < kAffinityBits; ++i) {
            if (mask & (static_cast<KAFFINITY>(1) << i))
                cores.push_back(i);
        }
        return cores;
    }

    /*
     * Pin the calling thread to a single group-relative core within the given
     * processor group, via SetThreadGroupAffinity (group-aware, unlike
     * SetThreadAffinityMask which is relative to the thread's current group).
     * Returns false on failure. The caller restores the original group affinity
     * afterwards.
     */
    bool pinTo(WORD group, CpuNumT core)
    {
        if (core >= kAffinityBits)
            return false;
        GROUP_AFFINITY ga{};
        ga.Group = group;
        ga.Mask  = static_cast<KAFFINITY>(1) << core;
        return SetThreadGroupAffinity(GetCurrentThread(), &ga, nullptr) != 0;
    }
} // namespace
#endif

/*
 * Select the crash-safe target core on a hybrid package.
 *
 * On a hybrid Intel package whose mask spans both core types, the thread can be
 * migrated P<->E at any time, so the only crash-safe feature set is the
 * intersection of all reachable cores. For every AOCL-relevant ISA extension
 * the hierarchy is strict (E-core features subset of P-core features), so that
 * intersection is exactly the E-core's feature set - hence we probe an E-core
 * (Atom) when one is present.
 *
 * Assumption: E-core feature set is a subset of
 * the P-core feature set for all flags AOCL queries; true on all shipping
 * hybrid parts.
 * TODO: if a future hybrid SKU ships an E-core-only flag, replace "probe one
 * E-core" with a true per-core feature-intersection walk.
 *
 * Pure with respect to the platform: the pin side-effect is injected via pinFn,
 * which lets white-box tests drive the walk with an affinity-aware mock.
 *
 * Pinning note: the walk pins each candidate it probes, so on return the thread
 * is left pinned to the LAST candidate scanned -- which equals the returned
 * core only when an E-core was found. When no E-core is found the function
 * returns candidates.front() while the thread is pinned to the last candidate.
 * Callers must therefore pin to the returned target themselves and not rely on
 * the thread already being on it; the constructor does exactly that before
 * probing.
 */
CpuNumT
X86Cpu::selectHybridTarget(const std::vector<CpuNumT>&         candidates,
                           CpuidUtils&                         cUtils,
                           const std::function<bool(CpuNumT)>& pinFn)
{
    bool hadUnpinnable = false;
    for (CpuNumT core : candidates) {
        if (!pinFn(core)) {
            hadUnpinnable = true;
            continue; // skip cores we cannot pin to; best effort
        }
        if (cUtils.getCoreType() == AU_X86_CORE_TYPE_ATOM)
            return core; // E-core: crash-safe intersection, pinned here
    }
    // Reached only when no E-core was pinned. A pinnable E-core would have been
    // returned above, so every core we could verify was a P-core. If all cores
    // were pinnable, the mask is genuinely P-only and reporting P-core features
    // is correct - no warning. Only an *unpinnable* core is unknown: it may be
    // an E-core the thread could later migrate onto and then see fewer features
    // than the P-core we fall back to reports. Warn on stderr in that case so
    // the library developer knows the result may be over-wide.
    if (hadUnpinnable) {
        std::fprintf(
            stderr,
            "aoclutils: X86Cpu could not pin to every core in a hybrid CPU's "
            "affinity mask; falling back to core %u. CPUID feature flags may "
            "be wider than an unverified E-core in the mask supports.\n",
            candidates.front());
    }
    return candidates.front();
}

/*
 * Target for the AU_CURRENT_CPU_NUM path: the first candidate on a non-hybrid
 * package (AMD + old Intel, the 99% case), or the crash-safe E-core on a hybrid
 * package whose mask spans both core types.
 */
CpuNumT
X86Cpu::resolveSentinelTarget(const std::vector<CpuNumT>&         candidates,
                              const std::function<bool(CpuNumT)>& pinFn)
{
    CpuidUtils cutils;
    if (!cutils.isHybrid())
        return candidates.front();
    return selectHybridTarget(candidates, cutils, pinFn);
}

X86Cpu::X86Cpu(CpuidUtils* cUtils, CpuNumT num)
    : CpuInfo{ num }
    , m_pimpl{ new X86Cpu::Impl{ cUtils } }
{
    // TODO: Thread pinning.
    pImpl()->update();
}

X86Cpu::X86Cpu(CpuNumT num)
    : CpuInfo{ num }
    , m_pimpl{ new X86Cpu::Impl{} }
{
#if defined(AU_TARGET_OS_IS_LINUX) && !defined(__CYGWIN__)
    auto      tid = gettid();
    cpu_set_t mask;
    CPU_ZERO(&mask);

    // If we cannot even read the mask, fall back to a best-effort probe.
    if (sched_getaffinity(tid, sizeof(mask), &mask) != 0) {
        pImpl()->update();
        return;
    }

    // Resolve the target core deterministically:
    //  - explicit num       -> that core if allowed, else first core of the
    //  mask
    //  - AU_CURRENT_CPU_NUM  -> first core, or the crash-safe E-core on a
    //  hybrid
    //                           mixed mask (see resolveSentinelTarget)
    // The explicit-num fallback is deterministic (first core of the mask) and
    // does not re-run hybrid selection.
    //
    // Guard num against CPU_SETSIZE before CPU_ISSET: the macro indexes the
    // fixed-size mask and reads out of bounds for an arbitrarily large index.
    CpuNumT target = (num != AU_CURRENT_CPU_NUM)
                         ? ((num < CPU_SETSIZE && CPU_ISSET(num, &mask))
                                ? num
                                : getFirstCore(mask))
                         : resolveSentinelTarget(maskToCandidates(mask),
                                                 [tid](CpuNumT core) {
                                                     return pinTo(tid, core);
                                                 });

    cpu_set_t pinned;
    CPU_ZERO(&pinned);
    CPU_SET(target, &pinned);

    if (sched_setaffinity(tid, sizeof(pinned), &pinned) == 0) {
        pImpl()->update();                           // coherent: pinned probe
        sched_setaffinity(tid, sizeof(mask), &mask); // best-effort restore
    } else {
        pImpl()->update(); // race despite CPU_ISSET: best effort
    }
#elif defined(AU_TARGET_OS_IS_WINDOWS)
    // Mirror the Linux contract in Win32 terms: respect the calling THREAD's
    // affinity mask, resolve the target core deterministically (explicit core
    // if allowed else first core of the mask; for AU_CURRENT_CPU_NUM the first
    // core, or the crash-safe E-core on a hybrid mixed mask), pin, probe,
    // restore.
    HANDLE         thread = GetCurrentThread();
    GROUP_AFFINITY original{};

    // Read the thread's group affinity without mutating scheduling. If we
    // cannot read it, best-effort probe in place. Keep the whole GROUP_AFFINITY
    // so all pinning below is group-correct on >64-CPU (multi-group) machines.
    if (!readThreadAffinity(&original) || original.Mask == 0) {
        pImpl()->update();
        return;
    }

    const WORD      group = original.Group;
    const KAFFINITY mask  = original.Mask;

    // num is a group-relative core index (0..63 within the thread's group).
    CpuNumT target = (num != AU_CURRENT_CPU_NUM)
                         ? ((num < kAffinityBits
                             && (mask & (static_cast<KAFFINITY>(1) << num)))
                                ? num
                                : getFirstCore(mask))
                         : resolveSentinelTarget(maskToCandidates(mask),
                                                 [group](CpuNumT core) {
                                                     return pinTo(group, core);
                                                 });

    // resolveSentinelTarget may have left the thread pinned to the last probed
    // candidate; pin explicitly to the resolved target before the real probe.
    GROUP_AFFINITY pinned{};
    pinned.Group = group;
    pinned.Mask  = static_cast<KAFFINITY>(1) << target;
    if (SetThreadGroupAffinity(thread, &pinned, nullptr)) {
        pImpl()->update(); // coherent: pinned probe
        SetThreadGroupAffinity(thread, &original, nullptr); // restore
    } else {
        pImpl()->update(); // could not pin: best effort
    }
#else
    pImpl()->update();
#endif
}

StatusOr<X86Cpu>
X86Cpu::buildFromCore(CpuNumT num)
{
#if defined(AU_TARGET_OS_IS_LINUX) && !defined(__CYGWIN__)
    if (num != AU_CURRENT_CPU_NUM) {
        auto      tid = gettid();
        cpu_set_t mask;
        CPU_ZERO(&mask);
        if (sched_getaffinity(tid, sizeof(mask), &mask) != 0) {
            Status sts = StatusInvalidArgument(
                "X86Cpu::buildFromCore: failed to query thread affinity");
            return sts;
        }
        // Guard num before CPU_ISSET (it indexes a fixed-size mask).
        if (num >= CPU_SETSIZE || !CPU_ISSET(num, &mask)) {
            Status sts = StatusInvalidArgument(
                "X86Cpu::buildFromCore: requested core is outside the "
                "affinity mask");
            return sts;
        }
    }
#elif defined(AU_TARGET_OS_IS_WINDOWS)
    if (num != AU_CURRENT_CPU_NUM) {
        // Read the calling THREAD's group affinity without mutating scheduling
        // (GetThreadGroupAffinity), matching the Linux sched_getaffinity scope.
        // A failed query is a hard error for this strict factory rather than a
        // silent degrade. num is validated as a group-relative core index
        // against the thread's current group mask.
        GROUP_AFFINITY ga{};
        if (!readThreadAffinity(&ga)) {
            Status sts = StatusInvalidArgument(
                "X86Cpu::buildFromCore: failed to query thread affinity");
            return sts;
        }
        if (num >= kAffinityBits
            || (ga.Mask & (static_cast<KAFFINITY>(1) << num)) == 0) {
            Status sts = StatusInvalidArgument(
                "X86Cpu::buildFromCore: requested core is outside the "
                "affinity mask");
            return sts;
        }
    }
#endif
    // The requested core is within the thread's affinity mask (or the caller
    // asked for AU_CURRENT_CPU_NUM); construct via the best-effort constructor.
    return X86Cpu{ num };
}

X86Cpu::X86Cpu(X86Cpu&&) noexcept = default;

X86Cpu::~X86Cpu() = default;

void
X86Cpu::update()
{
    pImpl()->update();
}

bool
X86Cpu::isAMD() const
{
    return pImpl()->isAMD();
}

bool
X86Cpu::isX86_64v2() const
{
    return pImpl()->isX86_64v2();
}

bool
X86Cpu::isX86_64v3() const
{
    return pImpl()->isX86_64v3();
}

bool
X86Cpu::isX86_64v4() const
{
    return pImpl()->isX86_64v4();
}

bool
X86Cpu::isIntel() const
{
    return pImpl()->isIntel();
}

bool
X86Cpu::isAvailable(ECpuidFlag const& eflag) const
{
    return hasFlag(eflag);
}

bool
X86Cpu::hasFlag(ECpuidFlag const& eflag) const
{
    return pImpl()->hasFlag(eflag);
}

bool
X86Cpu::hasFlags(Au::Memory::BufferView<ECpuidFlag> const& eflags,
                 HasFlagsMode const&                       mode) const
{
    switch (mode) {
        case HasFlagsMode::Classic:
        case HasFlagsMode::All:
            for (auto flag : eflags)
                if (!hasFlag(flag))
                    return false;
            return true;
        case HasFlagsMode::Any:
            for (auto flag : eflags)
                if (hasFlag(flag))
                    return true;
            return false;
        default:
            return false;
    }
}

EUarch
X86Cpu::getUarch() const
{
    return pImpl()->getUarch();
}

bool
X86Cpu::isUarch(EUarch uarch, bool strict) const
{
    return pImpl()->isUarch(uarch, strict);
}

bool
X86Cpu::isZenFamily() const
{
    return pImpl()->isZenFamily();
}

VendorInfo
X86Cpu::getVendorInfo() const
{
    return pImpl()->getVendorInfo();
}
} // namespace Au
