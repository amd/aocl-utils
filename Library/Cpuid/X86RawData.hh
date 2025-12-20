/*
 * Copyright (C) 2023-2025, Advanced Micro Devices. All rights reserved.
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
#include "Au/Misc.hh"

#include <algorithm>
#include <map>

namespace Au {

/* Precise identification */
#define MAKE_MODEL(base, ext) (((ext) << 4) | (base))

enum class EUModel : Uint8 // NOLINT
{
    /* Zen 17H */
    Zen_Min = MAKE_MODEL(0x0, 0x0), /* 0 */
    /* Zen, Naples, Whitehaven, Summit Ridge, Snowy Owl */
    Naples = MAKE_MODEL(0x1, 0x0), /* 1 */
    /* Ravenridge Great Horned Owl */
    Ravenridge    = MAKE_MODEL(0x1, 0x1), /* 17 */
    Bandedkestrel = MAKE_MODEL(0x8, 0x1), /* 24 */

    /* Zen+ 17H*/
    /* Pinnacleridge Colfax*/
    Pinnacleridge = MAKE_MODEL(0x8, 0x0), /* 8 */
    Picasso       = MAKE_MODEL(0x8, 0x1), /* 24 */

    /* Zen2 17H*/
    /* Rome Castle peak*/
    Rome          = MAKE_MODEL(0x1, 0x3), /* 49 */
    Castlepeakpro = MAKE_MODEL(0x7, 0x4), /* 71 */
    /* Renoir Grey Hawk */
    Renoir    = MAKE_MODEL(0x0, 0x6), /* 96 */
    Matisse   = MAKE_MODEL(0x1, 0x7), /* 113 */
    Vangogh   = MAKE_MODEL(0x0, 0x9), /* 144 */
    Mendocino = MAKE_MODEL(0x0, 0xA), /* 160 */

    /* Zen3 19H*/
    Milan     = MAKE_MODEL(0x1, 0x0), /* 1 */
    Chagall   = MAKE_MODEL(0x8, 0x0), /* 08 */
    Vermeer   = MAKE_MODEL(0x1, 0x2), /* 33 */
    Rembrandt = MAKE_MODEL(0x4, 0x4), /* 68 */
    Cezanne   = MAKE_MODEL(0x0, 0x5), /* 80 */

    /* Zen4 19H*/
    Genoa        = MAKE_MODEL(0x1, 0x1), /* 17 */
    Stormpeak    = MAKE_MODEL(0x8, 0x1), /* 24 */
    Warhol       = MAKE_MODEL(0x1, 0x2), /* 33 */
    Raphael      = MAKE_MODEL(0x1, 0x6), /* 97 */
    Phoenix1     = MAKE_MODEL(0x4, 0x7), /* 116 */
    Phoenixpoint = MAKE_MODEL(0x5, 0x7), /* 117 */

    /* Zen5 - Wikipedia */

    Turin        = MAKE_MODEL(0x0, 0x2), /* 1 */
    TurinD       = MAKE_MODEL(0x1, 0x0), /* 16 */
    GraniteRidge = 0x0,                  /* desktop */
    StrixPoint   = 0x0,                  /* mobile processors  */
};

