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

#include "alci/alci.h"

#include "alci/cxx/alci.hh"
#include "alci/cxx/cpu.hh"

#include <cstring>
#include <sstream>

using namespace alci;

EXTERN_C_BEGIN

/**
 * \brief    Allows caller to check if the processor vendor is AMD.
 *
 * \return   1/true if processor vendor is AMD.
 */
bool
alcpu_is_amd(void)
{
    alci::Cpu c{};
    return (c.getVendor() == Vendor::eAmd);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN.
 *
 * \return  1/true if processor architecture is AMD ZEN.
 */
bool
alcpu_arch_is_zen(void)
{
    auto c = alci::Cpu();
    return c.isUarch(Uarch::eZen);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN2.
 *
 * \return  1/true if processor architecture is AMD ZEN2.
 */
bool
alcpu_arch_is_zen2(void)
{
    auto c = alci::Cpu();
    return c.isUarch(Uarch::eZen2);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN3.
 *
 * \return  1/true if processor architecture is AMD ZEN3.
 */
bool
alcpu_arch_is_zen3(void)
{
    auto c = alci::Cpu();
    return c.isUarch(Uarch::eZen3);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN4.
 *
 * \return  1/true if processor architecture is AMD ZEN4.
 */
bool
alcpu_arch_is_zen4(void)
{
    auto c = alci::Cpu();
    return c.isUarch(Uarch::eZen4);
}

/**
 * \brief   Allows caller to check if the flag is available.
 *
 * \param   flag   CPU feature flag.
 *
 * \return  1/true if feature is available.
 */
bool
alcpu_flag_is_available(alc_flag_t flag)
{
    auto c = alci::Cpu();
    return c.hasFlag(flag);
}

/******* NEW APIs */

/**
 * \brief    Allows caller to check if the processor vendor is AMD.
 *
 * \return   1/true if processor vendor is AMD.
 */
bool
alci_is_amd(alc_core_num_t cpu_num)
{
    alci::Cpu c{};
    return (c.getVendor() == Vendor::eAmd);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN.
 *
 * \return  1/true if processor architecture is AMD ZEN.
 */
bool
alci_arch_is_zen(alc_core_num_t cpu_num)
{
    alci::Cpu c{ cpu_num };
    return c.isUarch(Uarch::eZen);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN2.
 *
 * \return  1/true if processor architecture is AMD ZEN2.
 */
bool
alci_arch_is_zen2(alc_core_num_t cpu_num)
{
    alci::Cpu c{ cpu_num };
    return c.isUarch(Uarch::eZen2);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN3.
 *
 * \return  1/true if processor architecture is AMD ZEN3.
 */
bool
alci_arch_is_zen3(alc_core_num_t cpu_num)
{
    alci::Cpu c{ cpu_num };
    return c.isUarch(Uarch::eZen3);
}

/**
 * \brief   Allows caller to check if the processor arch is ZEN4.
 *
 * \return  1/true if processor architecture is AMD ZEN4.
 */
bool
alci_arch_is_zen4(alc_core_num_t cpu_num)
{
    alci::Cpu c{ cpu_num };
    return c.isUarch(Uarch::eZen4);
}

/**
 * \brief   Allows caller to check if the flag is available.
 *
 * \param   flag   CPU feature flag.
 *
 * \return  1/true if feature is available.
 */
bool
alci_cpu_has_flag(alc_core_num_t cpu_num, alc_flag_t flag)
{
    alci::Cpu c{ cpu_num };
    return c.hasFlag(flag);
}

/**
 * \brief   Get Cpu vendor info.
 *
 * \param[in]       cpu_num   CPU number.
 * \param[in, out]  arr       Vendor info array.
 * \param[in]       size      Size of Vendor info array.
 *
 * \return  1/true if getting vendor info is successful.
 */
alc_cpuid_error_t
alci_cpu_get_vendor(alc_core_num_t cpu_num, char* arr, size_t size)
{
    alci::Cpu          c{ cpu_num };
    std::ostringstream oss;
    oss << c.getVendor();

    std::snprintf(arr, size, "%s", oss.str().c_str());

    return ALC_E_SUCCESS;
}

/**
 * \brief   Get last level cache.
 *
 * \param[in] core  CPU core number.
 *
 * \return  Returns last level cache.
 */
uint32_t
alci_cpu_get_last_cache_level(alc_core_num_t core)
{
    Cpu c{core};
    
    return  static_cast<uint32_t>(c.getLastCacheLevel());
}

EXTERN_C_END
