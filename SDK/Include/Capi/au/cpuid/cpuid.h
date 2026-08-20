/*
 * Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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

/* Library-backed CPUID C API entry point: declarations only; link libaoclutils (C++ + C) or libaoclutils_c (C-only, no libstdc++). Include cpuid_inline.h to inline the API instead, or au_cpuid_header_only.h for the standalone single-file drop-in. */

#include "Au/Config.h"
#include "Au/Defs.hh"
#include "Capi/au/au.h"

#include "Capi/au/cpuid/cpuid_core.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Linkage-mode mechanism (INTERNAL). AU_CPUID_API sets storage class (default: plain, i.e. declarations resolved by the linked library). AU_CPUID_IMPLEMENTATION emits bodies when defined, else only decls. cpuid_inline.h sets AU_CPUID_API=static inline and opts into bodies; the library TUs set it to the export attribute. A direct include defaults to link-a-library, so misuse fails loudly at link time rather than silently inlining. */
#ifndef AU_CPUID_API
#define AU_CPUID_API
#endif

AUD_EXTERN_C_BEGIN

#if !defined(au_cpu_num_t)
/**
 * @brief CPU number type
 */
typedef Uint32 au_cpu_num_t;
/**
 * @brief CPU flag type
 */
typedef Uint32 au_cpu_flag_t;
#endif

/**
 * @brief Check if the processor vendor is AMD.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is designed by AMD.
 */
AU_CPUID_API bool
au_cpuid_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief Get CPU vendor info (newline-separated: VendorID, FamilyID, ModelID, SteppingID, UarchID).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs. Buffer size >= 16 bytes required.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[out] vend_info Vendor info array.
 * @param[in] size Size of vendor info array in bytes.
 */
AU_CPUID_API void
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size);

/**
 * @brief Check if processor arch is Zen1/1+ or newer (Zen1+ through Zen6+).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen1/1+ or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is ZENPLUS or newer (alias of au_cpuid_arch_is_zen for backward compat).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen1/1+ or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zenplus(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen2 or newer.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen2 or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen3 or newer.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen3 or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen4 or newer.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen4 or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen5 or newer.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen5 or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor arch is Zen6 or newer.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is Zen6 or newer.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen6(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor is x86_64-v2 compliant (baseline + cx16, lahf_lm, popcnt, sse4_1, sse4_2, ssse3).
 * @param[in] cpu_num CPU number.
 * @return true if processor is x86_64-v2 compliant.
 */
