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

namespace Au {

/*
 * All affinity logic (selection, pinning, hybrid E-core, multi-group) now lives in
 * the pure-C core resolver (au_cpuid_init). X86Cpu is a pure decoder that forwards
 * cpu_num to the core. AU_CURRENT_CPU_NUM (UINT32_MAX) casts to -1, matching the
 * core's sentinel.
 */

X86Cpu::X86Cpu(CpuidUtils* cUtils, CpuNumT num)
    : CpuInfo{ num }
    , m_pimpl{ new X86Cpu::Impl{ cUtils } }
{
    /* MOCK path: core skips selection/pinning. Non-strict. */
    m_resolved = pImpl()->update((int)num, /*strict=*/false);
}

X86Cpu::X86Cpu(CpuNumT num)
    : X86Cpu{ num, /*strict=*/false }
{
}

X86Cpu::X86Cpu(CpuNumT num, bool strict)
    : CpuInfo{ num }
    , m_pimpl{ new X86Cpu::Impl{} }
{
    /* C core handles all selection/pinning/restore. strict=false degrades; strict=true fails on out-of-mask. */
    m_resolved = pImpl()->update((int)num, strict);
}

StatusOr<X86Cpu>
X86Cpu::buildFromCore(CpuNumT num)
{
    /* Strict resolve: out-of-mask core returns InvalidArgument. */
    X86Cpu cpu{ num, /*strict=*/true };
    if (!cpu.m_resolved) {
        Status sts = StatusInvalidArgument(
            "X86Cpu::buildFromCore: requested core is outside affinity mask");
        return sts;
    }
    return cpu;
}

X86Cpu::X86Cpu(X86Cpu&&) noexcept = default;

X86Cpu::~X86Cpu() = default;

void
X86Cpu::update()
{
    /* Re-resolve non-strict on stored cpu_num (core owns affinity). */
    m_resolved = pImpl()->update();
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
