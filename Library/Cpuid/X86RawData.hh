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

#pragma once

#include "Au/Cpuid/X86Cpu.hh"

#include <map>
#include <algorithm>

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
    Rembrandt = MAKE_MODEL(0x0, 0x4), /* 64 */
    Cezanne   = MAKE_MODEL(0x0, 0x5), /* 80 */

    /* Zen4 19H*/
    Genoa       = MAKE_MODEL(0x1, 0x1), /* 17 */
    Stormpeak   = MAKE_MODEL(0x8, 0x1), /* 24 */
    Warhol      = MAKE_MODEL(0x1, 0x2), /* 33 */
    Raphael     = MAKE_MODEL(0x1, 0x6), /* 97 */
    Pheonix     = MAKE_MODEL(0x5, 0x7), /* 117 */
    Phenixpoint = MAKE_MODEL(0x8, 0x7), /* 120 */

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
        , m_cutils{ cUtils }
        , m_vendor_info{}
        , m_cache_view{}
    {
    }
    Impl()
        : m_avail_flags{}
        , m_usable_flags{}
        , m_cutils{ new CpuidUtils{} }
        , m_vendor_info{}
        , m_cache_view{}
    {
    }
    Impl(const Impl& other)            = default;
    Impl& operator=(const Impl& other) = default;
    ~Impl()                            = default;

    void update();

    bool isIntel() const;
    bool isAMD() const;

    bool isX86_64v2() const;
    bool isX86_64v3() const;
    bool isX86_64v4() const;

    bool hasFlag(EFlag const& ef) const;

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
     * based on the Family model and stepping values
     */
    void setUarch()
    {
        /**
         * As m_vendor_info.m_family will only hold the minimum
         * value of the family @see getFamily() ID. Eg: Zen, ZenPlus and zen2
         * has same family ID. Even if the CPU belongs to the Zen2 family, it
         * holds EFamily::Zen hence if checks can look only for the smallest
         * family ID in the group and rest can be skipped.
         */
        if (m_vendor_info.m_family == EFamily::Zen) {
            switch (m_vendor_info.m_model) {
                case *EUModel::Naples:
                case *EUModel::Ravenridge:
                    m_vendor_info.m_uarch = EUarch::Zen;
                    break;
                    // case *EUModel::Bandedkestrel:
                case *EUModel::Picasso:
                    if (m_vendor_info.m_stepping == 1)
                        m_vendor_info.m_uarch = EUarch::ZenPlus;
                    else
                        m_vendor_info.m_uarch = EUarch::Zen;
                    break;
                case *EUModel::Pinnacleridge:
                    m_vendor_info.m_uarch = EUarch::ZenPlus;
                    break;
                case *EUModel::Rome:
                case *EUModel::Castlepeakpro:
                case *EUModel::Renoir:
                case *EUModel::Matisse:
                case *EUModel::Vangogh:
                case *EUModel::Mendocino:
                    m_vendor_info.m_uarch = EUarch::Zen2;
                    break;
            }

        } else if (m_vendor_info.m_family == EFamily::Zen3) {

            switch (m_vendor_info.m_model) {
                case *EUModel::Milan:
                case *EUModel::Chagall:
                    m_vendor_info.m_uarch = EUarch::Zen3;
                    break;
                // case *EUModel::Vermeer:
                case *EUModel::Warhol:
                    if (m_vendor_info.m_stepping == 2)
                        m_vendor_info.m_uarch = EUarch::Zen4;
                    else
                        m_vendor_info.m_uarch = EUarch::Zen3;
                    break;
                case *EUModel::Rembrandt:
                case *EUModel::Cezanne:
                    m_vendor_info.m_uarch = EUarch::Zen3;
                    break;
                case *EUModel::Genoa:
                case *EUModel::Stormpeak:
                case *EUModel::Raphael:
                case *EUModel::Pheonix:
                case *EUModel::Phenixpoint:
                    m_vendor_info.m_uarch = EUarch::Zen4;
                    break;
            }
        } else if (m_vendor_info.m_family == EFamily::Zen5) {
            m_vendor_info.m_uarch = EUarch::Zen5;
        } else {

            m_vendor_info.m_uarch = EUarch::Unknown;
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
};

} // namespace Au
