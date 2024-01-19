/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#include "Au/Assert.hh"
#include "Au/Au.hh"
#include "Au/Cpuid/X86Cpu.hh"
#include "Au/Cpuid/CpuidUtils.hh"

#include <map>

namespace Au {

using EFlag = ECpuidFlag;

/** \enum  ECpuidReg
 *  \brief CPUID registers.
 */
enum class ECpuidReg
{
    Eax = 0, /**< EAX register. */
    Ebx,     /**< EBX register. */
    Ecx,     /**< ECX register. */
    Edx      /**< EDX register. */
};
using EReg = ECpuidReg;

/**
 * @brief   Updates CPU vendor info internal.
 *
 * @details
 * Initialize CPU Vendor info with Vendor name, manufacturing info and
 * all CPUID results with different eax, ecx register values.
 * It gives CPU Family, Model & Stepping.
 *
 */
class X86Cpu::Impl
{

  public:
    Impl()
        : m_vendor_info{}
        , m_avail_flags{}
        , m_usable_flags{}
        , m_cache_view{}
    {
    }

    bool isAMD() const;

    bool isX86_64v2() const;
    bool isX86_64v3() const;
    bool isX86_64v4() const;

    bool isIntel() const;

    bool hasFlag(EFlag const& ef) const;

    /**
     * @brief  Update Raw Data by re-reading the registers
     *
     * @return None
     */
    void update();

    /**
     * @brief       Get CPUID output based on eax, ecx register values as
     * input.
     *
     * @param[in]   eax   EAX register value.
     * @param[in]   ecx   ECX register value.
     *
     * @return      Returns register values after performing cpuid.
     */
    ResponseT at(RequestT& req) const;

    /**
     * @brief Helper to manually override some settings
     *
     * @param[in]       regs  Request registers to be given to cpuid instruction
     * @param[in]       resp  Corresponding response of 'regs'
     *
     * @return     void
     */
    void apply(RequestT& regs, ResponseT const& resp);

    /**
     * @brief Setter for usable flag to override, by default usable = available
     *
     * @param[in]       flg     EFlag that needs to be updated
     * @param[in]       res     Result (true/false) to be updated with
     *
     * @return     void
     */
    void setUsableFlag(EFlag const& flg, bool res = true);

  private:
    bool isUsable(EFlag const& flg) const
    {
        auto __usable = m_usable_flags.find(flg);
        if (__usable != m_usable_flags.end())
            return __usable->second;

        return false;
    }

    /* All or None flags check */
    bool isUsable(std::vector<EFlag> const& __arr) const
    {
        for (auto& i : __arr) {
            if (!isUsable(i))
                return false;
        }
        return true;
    }

    void updateflag(EFlag const& flg, bool res = true)
    {
        m_avail_flags[flg] = m_usable_flags[flg] = res;
    }

    VendorInfo m_vendor_info;

    /*
     * FIXME : Eventually change to a bitmap
     */
    std::map<EFlag, bool> m_avail_flags;
    std::map<EFlag, bool> m_usable_flags;

    CacheView m_cache_view;
};

static inline void
__raw_cpuid(RequestT& req, ResponseT& resp)
{
    if (req.eax == 0x00000007) {
        asm volatile(
            "cpuid"
            : "=a"(resp.eax), "=b"(resp.ebx), "=c"(resp.ecx), "=d"(resp.edx)
            : "0"(req.eax), "2"(req.ecx));

    } else {
        asm volatile(
            "cpuid"
            : "=a"(resp.eax), "=b"(resp.ebx), "=c"(resp.ecx), "=d"(resp.edx)
            : "0"(req.eax));
    }
}

static inline ResponseT
__raw_cpuid(RequestT& req)
{
    ResponseT resp;
    __raw_cpuid(req, resp);

    return resp;
}

} // namespace Au
