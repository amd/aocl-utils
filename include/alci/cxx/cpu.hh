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
#pragma once

#include "alci/cxx/alci.hh"
#include "alci/cxx/cache.hh"
#include "alci/cxx/cpuidrawdata.hh"
#include "alci/macros.h"

namespace alci {

/**
 * \class   Cpu
 * \brief   Class to get CPU info.
 *
 * Details include core vendor, Micro arch, features supported.
 */
class Cpu
{
  public:
    typedef std::map<alc_flag_t, bool> flagmap;

  private:
    // Cache hierarchy as seen by each cpu
    CacheInfo m_cache_info[5];

    /**< Flagmap is boolean type to contain flags of CPUID
     * features.
     */
    flagmap m_flags; /**< Stores CPUID feature flags. */
    
    CpuIdRawData m_raw_data; /**< To hold cpuid raw data.*/

  public:
    /**
     * \brief   Cpu constructor to initialize flags.
     *
     * \param[in] Cpu number.
     * 
     */    
    explicit Cpu(uint32_t cpu_num = 0);
    /**
     * \brief   Get Cache info using cache level.
     *
     * \param[in] level Cache level.
     *
     * \return  Returns Cache info.
     */
    CacheInfo const& getCacheInfo(CacheLevel level) const;
    
    /**
     * \brief   Get last level cache (LLC).
     *
     * \return  Returns last level cache.
     */
    CacheLevel getLastCacheLevel() const;

    /**
     * \brief   Updates Cpu class members if any.
     */
    void update();

    /**
     * \brief   Get CPU manufacturing info from CPUID instruction.
     *
     * It gives CPU Vendor, Family, Model & Stepping.
     *
     * \return  Pointer to manufacturing info.
     */
    const VendorInfo& getMfgInfo() const;

    /**
     * \brief   Get CPU vendor from CPUID instruction.
     *
     * \return  Vendor information.
     */
    alci::Vendor getVendor() const;

    /**
     * \brief   Get generation of CPU microarchitecture from CPUID instruction.
     *
     * Mapped values for Zen, Zen2, Zen3, etc.
     *
     * \return  Generation of CPU microarchitecture.
     */
    alci::Uarch getUarch() const;

    /**
     * \brief   Checks for AMD CPU vendor from CPUID instruction.
     *
     * \return  Returns 1 if CPU vendor is AMD.
     */
    uint32_t isAmd() const;

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
    bool isUarch(alci::Uarch arch) const;

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
    bool isAvailable(alc_flag_t const cpuid_flag);
    
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
    bool hasFlag(alc_flag_t const cpuid_flag);
};

} // namespace alci