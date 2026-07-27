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

#pragma once

#include "Au/Cpuid/X86Cpu.hh"
#include "Capi/au/cpuid/cpuid_core.h"

namespace Au {

using EFlag = ECpuidFlag;

/**
 * @brief Thin C++ wrapper over the pure-C CPUID core (cpuid_core.h).
 * @details Holds au_cpu_info_t handle; enum casts are identity. ABI: one pointer (unique_ptr<Impl>).
 */
class X86Cpu::Impl
{
  public:
    /* Test/mock constructor: routes core cpuid reads through injected CpuidUtils. */
    explicit Impl(CpuidUtils* cUtils);
    Impl();
    Impl(const Impl& other)            = default;
    Impl& operator=(const Impl& other) = default;
    ~Impl()                            = default;

    /* Resolve CPUID for cpu_num. Delegates all selection/pinning to au_cpuid_init.
     * Returns false only on strict failure (specific core out-of-mask). */
    bool update(int cpu_num, bool strict);

    /* Re-resolve on last cpu_num, non-strict. */
    bool update();

    bool isIntel() const;
    bool isAMD() const;

    bool isX86_64v2() const;
    bool isX86_64v3() const;
    bool isX86_64v4() const;

    bool hasFlag(EFlag const& ef) const;

    bool isZenFamily() const;

    EUarch     getUarch() const;
    bool       isUarch(EUarch uarch, bool strict = false) const;
    VendorInfo getVendorInfo() const;

  private:
    /* Trampoline: adapter letting C core call C++ CpuidUtils->__raw_cpuid(). */
    static void cutilsTrampoline(uint32_t         eax,
                                 uint32_t         ecx,
                                 au_cpuid_regs_t* out,
                                 void*            ctx);

    au_cpu_info_t m_info{};
    CpuidUtils*   m_cutils = nullptr; /* mock object, not owned */
};

} // namespace Au