AU_CPUID_API bool
au_cpuid_arch_is_x86_64v2(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor is x86_64-v3 compliant (v2 + avx, avx2, bmi1, bmi2, f16c, fma, abm, movbe, xsave).
 * @param[in] cpu_num CPU number.
 * @return true if processor is x86_64-v3 compliant.
 */
AU_CPUID_API bool
au_cpuid_arch_is_x86_64v3(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor is x86_64-v4 compliant (v3 + avx512f, avx512bw, avx512cd, avx512dq, avx512vl).
 * @param[in] cpu_num CPU number.
 * @return true if processor is x86_64-v4 compliant.
 */
AU_CPUID_API bool
au_cpuid_arch_is_x86_64v4(au_cpu_num_t cpu_num);

/**
 * @brief Check if processor is part of the Zen family (Zen1 through Zen6+).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @return true if processor is part of the Zen family.
 */
AU_CPUID_API bool
au_cpuid_arch_is_zen_family(au_cpu_num_t cpu_num);

/**
 * @brief Check if the package is a hybrid (heterogeneous) part (CPUID 0x7.0 EDX bit 15).
 * @return true if the package advertises the Hybrid bit (zero on all AMD, pre-Alder-Lake Intel).
 */
AU_CPUID_API bool
au_cpuid_is_hybrid(void);

/**
 * @brief Get the core type of the currently executing core (CPUID 0x1A.0 EAX[31:24]).
 * @return Raw core-type byte (Intel: 0x40=P-core, 0x20=E-core), or 0 if unsupported.
 */
AU_CPUID_API uint32_t
au_cpuid_get_core_type(void);

/**
 * @brief Check if CPU feature flags are available (deprecated, use au_cpuid_has_flags instead).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @warning Deprecated. Use au_cpuid_has_flags instead.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[in] flag_array CPU feature flag names.
 * @param[in] count Number of flags in the list.
 * @return Boolean array corresponding to each flag in flag_array.
 */
AU_DEPRECATED_API_X("Use au_cpuid_has_flags instead.")
AU_CPUID_API bool*
au_cpuid_has_flag(au_cpu_num_t      cpu_num,
                  const char* const flag_array[],
                  int               count);

/**
 * @brief Check if all CPU feature flags in the list are available.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[in] flag_array CPU feature flag names.
 * @param[in] count Number of flags in the list.
 * @return true if all flags are available.
 */
AU_CPUID_API bool
au_cpuid_has_flags_all(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count);

/**
 * @brief Check if any of the CPU feature flags in the list are available.
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[in] flag_array CPU feature flag names.
 * @param[in] count Number of flags in the list.
 * @return true if any of the flags are available.
 */
AU_CPUID_API bool
au_cpuid_has_flags_any(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count);

/**
 * @brief Check if all CPU feature flags in the list are available (logical AND).
 * @warning If cpu_num is not AU_CURRENT_CPU_NUM, thread migration occurs.
 * @param[in] cpu_num Any valid core number starting from 0.
 * @param[in] flag_array CPU feature flag names.
 * @param[in] count Number of flags in the list.
 * @return true if all flags are available (logical AND).
 */
AU_CPUID_API bool
au_cpuid_has_flags(au_cpu_num_t      cpu_num,
                   const char* const flag_array[],
                   int               count);
/**
 * @brief          Portable API to check if an error has occurred
 *
 * @param          err  Actual error number
 *
 * @return         1/true if error occurred, false otherwise.
 */
AU_CPUID_API bool
au_cpuid_is_error(au_error_t err);

/* Header-only implementation: each entry point resolves a handle (one cpuid sweep) and decodes it. Emitted only when AU_CPUID_IMPLEMENTATION is set. */
#if defined(AU_CPUID_IMPLEMENTATION)

/* Resolve a fully-populated handle for cpu_num using the real cpuid. Non-strict: out-of-mask cpu_num degrades to first-allowed core. */
static inline au_cpu_info_t
au_capi_resolve(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c;
    c.raw_fn  = NULL;
    c.raw_ctx = NULL;
    au_cpuid_init(&c, (int)cpu_num, false);
    return c;
}

AU_CPUID_API bool
au_cpuid_is_amd(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_amd(&c);
}

AU_CPUID_API void
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    snprintf(vend_info,
             size,
             "%u\n%u\n%u\n%u\n%u\n",
             (unsigned)c.vendor,
             (unsigned)c.family,
             (unsigned)c.model,
             (unsigned)c.stepping,
             (unsigned)c.uarch);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zenplus(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZENPLUS, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN2, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN3, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN4, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN5, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen6(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_uarch(&c, AU_UARCH_ZEN6, false);
}

AU_CPUID_API bool
au_cpuid_arch_is_x86_64v2(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_x86_64v2(&c);
}

AU_CPUID_API bool
au_cpuid_arch_is_x86_64v3(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_x86_64v3(&c);
}

AU_CPUID_API bool
au_cpuid_arch_is_x86_64v4(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_x86_64v4(&c);
}

AU_CPUID_API bool
au_cpuid_arch_is_zen_family(au_cpu_num_t cpu_num)
{
    au_cpu_info_t c = au_capi_resolve(cpu_num);
    return au_cpuid_info_is_zen_family(&c);
}

AU_CPUID_API bool
au_cpuid_is_hybrid(void)
{
    return au_cpuid_read_is_hybrid(NULL, NULL);
}

AU_CPUID_API uint32_t
au_cpuid_get_core_type(void)
{
    return au_cpuid_read_core_type(NULL, NULL);
}

AU_CPUID_API bool*
au_cpuid_has_flag(au_cpu_num_t      cpu_num,
                  const char* const flag_array[],
                  int               count)
{
    if (count <= 1)
        return NULL;

    au_cpu_info_t c      = au_capi_resolve(cpu_num);
    bool*         result = (bool*)malloc((size_t)count * sizeof(bool));
    if (!result)
        return NULL;

    for (int i = 0; i < count; i++) {
        result[i] = au_cpuid_info_has_flag(
            &c, au_cpuid_flag_from_string(flag_array[i]));
    }
    return result;
}

AU_CPUID_API bool
au_cpuid_has_flags_all(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count)
{
    if (count <= 0)
        return false;

    au_cpu_info_t c = au_capi_resolve(cpu_num);
    for (int i = 0; i < count; i++) {
        if (!au_cpuid_info_has_flag(&c,
                                    au_cpuid_flag_from_string(flag_array[i])))
            return false;
    }
    return true;
}

AU_CPUID_API bool
au_cpuid_has_flags_any(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count)
{
    if (count <= 0)
        return false;

    au_cpu_info_t c = au_capi_resolve(cpu_num);
    for (int i = 0; i < count; i++) {
        if (au_cpuid_info_has_flag(&c,
                                   au_cpuid_flag_from_string(flag_array[i])))
            return true;
    }
    return false;
}

AU_CPUID_API bool
au_cpuid_has_flags(au_cpu_num_t      cpu_num,
                   const char* const flag_array[],
                   int               count)
{
    if (count <= 0)
        return false;

    au_cpu_info_t c      = au_capi_resolve(cpu_num);
    bool          result = true;
    for (int i = 0; i < count; i++) {
        result = result
                 && au_cpuid_info_has_flag(
                     &c, au_cpuid_flag_from_string(flag_array[i]));
    }
    return result;
}

AU_CPUID_API bool
au_cpuid_is_error(au_error_t err)
{
    if ((int32_t)err)
        return true;
    return false;
}

#endif /* AU_CPUID_IMPLEMENTATION */

AUD_EXTERN_C_END

#endif /* __AU_CPUID_CPUID_H__ */
