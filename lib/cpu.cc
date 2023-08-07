/*
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
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

#include "alci/cxx/cpu.hh"

namespace alci {

/**
 * \brief   Cpu constructor to initialize flags.
 *
 * \param[in] Cpu number.
 * 
 */
Cpu::Cpu(uint32_t cpu_num)
{
    /* TODO: switch to cpu_num before executing */

    for (int key = 0; key < ALC_CPUID_FLAG_MAX; key++) {
        auto k = cpuid_flags[key];
        auto value =
            static_cast<bool>((m_raw_data.m_cpuid[k.id][k.reg]) & k.flg);
        m_flags.insert({ static_cast<alc_flag_t>(key), value });
    }
}

/**
 * \brief   Get CPU manufacturing info from CPUID instruction.
 *
 * It gives CPU Vendor, Family, Model & Stepping.
 *
 * \return  Pointer to manufacturing info.
 */
const VendorInfo&
Cpu::getMfgInfo() const
{
    return m_raw_data.m_vendor_info;
}

/**
 * \brief   Get CPU vendor from CPUID instruction.
 *
 * \return  Vendor information.
 */
alci::Vendor
Cpu::getVendor() const
{
    auto mfg_info = getMfgInfo();

    return mfg_info.m_mfg;
}

/**
 * \brief   Checks for AMD CPU vendor from CPUID instruction.
 *
 * \return  Returns 1 if CPU vendor is AMD.
 */
uint32_t
Cpu::isAmd() const
{
    return (getVendor() == Vendor::eAmd);
}

/**
 * \brief   Get generation of CPU microarchitecture from CPUID instruction.
 *
 * Mapped values for Zen, Zen2, Zen3, etc.
 *
 * \return  Generation of CPU microarchitecture.
 */
alci::Uarch
Cpu::getUarch() const
{
    alci::Uarch ret = Uarch::eOther;

    if ((getVendor() != Vendor::eAmd))
        return ret;

    auto mfg_info = getMfgInfo();
    auto model    = mfg_info.m_model;

    /* For undefined latest family IDs, default to last known platform */
    if (mfg_info.m_family > ALC_CPU_FAMILY_MAX)
        ret = Uarch::eZen4; /* Always set to latest known Uarch*/

    switch (mfg_info.m_family) {

        case ALC_CPU_FAMILY_ZEN4:

            if (   /* ALC_CPU_FAMILY_ZEN4 */
                (0x10 <= model && model <= 0x1F)
                || // Genoa: Stones(RS), Storm Peak(STP)
                (0x60 <= model && model <= 0x6F) || // Raphael(RPL)
                (0x70 <= model && model <= 0x74) || // Phoenix(PHX)
                (0x78 <= model && model <= 0x7B) || // Phoenix 2(PHX2)
                (model == 0x80) ||                  // MI300C
                (0xA0 <= model && model <= 0xAF)    // Stones-Dense(RSDN)
            ) {
                ret = Uarch::eZen4;
                break;
            } else if (                   /* ALC_CPU_FAMILY_ZEN3 */
                       (model <= 0x0f) || // Milan: Genesis(GN), Chagall(CGL)
                       (0x20 <= model && model <= 0x2F) || // Vermeer(VMR)
                       (0x30 <= model && model <= 0x3F) || // Badami(BA)
                       (0x40 <= model && model <= 0x4F) || // Rembrandt(RMB)
                       (0x50 <= model && model <= 0x5F)    // Cezanne(CZN)
            ) {
                ret = Uarch::eZen3;
                break;
            } else {
                ret = Uarch::eZen4; /* Temporary support for unimplemented
                                       models under Zen4*/
                break;
            }

        case ALC_CPU_FAMILY_ZEN2:

            if (0x30 <= model && model <= 0xff) { /* ALC_CPU_FAMILY_ZEN2 */
                ret = Uarch::eZen2;
                break;
            } else { /* ALC_CPU_FAMILY_ZEN, ALC_CPU_FAMILY_ZEN_PLUS */
                ret = Uarch::eZen;
                break;
            }
        default: /* todo: add support for non Zen AMD platforms when required */
            break;
    }

    return ret;
}

/**
 * \brief   Checks microarchitecture from CPUID instruction and compare with
 * input.
 *
 * Like Zen, Zen2, Zen3 etc to be compared with specific input.
 *
 * \param[in] arch Microarchitecture input to check for.
 *
 * \return  Returns 1 if CPU microarchitecture is matched with input.
 */
bool
Cpu::isUarch(alci::Uarch arch) const
{
    return (getUarch() == arch);
}

/**
 * \brief   Updates Cpu class members if any.
 */
void
Cpu::update()
{
    m_raw_data.update();
}

/**
 * \brief   Checks availability of CPU flag from CPUID instruction.
 *
 * Like AVX, AVX512 etc.
 *
 * \param[in] cpuid_flag Input CPU flag.
 *
 * \return  Returns 1 if CPU microarchitecture has input CPU flag supported.
 *          Returns 0 otherwise.
 */
bool
Cpu::hasFlag(alc_flag_t const cpuid_flag)
{
    if (m_flags.find(cpuid_flag) != m_flags.end())
        return m_flags[cpuid_flag];

    return false;
}

/**
 * \brief   Checks availability of CPU flag from CPUID instruction.
 *
 * Like AVX, AVX512 etc.
 *
 * \param[in] cpuid_flag Input CPU flag.
 *
 * \return  Returns 1 if CPU microarchitecture has input CPU flag supported.
 *          Returns 0 otherwise.
 */
bool
Cpu::isAvailable(alc_flag_t const cpuid_flag)
{
   return hasFlag(cpuid_flag);
}

/**
 * \brief   Get Cache info using cache level.
 *
 * \param[in] level Cache level.
 *
 * \return  Returns Cache info.
 */
CacheInfo const&
Cpu::getCacheInfo(CacheLevel level) const
{
    if (level != CacheLevel::e_Level_Unknown)
        return m_cache_info[static_cast<uint32_t>(level)];

    return m_cache_info[0];
}

/**
 * \brief   Get last level cache (LLC).
 *
 * \return  Returns last level cache.
 */
CacheLevel
Cpu::getLastCacheLevel() const
{
    /* TODO: No mechanism to find last level cache using cpuid.
       Default returns to L3.*/
    return CacheLevel::e_Level_3;
}

} // namespace alci