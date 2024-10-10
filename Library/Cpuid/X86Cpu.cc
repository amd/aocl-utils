/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

#include "X86RawData.hh"
#include <thread>
#ifdef __linux__
#include <unistd.h>
#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif
#else
#include <Windows.h>
#include <direct.h>
#include <io.h>
#endif

namespace Au {

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
    /* switch to the correct cpunum,
     * using sched_setaffinity() */

    auto nthreads = std::thread::hardware_concurrency();
    if (num != AU_CURRENT_CPU_NUM)
        AUD_ASSERT(num < nthreads, "Invalid Cpuid Number");
    if (num > nthreads)
        num = AU_CURRENT_CPU_NUM;    // fallback to default behaviour
    if (num != AU_CURRENT_CPU_NUM) { // In the default behaviour, the cpuid is
                                     // quried on the current cpu.
#ifdef __linux__
        cpu_set_t newMask, currentMask, testMask;
        auto      tid = gettid();
        int result    = sched_getaffinity(tid, sizeof(cpu_set_t), &currentMask);

        AUD_ASSERT(result == 0, "Failed to get thread affinity.");
        if (result)
            std::cout << "Failed to get thread affinity" << std::endl;
        CPU_ZERO(&newMask);
        CPU_SET(num, &newMask);
        result = sched_setaffinity(tid, sizeof(cpu_set_t), &newMask);
        if (result)
            std::cout << "Failed to set thread affinity" << std::endl;
        AUD_ASSERT(result == 0, "Failed to set thread affinity.");
        sched_getaffinity(tid, sizeof(cpu_set_t), &testMask);

#else
        DWORD threadId    = GetCurrentThreadId();
        auto  mask        = (static_cast<DWORD_PTR>(1) << num);
        auto  currentMask = SetThreadAffinityMask(&threadId, mask);
#endif
        pImpl()->update();
#ifdef __linux__
        result = sched_setaffinity(tid, sizeof(cpu_set_t), &currentMask);
        AUD_ASSERT(result == 0, "Failed to set thread affinity.");
        if (result)
            std::cout << "Failed to set thread affinity" << std::endl;
#else
        auto newMask = SetThreadAffinityMask(&threadId, currentMask);
#endif
    } else {
        pImpl()->update();
    }
}

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
VendorInfo
X86Cpu::getVendorInfo() const
{
    return pImpl()->getVendorInfo();
}
} // namespace Au
