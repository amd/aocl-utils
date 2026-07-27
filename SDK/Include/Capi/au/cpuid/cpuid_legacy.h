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

#ifndef __AU_CPUID_LEGACY_H__
#define __AU_CPUID_LEGACY_H__

#include "Au/Config.h"
#include "Au/Defs.hh"
#include "Capi/au/au.h"

/* Header-only: alci_* legacy API forwards to the header-only au_cpuid_* API. */
#include "Capi/au/cpuid/cpuid.h"

#include <stdbool.h>
#include <stdint.h>

AUD_EXTERN_C_BEGIN

/** @cond INTERNAL */
#if !defined(au_cpu_num_t)
typedef Uint32 au_cpu_num_t;
typedef Uint32 au_cpu_flag_t;
#endif
/** @endcond */

/**
 * @brief Check if processor vendor is AMD (deprecated, use au_cpuid_is_amd instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_is_amd instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor vendor is AMD.
 */
AU_DEPRECATED_API_X("Use au_cpuid_is_amd instead.")
AU_CPUID_API bool
alci_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief Get CPU vendor info (newline-separated: VendorID, FamilyID, ModelID, SteppingID, UarchID; deprecated, use au_cpuid_get_vendor).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs. Buffer size >= 16 bytes required.
 * @warning Deprecated. Use au_cpuid_get_vendor instead.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[out] arr Vendor info array.
 * @param[in] size Size of vendor info array in bytes.
 * @return Boolean, true if success.
 */
AU_DEPRECATED_API_X("Use au_cpuid_get_vendor instead.")
AU_CPUID_API au_error_t
alci_cpu_get_vendor(au_cpu_num_t cpu_num, char* arr, size_t size);

/**
 * @brief Check if processor arch is Zen or newer (deprecated, use au_cpuid_arch_is_zen instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_arch_is_zen instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen or newer.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen instead.")
AU_CPUID_API bool
alci_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen2 or newer (deprecated, use au_cpuid_arch_is_zen2 instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_arch_is_zen2 instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen2 or newer.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen2 instead.")
AU_CPUID_API bool
alci_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen3 or newer (deprecated, use au_cpuid_arch_is_zen3 instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_arch_is_zen3 instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen3 or newer.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen3 instead.")
AU_CPUID_API bool
alci_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen4 or newer (deprecated, use au_cpuid_arch_is_zen4 instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_arch_is_zen4 instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen4 or newer.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen4 instead.")
AU_CPUID_API bool
alci_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen5 or newer (deprecated, use au_cpuid_arch_is_zen5 instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_arch_is_zen5 instead.
 * @param cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen5 or newer.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen5 instead.")
AU_CPUID_API bool
alci_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief Check if CPU feature flag is available (deprecated, use au_cpuid_has_flag instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_has_flag instead.
 * @param cpu_num Any valid core number starting from 0.
 * @param flag CPU feature flag.
 * @return true if feature is available.
 */
AU_DEPRECATED_API_X("Use au_cpuid_has_flag instead.")
AU_CPUID_API bool
alci_cpu_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag);

#define alcpu_is_amd()       alci_is_amd(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen()  alci_arch_is_zen(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen2() alci_arch_is_zen2(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen3() alci_arch_is_zen3(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen4() alci_arch_is_zen4(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen5() alci_arch_is_zen5(ALCI_CURRENT_CPU_NUM)
#define alcpu_flag_is_available(flag)                                          \
    alci_cpu_has_flag(ALCI_CURRENT_CPU_NUM, flag)

/* Header-only implementation: forward to the au_cpuid_* API. Emitted only when AU_CPUID_IMPLEMENTATION is set. */
#if defined(AU_CPUID_IMPLEMENTATION)

AU_CPUID_API bool
alci_is_amd(au_cpu_num_t cpu_num)
{
    return au_cpuid_is_amd(cpu_num);
}

AU_CPUID_API au_error_t
alci_cpu_get_vendor(au_cpu_num_t cpu_num, char* arr, size_t size)
{
    au_cpuid_get_vendor(cpu_num, arr, size);
    return 1;
}

AU_CPUID_API bool
alci_arch_is_zen(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen(cpu_num);
}

AU_CPUID_API bool
alci_arch_is_zen2(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen2(cpu_num);
}

AU_CPUID_API bool
alci_arch_is_zen3(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen3(cpu_num);
}

AU_CPUID_API bool
alci_arch_is_zen4(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen4(cpu_num);
}

AU_CPUID_API bool
alci_arch_is_zen5(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen5(cpu_num);
}

AU_CPUID_API bool
alci_cpu_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_has_flag(&c, (uint16_t)flag);
}

#endif /* AU_CPUID_IMPLEMENTATION */

AUD_EXTERN_C_END
#endif /* __AU_CPUID_LEGACY_H__ */
