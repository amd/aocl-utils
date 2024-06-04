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

#ifndef __AU_CPUID_CPUID_H__
#define __AU_CPUID_CPUID_H__

#include "Au/Config.h"
#include "Au/Defs.hh"
#include "Capi/au/au.h"

#include <stdbool.h>
#include <stdint.h>

AUD_EXTERN_C_BEGIN

#if !defined(au_cpu_num_t)
typedef Uint32 au_cpu_num_t;
typedef Uint32 au_cpu_flag_t;
#endif

/**
 * @brief          Check if the processor vendor is AMD.
 *
 * @param[in]      cpu_num  CPU number.
 *
 * @return         1/true if processor vendor is AMD.
 */
AUD_API_EXPORT bool
au_cpuid_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief          Get Cpu vendor info.
 *
 * @param[in]      cpu_num   CPU number.
 * @param[in, out] vend_info Vendor info array.
 * @param[in]      size      Size of Vendor info array.
 *
 * @return         None
 */
AUD_API_EXPORT void
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size);

/**
 * @brief          Allows caller to check if the processor arch is ZEN.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZENPLUS.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZENPLUS.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zenplus(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN2.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN2.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN3.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN3.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN4.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN4.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN5.
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN5.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the flag is available.
 *
 * @param[in]      cpu_num   CPU number.
 * @param[in]      flag_array  CPU feature flag names.
 * @param[in]      count   Number of flags in the list.
 *
 * @return         an array indicating  feature availability
 */
AUD_API_EXPORT bool*
au_cpuid_has_flag(au_cpu_num_t      cpu_num,
                  const char* const flag_array[],
                  int               count);

/**
 * @brief          Portable API to check if an error has occured
 *
 * @param          err  Actual error number
 *
 * @return         1/true if error occorured, false otherwise.
 */
AUD_API_EXPORT bool
au_cpuid_is_error(au_error_t err);

/******* OLD APIs ********/

/**
 * @brief          Allows caller to check if the processor vendor is AMD.
 *
 *                 Note: The api is deprecated. Use au_cpuid_is_amd
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor vendor is AMD.
 */
AU_DEPRECATED_API_X("Use au_cpuid_is_amd instead.")
AUD_API_EXPORT bool
alci_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief          Get Cpu vendor info.
 *
 * @param[in]      cpu_num   CPU number.
 * @param[out]     arr       Vendor info array.
 * @param[in]      size      Size of Vendor info array.
 *
 * @return         1/true if getting vendor info is successful.
 */
AU_DEPRECATED_API_X("Use alci_cpu_get_vendor instead.")
AUD_API_EXPORT au_error_t
alci_cpu_get_vendor(au_cpu_num_t cpu_num, char* arr, size_t size);

/**
 * @brief          Allows caller to check if the processor arch is ZEN.
 *
 *                 Note: The api is deprecated. Use au_cpuid_arch_is_zen
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen instead.")
AUD_API_EXPORT bool
alci_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN2.
 *
 *                 Note: The api is deprecated. Use au_cpuid_arch_is_zen2
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN2.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen2 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN3.
 *
 *                 Note: The api is deprecated. Use au_cpuid_arch_is_zen3
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN3.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen3 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN4.
 *
 *                 Note: The api is deprecated. Use au_cpuid_arch_is_zen4
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN4.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen4 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN5.
 *
 *                 Note: The api is deprecated. Use au_cpuid_arch_is_zen5
 *                 instead.
 *
 * @param          cpu_num  CPU number.
 *
 * @return         1/true if processor architecture is AMD ZEN5.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen5 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the flag is available.
 *
 *                 Note: The api is deprecated. Use au_cpuid_has_flag
 *                 instead.
 *
 * @param          flag   CPU feature flag.
 *
 * @return         1/true if feature is available.
 */
AU_DEPRECATED_API_X("Use au_cpuid_has_flag instead.")
AUD_API_EXPORT bool
alci_cpu_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag);

/**
 * @brief          Get last level cache.
 *
 * @param[in]      core  CPU core number.
 *
 * @return         Returns last level cache.
 */
AUD_API_EXPORT uint32_t
alci_cpu_get_last_cache_level(au_cpu_num_t core);

#define alcpu_is_amd()       alci_is_amd(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen()  alci_arch_is_zen(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen2() alci_arch_is_zen2(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen3() alci_arch_is_zen3(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen4() alci_arch_is_zen4(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen5() alci_arch_is_zen5(ALCI_CURRENT_CPU_NUM)
#define alcpu_flag_is_available(flag)                                          \
    alci_cpu_has_flag(ALCI_CURRENT_CPU_NUM, flag)

AUD_EXTERN_C_END

#endif
