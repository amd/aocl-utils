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

#ifndef __AU_CPUID_LEGACY_H__
#define __AU_CPUID_LEGACY_H__

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

/******* OLD APIs ********/

/**
 * @brief          Allows caller to check if the processor vendor is AMD.
 *
 * @details        This function will work on all AMD processors.
 *                 |   AOCL 5.0.1    |  alci_is_amd()      |
 *                 |:---------------:|:-------------------:|
 *                 |Skylake          | False               |
 *                 |Bulldozer        | True                |
 *                 |Zen1             | True                |
 *                 |Zen2             | True                |
 *                 |Zen3             | True                |
 *                 |Zen4             | True                |
 *                 |Zen[X>4]         | True                |
 *
 * @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 * will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_is_amd instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor vendor is AMD.
 */
AU_DEPRECATED_API_X("Use au_cpuid_is_amd instead.")
AUD_API_EXPORT bool
alci_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief          Get Cpu vendor info.
 *
 * @details        arr [VendorID,FamilyID, ModelID, SteppingID,
 * UarchID], is a set of new line separated values.
 *
 * User must provide a buffer of size >= 16 bytes to store the vendor info
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_get_vendor
 *                 instead.**
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 * @param[out]     arr       Vendor info array
 *                           [VendorID\nFamilyID\nModelID\nSteppingID\nUarchID]
 * @param[in]      size      Size of Vendor info array in bytes.
 *
 * @return         Boolean, true if success.
 */
AU_DEPRECATED_API_X("Use au_cpu_get_vendor instead.")
AUD_API_EXPORT au_error_t
alci_cpu_get_vendor(au_cpu_num_t cpu_num, char* arr, size_t size);

/**
 * @brief          Allows caller to check if the processor arch is ZEN.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |  AOCL 5.0.1 |  alci_arch_is_zen()      |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1        | True                     |
 *                 | Zen1+       | True                     |
 *                 | Zen2        | True                     |
 *                 | Zen[3,4,5]  | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *                 |   AOCL 4.2  |  alci_arch_is_zen()      |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1        | True                     |
 *                 | Zen1+       | False                    |
 *                 | Zen2        | False                    |
 *                 | Zen3        | False                    |
 *                 | Zen[X>3]    | False                    |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_arch_is_zen instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor architecture is AMD ZEN.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen instead.")
AUD_API_EXPORT bool
alci_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN2.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |  AOCL 5.0.1 |  alci_arch_is_zen2()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1        | False                    |
 *                 | Zen1+       | False                    |
 *                 | Zen2        | True                     |
 *                 | Zen[3,4,5]  | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *                 |   AOCL 4.2  |  alci_arch_is_zen2()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1        | False                    |
 *                 | Zen1+       | False                    |
 *                 | Zen2        | True                     |
 *                 | Zen3        | False                    |
 *                 | Zen[X>3]    | False                    |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_arch_is_zen2
 *                  instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor architecture is AMD ZEN2.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen2 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN3.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |  AOCL 5.0.1 |  alci_arch_is_zen3()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1/1+     | False                    |
 *                 | Zen2        | False                    |
 *                 | Zen[3,4,5]  | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *                 |   AOCL 4.2  |  alci_arch_is_zen3()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen1/1+     | False                    |
 *                 | Zen2        | False                    |
 *                 | Zen3        | True                     |
 *                 | Zen4        | False                    |
 *                 | Zen[X>4]    | False                    |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_arch_is_zen3
 *                  instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor architecture is AMD ZEN3.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen3 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN4.
 *
 *
 * @details        This function is only meant for Zen based processors.
 *                 |  AOCL 5.0.1 |  alci_arch_is_zen4()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen[X<3]    | False                    |
 *                 | Zen3        | False                    |
 *                 | Zen[4,5]    | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *                 |   AOCL 4.2  |  alci_arch_is_zen4()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen[X<3]    | False                    |
 *                 | Zen3        | False                    |
 *                 | Zen4        | True                     |
 *                 | Zen5        | False                    |
 *                 | Zen[X>5]    | False                    |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_arch_is_zen4
 *                  instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor architecture is AMD ZEN4.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen4 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN5.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |  AOCL 5.0.1 |  alci_arch_is_zen5()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen[X<4]    | False                    |
 *                 | Zen4        | False                    |
 *                 | Zen5        | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *                 |  AOCL 4.2   |  alci_arch_is_zen5()     |
 *                 |:-----------:|:------------------------:|
 *                 | Skylake     | False                    |
 *                 | Bulldozer   | False                    |
 *                 | Zen[X<4]    | False                    |
 *                 | Zen4        | False                    |
 *                 | Zen5        | True                     |
 *                 | Zen[X>5]    | False                    |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_arch_is_zen5
 *                  instead.**
 *
 * @param          cpu_num  Any valid core number starting from 0.
 *
 * @return         1/true if processor architecture is AMD ZEN5.
 */
AU_DEPRECATED_API_X("Use au_cpuid_arch_is_zen5 instead.")
AUD_API_EXPORT bool
alci_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the flag is available.
 *
 * @details        List of supported flags: sse3, pclmulqdq, dtes64, monitor,
 dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm, pcid, dca,
 sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes, xsave, osxsave, avx,
 f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc, msr, pae, mce, cx8, apic,
 sep, mtrr, pge, mca, cmov, pat, pse36, pn, clflush, ds, acpi, mmx, fxsr, sse,
 sse2, ss, ht, tm, ia64, pbe, arat, fsgsbase, tsc_adjust, bmi1, hle, avx2, smep,
 bmi2, erms, invpcid, rtm, mpx, avx512f, avx512dq, rdseed, adx, smap,
 avx512ifma, pcommit, clflushopt, clwb, avx512pf, avx512er, avx512cd, sha_ni,
 avx512bw, avx512vl, avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57,
 rdpid, avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt, xsavec,
 xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic, cr8legacy, abm, sse4a,
 misalignsse, _3dnowprefetch, osvw, ibs, xop, skinit, wdt, lwp, fma4, tce,
 nodeid_msr, tbm, topoext, perfctr_core, perfctr_nb, syscall, nxxd, mmxext,
 fxsr_opt, pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow, invtsc, npt, lbrv,
 svm_lock, nrip_save, tsc_scale, vmcb_clean, flushbyasid, decodeassists,
 pause_filter, pfthreshold, xstore, xstore_en, xcrypt, xcrypt_en, ace2, ace2_en,
 phe, phe_en, pmm, pmm_en, vaes, vpclmulqdq, avx512_vnni, avx512_bitalg,
 avx512vbmi2, movdiri, movdir64b, avx512_vpintersect, x2avic
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @warning        **This api is deprecated. Use au_cpuid_has_flag instead.**
 *
 * @param          flag   CPU feature flag.
 *
 * @return         1/true if feature is available.
 */
AU_DEPRECATED_API_X("Use au_cpuid_has_flag instead.")
AUD_API_EXPORT bool
alci_cpu_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag);

#define alcpu_is_amd()       alci_is_amd(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen()  alci_arch_is_zen(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen2() alci_arch_is_zen2(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen3() alci_arch_is_zen3(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen4() alci_arch_is_zen4(ALCI_CURRENT_CPU_NUM)
#define alcpu_arch_is_zen5() alci_arch_is_zen5(ALCI_CURRENT_CPU_NUM)
#define alcpu_flag_is_available(flag)                                          \
    alci_cpu_has_flag(ALCI_CURRENT_CPU_NUM, flag)

AUD_EXTERN_C_END
#endif /* __AU_CPUID_LEGACY_H__ */
