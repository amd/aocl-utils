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
#include "X86RawData.hh"

#include <cstdint>

namespace Au {

/* Thin decoder over the pure-C core (cpuid_core.h). Enum casts are identity. */

void
X86Cpu::Impl::cutilsTrampoline(uint32_t         eax,
                               uint32_t         ecx,
                               au_cpuid_regs_t* out,
                               void*            ctx)
{
    CpuidUtils* u = static_cast<CpuidUtils*>(ctx);
    RequestT    req{ eax, 0, ecx, 0 };
    ResponseT   r = u->__raw_cpuid(req);
    out->eax      = r.eax;
    out->ebx      = r.ebx;
    out->ecx      = r.ecx;
    out->edx      = r.edx;
}

X86Cpu::Impl::Impl(CpuidUtils* cUtils)
    : m_cutils{ cUtils }
{
    m_info.raw_fn  = &X86Cpu::Impl::cutilsTrampoline;
    m_info.raw_ctx = m_cutils;
}

X86Cpu::Impl::Impl() = default;

bool
X86Cpu::Impl::update(int cpu_num, bool strict)
{
    /* Real-HW selection/pinning in au_cpuid_init; mocks (raw_fn set) skip it. */
    return au_cpuid_init(&m_info, cpu_num, strict);
}

bool
X86Cpu::Impl::update()
{
    /* Re-resolve on stored cpu_num (sentinel included), non-strict. */
    return au_cpuid_init(&m_info, m_info.cpu_num, false);
}

bool
X86Cpu::Impl::isIntel() const
{
    return au_cpuid_info_is_intel(&m_info);
}

bool
X86Cpu::Impl::isAMD() const
{
    return au_cpuid_info_is_amd(&m_info);
}

bool
X86Cpu::Impl::isX86_64v2() const
{
    return au_cpuid_info_is_x86_64v2(&m_info);
}

bool
X86Cpu::Impl::isX86_64v3() const
{
    return au_cpuid_info_is_x86_64v3(&m_info);
}

bool
X86Cpu::Impl::isX86_64v4() const
{
    return au_cpuid_info_is_x86_64v4(&m_info);
}

bool
X86Cpu::Impl::hasFlag(EFlag const& ef) const
{
    return au_cpuid_info_has_flag(&m_info, static_cast<uint16_t>(ef));
}

bool
X86Cpu::Impl::isZenFamily() const
{
    return au_cpuid_info_is_zen_family(&m_info);
}

EUarch
X86Cpu::Impl::getUarch() const
{
    return static_cast<EUarch>(au_cpuid_info_get_uarch(&m_info));
}

bool
X86Cpu::Impl::isUarch(EUarch uarch, bool strict) const
{
    return au_cpuid_info_is_uarch(
        &m_info, static_cast<au_uarch_t>(uarch), strict);
}

VendorInfo
X86Cpu::Impl::getVendorInfo() const
{
    VendorInfo v;
    v.m_mfg      = static_cast<EVendor>(m_info.vendor);
    v.m_family   = static_cast<EFamily>(m_info.family);
    v.m_model    = m_info.model;
    v.m_stepping = m_info.stepping;
    v.m_uarch    = static_cast<EUarch>(m_info.uarch);
    return v;
}

} // namespace Au