using EFlag = ECpuidFlag;

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
    explicit Impl(CpuidUtils* cUtils)
        : m_avail_flags{}
        , m_usable_flags{}
        , m_cutils{ std::move(cUtils) }
        , m_vendor_info{}
        , m_cache_view{}
        , m_is_mock(true)
    {
    }
    Impl()
        : m_avail_flags{}
        , m_usable_flags{}
        , m_cutils{ new CpuidUtils{} }
        , m_vendor_info{}
        , m_cache_view{}
        , m_is_mock(false)
    {
    }
    Impl(const Impl& other)            = default;
    Impl& operator=(const Impl& other) = default;
    ~Impl()
    {
        if (m_is_mock == false)
            delete m_cutils;
    }

    void update();

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
    void apply(RequestT& regs);

    /**
     * @brief Setter for usable flag to override, by default usable = available
     *
     * @param[in]       flag     EFlag that needs to be updated
     * @param[in]       res     Result (true/false) to be updated with
     *
     * @return     void
     */
    void setUsableFlag(EFlag const& flag, bool res = true);

  private:
    /**
     *  @brief Check if a cpuid flag is usable
     *
     * @param[in] flag  EFlag to be checked
     *
     * @return bool
     */
    bool isUsable(EFlag const& flag) const { return m_usable_flags.at(flag); }
    /**
     * @brief Check if all of the cpuid flag is available
     *
     * @param[in] featureArr  Vector of EFlag to be checked
     *
     * @return bool
     */
    bool isUsable(std::vector<EFlag> const& featureArr) const
    {
        return std::all_of(
            featureArr.begin(), featureArr.end(), [this](EFlag const& flag) {
                return isUsable(flag);
            });
    }
    /**
     * @brief Enable/Disable a cpuid flag
     * @param[in] flag  EFlag to be updated
     * @param[in] res   Result (true/false) to be updated with
     *
     * @return void
     */
    void updateflag(EFlag const& flag, bool res = true)
    {
        m_avail_flags[flag] = m_usable_flags[flag] = res;
    }
    /**
     * @brief Update the microarchitecture of CPU in the m_vendor_info structure
     * based on the Family model and stepping values.
     *
     * This implementation follows GCC's approach using model number ranges
     * for detecting the microarchitecture within each family.
     * When model numbers don't match known ranges, feature flags are used
     * as a fallback detection mechanism (matching GCC's get_amd_cpu).
     */
    void setUarch()
    {
        // Only set microarchitecture for AMD CPUs
        if (m_vendor_info.m_mfg != EVendor::Amd) {
            m_vendor_info.m_uarch = EUarch::Unknown;
            return;
        }

        Uint16 model = m_vendor_info.m_model;

        switch (m_vendor_info.m_family) {
            case EFamily::Zen: // Family 0x17 (Zen, Zen+, Zen2)
                if (model <= 0x1f) {
                    // Zen1 - Naples, Whitehaven, Summit Ridge, Snowy Owl,
                    // Ravenridge
                    m_vendor_info.m_uarch = EUarch::Zen;
                } else if (model >= 0x30) {
                    // Zen2 - Rome, Castle Peak, Renoir, Matisse, Vangogh,
                    // Mendocino
                    m_vendor_info.m_uarch = EUarch::Zen2;
                } else {
                    // Models 0x20-0x2f - use feature flag fallback (GCC style)
                    // CLWB is present on Zen2+, absent on Zen1
                    if (m_avail_flags[EFlag::clwb]) {
                        m_vendor_info.m_uarch = EUarch::Zen2;
                    } else if (m_avail_flags[EFlag::clzero]) {
                        // CLZERO is present on Zen1+
                        m_vendor_info.m_uarch = EUarch::Zen;
                    } else {
                        m_vendor_info.m_uarch = EUarch::Zen;
                    }
                }
                break;

            case EFamily::Zen3: // Family 0x19 (Zen3, Zen4)
                if (model <= 0x0f) {
                    // Zen3 - Milan
                    m_vendor_info.m_uarch = EUarch::Zen3;
                } else if ((model >= 0x10 && model <= 0x1f)
                           || (model >= 0x60 && model <= 0xaf)) {
                    // Zen4 - Genoa, Raphael, Phoenix
                    m_vendor_info.m_uarch = EUarch::Zen4;
                } else {
                    // Unknown 0x19 models - use feature flag fallback (GCC
                    // style) AVX512F is present on Zen4, absent on Zen3
                    if (m_avail_flags[EFlag::avx512f]) {
                        m_vendor_info.m_uarch = EUarch::Zen4;
                    } else if (m_avail_flags[EFlag::vaes]) {
                        m_vendor_info.m_uarch = EUarch::Zen3;
                    } else {
                        m_vendor_info.m_uarch = EUarch::Zen3;
                    }
                }
                break;

            case EFamily::Zen5: // Family 0x1A (Zen5)
                // TODO: Add Zen6 detection when model ranges are known

                // FIXME: Enable the below code when Zen6 model ranges are
                // known, for now we return 0x1A as Zen5
                /*
                if (model <= 0x4f || (model >= 0x60 && model <= 0x77)
                    || (model >= 0xd0 && model <= 0xd7)) {
                    // Zen5 - Turin, Granite Ridge, Strix Point
                    m_vendor_info.m_uarch = EUarch::Zen5;
                } else {
                    // Unknown 0x1A models - use feature flag fallback (GCC
                    // style) AVX512_VPINTERSECT is present on Zen5
                    if (m_avail_flags[EFlag::avx512_vpintersect]) {
                        m_vendor_info.m_uarch = EUarch::Zen5;
                    } else {
                        m_vendor_info.m_uarch = EUarch::Zen5;
                    }
                }
                */
                m_vendor_info.m_uarch = EUarch::Zen5;
                break;

            default:
                m_vendor_info.m_uarch = EUarch::Unknown;
                break;
        }

        // Fallback for future AMD families beyond current Max
        if ((m_vendor_info.m_uarch == EUarch::Unknown)
            && (m_vendor_info.m_family > EFamily::Max)) {
            // Assuming Family increases each generation, set to Zen5 for future
            // AMD families (vendor check is done at the start of this function)
            m_vendor_info.m_uarch = EUarch::Zen5;
        }
    }
    /*
     * FIXME : Eventually change to a bitmap
     */
    std::map<EFlag, bool> m_avail_flags;
    std::map<EFlag, bool> m_usable_flags;
    CpuidUtils*           m_cutils;
    VendorInfo            m_vendor_info;
    CacheView             m_cache_view;
    bool                  m_is_mock; // Flag to recongnize gmock object.
};

} // namespace Au
