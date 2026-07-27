/*
 * Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
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

namespace Au {
/* ID return values */
struct CpuidRegs
{
    Uint32 eax;
    Uint32 ebx;
    Uint32 ecx;
    Uint32 edx;

    bool operator==(CpuidRegs const& Reg) const
    {
        return eax == Reg.eax && ebx == Reg.ebx && ecx == Reg.ecx
               && edx == Reg.edx;
    }

    /* Required for std::map key. Overflow harmless (ordering is insignificant). */
    bool operator<(CpuidRegs const& Reg) const
    {
        return eax + ebx + ecx + edx < Reg.eax + Reg.ebx + Reg.ecx + Reg.edx;
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
 *
 * @note  Mirror of the C enum @c au_vendor_t in
 *        SDK/Include/Capi/au/cpuid/cpuid_flags.h. The two MUST stay
 *        value-for-value identical (the C core and C++ wrapper cast between
 *        them). If you edit this enum, update au_vendor_t too, and vice versa.
 */
enum class EVendor : Uint32
{
    Amd = 1, /**< AMD. */
    Intel,   /**< Intel. */
    Other    /**< Others. */
};

/**
 * @brief Processor family info.
 *
 * @note  Mirror of the C enum @c au_family_t in
 *        SDK/Include/Capi/au/cpuid/cpuid_flags.h. The two MUST stay
 *        value-for-value identical. If you edit this enum, update au_family_t
 *        too, and vice versa.
 */
enum class EFamily : Uint16
{
    Unknown   = 0x0,
    Family17h = 0x17, /* Zen, Zen+, Zen2 */
    Family19h = 0x19, /* Zen3, Zen4      */
    Family1Ah = 0x1A, /* Zen5, Zen6      */
    Family1Bh = 0x1B, /* Zen6 (future)   */
    Max       = 0x1B, /* Always set to latest family ID */
};

class CpuidUtils
{
  public:
    virtual ~CpuidUtils() {}
    /**
     * \brief   Query CPUID instruction based on EAX (and sometimes ECX).
     * \param[in] req Request structure containing EAX, EBX, ECX, EDX values.
     * \param[out] resp regs pointer which has EAX, EBX, ECX, EDX values.
     */
    virtual ResponseT __raw_cpuid(RequestT& req);
    /**
     * \brief   Get CPU Vendor from CPUID (AMD, Intel, or Other).
     * \param[in]  ResponseT    The CPUID Register Data.
     * @return The EVendor [AMD, Intel or Other]
     */
    static EVendor getMfgInfo(ResponseT const& regs);
    /**
     * \brief       Get Family ID from given 32-bit value.
     * \details     Family[7:0] = ExtendedFamily[7:0] + BaseFamily[3:0].
     * \param[in]   var  32-bit value.
     * \return      Family ID.
     */
    static EFamily getFamily(Uint32 var);
    /**
     * \brief       Get Model number from given value.
     * \details     Model[7:0] = {ExtendedModel[3:0], BaseModel[3:0]}.
     * \param[in]   var  32-bit value.
     * \return      Model number.
     */
    static Uint16 getModel(Uint32 var);
    /**
     * \brief       Get Stepping ID from given value.
     * \param[in]   var  32-bit value.
     * \return      Stepping ID.
     */
    static Uint16 getStepping(Uint32 var);
    /**
     * @brief   Check if expected bits are set in actual CPUID response.
     * @param[in] expected Expected bit(s) to check.
     * @param[in] actual   Actual E{A,B,C,D}X after cpuid.
     * @return true if flag present, false otherwise.
     */
    static bool hasFlag(ResponseT const& expected, ResponseT const& actual);
    void        updateCacheView(CacheView& cView);
    static void updateCacheInfo(CacheInfo& cInfo, ResponseT const& resp);

    /**
     * @brief   Check if package is hybrid (leaf 0x7.0 EDX[15]).
     * @details Reports package-level heterogeneity; use getCoreType() for per-core type.
     * @return  true if Hybrid bit set, false otherwise.
     */
    bool isHybrid();

    /**
     * @brief   Get core type (leaf 0x1A.0 EAX[31:24]).
     * @details Per-core query; must run on target core. Intel: 0x40=P-core, 0x20=E-core.
     * @return  Raw core-type byte; 0 if unsupported.
     */
    Uint32 getCoreType();
};

/* Intel hybrid core-type values (CPUID.1A:EAX[31:24]). */
static constexpr Uint32 AU_X86_CORE_TYPE_ATOM = 0x20; /**< E-core. */
static constexpr Uint32 AU_X86_CORE_TYPE_CORE = 0x40; /**< P-core. */
} // namespace Au
