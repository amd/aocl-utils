/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#include "Au/Au.hh"
#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Types.hh"

#include <iostream>
#include <type_traits>
using namespace std;

namespace Au {
/* ID return values */
struct CpuidRegs
{
    Uint32 eax;
    Uint32 ebx;
    Uint32 ecx;
    Uint32 edx;

    /* following are required for making this key in a std::map */
    bool operator==(CpuidRegs const& Reg) const
    {
        return eax == Reg.eax && ebx == Reg.ebx && ecx == Reg.ecx
               && edx == Reg.edx;
    }

    bool operator<(CpuidRegs const& Reg) const
    {
        return eax < Reg.eax || ebx < Reg.ebx || ecx < Reg.ecx || edx < Reg.edx;
    }

    CpuidRegs const operator&(CpuidRegs const& Reg) const
    {
        return {
            (eax & Reg.eax), (ebx & Reg.ebx), (ecx & Reg.ecx), (edx & Reg.edx)
        };
    }
};
using RequestT  = const CpuidRegs;
using ResponseT = CpuidRegs;

using CacheLevel = CacheInfo::CacheLevel;
using CacheType  = CacheInfo::CacheType;
/**
 * @enum  Vendor
 * @brief CPU vendors.
 */
enum class EVendor : Uint32
{
    Amd = 1, /**< AMD. */
    Intel,   /**< Intel. */
    Other    /**< Others. */
};

/* Processor family info */
enum class EFamily : Uint16
{
    Unknown  = 0x0,
    Zen      = 0x17,
    Zen_Plus = 0x17,
    Zen2     = 0x17,
    Zen3     = 0x19,
    Zen4     = 0x19,
    Zen5     = 0xA0,
    Max      = 0xA0, /* Always set to latest family ID */
};

class CpuidUtils
{
  public:
    virtual ~CpuidUtils() {}
    /**
     * \brief   Function to query CPUID instruction based on EAX input
     * parameter.
     *
     * Function which has assembly code to query CPUID instruction
     * based on EAX input value. EAX should have valid value to
     * get CPUID information and other EBX, ECX, EDX should have 0s.
     * In some cases, ECX also will have valid values.
     *
     * \param[in] req Request structure containing EAX, EBX, ECX, EDX values.
     * \param[out] resp regs pointer which has EAX, EBX, ECX, EDX values.
     */
    virtual ResponseT __raw_cpuid(RequestT& req);
    /**
     * \brief   Get CPU Vendor info from CPUID instruction.
     *
     * It identifies AMD and Intel Cpus. Tags anyother vendor as Other.
     *
     * \param[in]  ResponseT    The CPUID Register Data.
     * @return The EVendor [AMD, Intel or Other]
     */
    EVendor getMfgInfo(ResponseT const& regs);
    /**
     * \brief       Get Family ID from given 32-bit input value.
     *
     * Family[7:0] = (ExtendedFamily[7:0] + {0000b,BaseFamily[3:0]})
     * where ExtendedFamily[7:0] = var[27:20], BaseFamily[3:0] = var[11, 8]
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Family ID value.
     * Note: Returns the minimum value of the family ID.
     * Eg: Zen, ZenPlus and zen2 has same family ID. even if the CPU belongs to
     * the Zen2 family, it returns Zen2 the function return Zen
     */
    EFamily getFamily(Uint32 var);
    /**
     * \brief       Get Model number from given input value.
     *
     *  Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
     *  where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Model number value.
     */
    Uint16 getModel(Uint32 var);
    /**
     * \brief       Get Stepping ID from given input value.
     *
     * Get Stepping ID from given input value.
     * Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
     * where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
     * For now, model number is stepping. TODO: need to revisit for each family.
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Stepping ID value.
     */
    Uint16 getStepping(Uint32 var);
    /**
     * @details Issues the cpuid instruction using EAX/ECX gets response and
     * checks a flag in appropriate register
     *
     * @param[in] expected Expected bit(s) to check
     *
     * @param[in] actual   Actula value of E{A,B,C,D}X after 'cpuid' issued
     *
     * @return true if cpu has flag, false otherwise
     */
    bool hasFlag(ResponseT const& expected, ResponseT const& actual);
    void updateCacheView(CacheView& cView);
    void updateCacheInfo(CacheInfo& cInfo, ResponseT const& resp);
};
} // namespace Au
