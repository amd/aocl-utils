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
 * @details        This function will work on all AMD processors.
 *                 |    AOCL 5.2    |  au_cpuid_is_amd()  |
 *                 |:--------------:|:-------------------:|
 *                 |    Skylake     |        False        |
 *                 |   Bulldozer    |        True         |
 *                 |      Zen1      |        True         |
 *                 |      Zen2      |        True         |
 *                 |      Zen3      |        True         |
 *                 |      Zen4      |        True         |
 *                 |    Zen[X>4]    |        True         |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num  Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is designed by AMD.
 */
AUD_API_EXPORT bool
au_cpuid_is_amd(au_cpu_num_t cpu_num);

/**
 * @brief          Get Cpu vendor info.
 *
 * @details        vendor_info [VendorID,FamilyID, ModelID, SteppingID,
 * UarchID], is a set of new line separated values.
 *
 * User must provide a buffer of size >= 16 bytes to store the vendor info.
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 * @param[out]     vend_info Vendor info array
 * [VendorID\nFamilyID\nModelID\nSteppingID\nUarchID]
 * @param[in]      size      Size of Vendor info array in bytes.
 *
 * @return         None
 */
AUD_API_EXPORT void
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size);

/**
 * @brief          Allows caller to check if the processor arch is ZEN1.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |    AOCL 5.2    |  au_cpuid_arch_is_zen()  |
 *                 |:--------------:|:------------------------:|
 *                 |    Skylake     |          False           |
 *                 |   Bulldozer    |          False           |
 *                 |      Zen1      |          True            |
 *                 |      Zen2      |          True            |
 *                 |   Zen[3,4,5]   |          True            |
 *                 |   Zen[X>5]     |          True            |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen1 architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZENPLUS.
 *
 * @details        This function is only meant for Zen based processors.
 *                 NOTE: ZenPlus enum is disabled, this API now returns
 *                 same result as au_cpuid_arch_is_zen() for backward
 *                 compatibility.
 *                 |    AOCL 5.2    |  au_cpuid_arch_is_zenplus()  |
 *                 |:--------------:|:----------------------------:|
 *                 |    Skylake     |           False              |
 *                 |   Bulldozer    |           False              |
 *                 |      Zen1      |           True               |
 *                 |      Zen2      |           True               |
 *                 |   Zen[3,4,5]   |           True               |
 *                 |   Zen[X>5]     |           True               |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen1 or higher architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zenplus(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN2.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |    AOCL 5.2    |  au_cpuid_arch_is_zen2()  |
 *                 |:--------------:|:-------------------------:|
 *                 |    Skylake     |          False            |
 *                 |   Bulldozer    |          False            |
 *                 |      Zen1      |          False            |
 *                 |      Zen2      |          True             |
 *                 |   Zen[3,4,5]   |          True             |
 *                 |   Zen[X>5]     |          True             |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen2 architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN3.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |    AOCL 5.2    |  au_cpuid_arch_is_zen3()  |
 *                 |:--------------:|:-------------------------:|
 *                 |    Skylake     |          False            |
 *                 |   Bulldozer    |          False            |
 *                 |      Zen1      |          False            |
 *                 |      Zen2      |          False            |
 *                 |   Zen[3,4,5]   |          True             |
 *                 |   Zen[X>5]     |          True             |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen3 architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN4.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |   AOCL 5.2   |  au_cpuid_arch_is_zen4()  |
 *                 |:-----------:|:-------------------------:|
 *                 |   Skylake   |          False            |
 *                 |  Bulldozer  |          False            |
 *                 |   Zen[X<3]  |          False            |
 *                 |    Zen3     |          False            |
 *                 |   Zen[4,5]  |          True             |
 *                 |   Zen[X>5]  |          True             |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen4 architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is ZEN5.
 *
 * @details        This function is only meant for Zen based processors.
 *                 |   AOCL 5.2   |  au_cpuid_arch_is_zen5()  |
 *                 |:------------:|:------------------------:|
 *                 |   Skylake    |          False           |
 *                 |  Bulldozer   |          False           |
 *                 |   Zen[X<4]   |          False           |
 *                 |    Zen4      |          False           |
 *                 |    Zen5      |          True            |
 *                 |   Zen[X>5]   |          True            |
 *
 *  <a href="#c-api-behaviour-summary"> C-API Behaviour Summary </a>
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 * will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is Zen5 architecture.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num);

/**
 * @brief     Checks if processor is x86_64-v2 compliant
 *
 * @details   Based on GCC following flags account for x86_64-v2
 *            (in addition to x86_64 [sse, sse2])
 *
 *            cx16       lahf_lm
 *            popcnt     sse4_1
 *            sse4_2     ssse3
 *
 *            Output of this API will be same as
 *            hasFlag(ECpuidFlag::sse)   && hasFlag(ECpuidFlag::sse2)   &&
 *            hasFlag(ECpuidFlag::cx16)  && hasFlag(ECpuidFlag::lahf_lm)&&
 *            hasFlag(ECpuidFlag::popcnt)&& hasFlag(ECpuidFlag::sse4_1) &&
 *            hasFlag(ECpuidFlag::sse4_2)&& hasFlag(ECpuidFlag::ssse3)
 *
 *            |    AOCL 5.2    |  au_cpuid_arch_is_x86_64v2()  |
 *            |:--------------:|:-----------------------------:|
 *            |  Sandybridge   |            True               |
 *            |   Broadwell    |            True               |
 *            |    Skylake     |            True               |
 *            |   Bulldozer    |            True               |
 *            |     Zen1       |            True               |
 *            |     Zen2       |            True               |
 *            |     Zen3       |            True               |
 *            |     Zen4       |            True               |
 *            |     Zen5       |            True               |
 *            |   Zen[X>5]     |            True               |
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is x86_64v2.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_x86_64v2(au_cpu_num_t cpu_num);

/**
 * @brief     Checks if processor is x86_64-v3 compliant
 *
 * @details   Based on GCC following flags account for x86_64-v3
 *            (in addition to x86_64-v2)
 *
 *            avx    avx2    bmi1
 *            bmi2   f16c    fma
 *            abm    movbe   xsave
 *
 *            Output of this API will be same as        isX86_64v2() &&
 *            hasFlag(ECpuidFlag::avx)  && hasFlag(ECpuidFlag::avx2) &&
 *            hasFlag(ECpuidFlag::bmi1) && hasFlag(ECpuidFlag::bmi2) &&
 *            hasFlag(ECpuidFlag::f16c) && hasFlag(ECpuidFlag::fma)  &&
 *            hasFlag(ECpuidFlag::abm)  && hasFlag(ECpuidFlag::movbe)&&
 *            hasFlag(ECpuidFlag::xsave)
 *
 *            |    AOCL 5.2    |  au_cpuid_arch_is_x86_64v3()  |
 *            |:--------------:|:-----------------------------:|
 *            |  Sandybridge   |            False              |
 *            |   Broadwell    |            True               |
 *            |    Skylake     |            True               |
 *            |   Bulldozer    |            True               |
 *            |     Zen1       |            True               |
 *            |     Zen2       |            True               |
 *            |     Zen3       |            True               |
 *            |     Zen4       |            True               |
 *            |     Zen5       |            True               |
 *            |   Zen[X>5]     |            True               |
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is x86_64v3.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_x86_64v3(au_cpu_num_t cpu_num);

/**
 * @brief     Checks if processor is x86_64-v4 compliant
 *
 * @details   Based on GCC following flags account for x86_64-v4
 *            (in addition to x86_64-v2 + x86_64-v3)
 *
 *            avx512f   avx512bw  avx512cd
 *            avx512dq  avx512vl
 *
 *            Output of this API will be same as  isX86_64v3() &&
 *            hasFlag(ECpuidFlag::avx512f)  &&
 *            hasFlag(ECpuidFlag::avx512bw) &&
 *            hasFlag(ECpuidFlag::avx512cd) &&
 *            hasFlag(ECpuidFlag::avx512dq) &&
 *            hasFlag(ECpuidFlag::avx512vl)
 *
 *            |    AOCL 5.2    |  au_cpuid_arch_is_x86_64v4()  |
 *            |:--------------:|:-----------------------------:|
 *            |   Sandybridge  |            False              |
 *            |    Broadwell   |            False              |
 *            |     Skylake    |            True               |
 *            |    Bulldozer   |            True               |
 *            |      Zen1      |            True               |
 *            |      Zen2      |            True               |
 *            |      Zen3      |            True               |
 *            |      Zen4      |            True               |
 *            |      Zen5      |            True               |
 *            |   Zen[X>5]     |            True               |
 *
 * @param[in]      cpu_num   CPU number.
 *
 * @return         1/true if processor architecture is x86_64v4.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_x86_64v4(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the processor arch is part of the
 * Zen family.
 *
 * @details        This function is meant for Zen based processors.
 *                 |    AOCL 5.2    |  au_cpuid_arch_is_zen_family()  |
 *                 |:--------------:|:-------------------------------:|
 *                 |    Skylake     |              False              |
 *                 |   Bulldozer    |              False              |
 *                 |      Zen1      |              True               |
 *                 |      Zen2      |              True               |
 *                 |      Zen3      |              True               |
 *                 |      Zen4      |              True               |
 *                 |      Zen5      |              True               |
 *                 |   Zen[X>5]     |              True               |
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 *
 * @return         Boolean, true if processor is part of the Zen family.
 */
AUD_API_EXPORT bool
au_cpuid_arch_is_zen_family(au_cpu_num_t cpu_num);

/**
 * @brief          Allows caller to check if the flags are available.
 *
 * @details        List of supported flags: sse3, pclmulqdq, dtes64, monitor,
 *       dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
 *       pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline,
 *       aes, xsave, osxsave, avx, f16c, rdrand, hypervisor, fpu, vme,
 *       de, pse, tsc, msr, pae, mce, cx8, apic, sep, mtrr, pge, mca,
 *       cmov, pat, pse36, pn, clflush, ds, acpi, mmx, fxsr, sse, sse2,
 *       ss, ht, tm, ia64, pbe, arat, fsgsbase, tsc_adjust, bmi1, hle,
 *       avx2, smep, bmi2, erms, invpcid, rtm, mpx, avx512f, avx512dq,
 *       rdseed, adx, smap, avx512ifma, pcommit, clflushopt, clwb,
 *       avx512pf, avx512er, avx512cd, sha_ni, avx512bw, avx512vl,
 *       avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57, rdpid,
 *       avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt,
 *       xsavec, xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic,
 *       cr8legacy, abm, sse4a, misalignsse, _3dnowprefetch, osvw, ibs,
 *       xop, skinit, wdt, lwp, fma4, tce, nodeid_msr, tbm, topoext,
 *       perfctr_core, perfctr_nb, syscall, nxxd, mmxext, fxsr_opt,
 *       pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow, invtsc, npt, lbrv,
 *       svm_lock, nrip_save, tsc_scale, vmcb_clean, flushbyasid,
 *       decodeassists, pause_filter, pfthreshold, xstore, xstore_en,
 *       xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en,
 *       vaes, vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2,
 *       movdiri, movdir64b, avx512_vpintersect, x2avic
 *
 * @warning        If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this
 * function will result in thread migration to the selected core.
 * @warning        The API is deprecated. Use au_cpuid_has_flags instead.
 *
 * @param[in]      cpu_num     Any valid core number starting from 0.
 * @param[in]      flag_array  CPU feature flag names.
 * @param[in]      count       Number of flags in the list.
 *
 * @return         An array with Boolean value corresponding to each flag given
 *                 in flag_array. If the value is true, the flag is available.
 */
AU_DEPRECATED_API_X("Use au_cpuid_has_flags instead.")
AUD_API_EXPORT bool*
au_cpuid_has_flag(au_cpu_num_t      cpu_num,
                  const char* const flag_array[],
                  int               count);

/**
 * @brief          Allows caller to check if all the flags are available.
 *
 * @details        List of supported flags: sse3, pclmulqdq, dtes64, monitor,
 *                 dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
 * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes, xsave,
 * osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc, msr, pae,
 * mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn, clflush, ds, acpi,
 * mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat, fsgsbase, tsc_adjust,
 * bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm, mpx, avx512f, avx512dq,
 * rdseed, adx, smap, avx512ifma, pcommit, clflushopt, clwb, avx512pf, avx512er,
 * avx512cd, sha_ni, avx512bw, avx512vl, avx512vbmi, umip, pku, ospke,
 * avx512_vpopcntdq, la57, rdpid, avx512_4vnniw, avx512_4fmaps, avx512_bf16,
 * avxvnni, xsaveopt, xsavec, xgetbv1, xsaves, lahf_lm, cmp_legacy, svm,
 * extapic, cr8legacy, abm, sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop,
 * skinit, wdt, lwp, fma4, tce, nodeid_msr, tbm, topoext, perfctr_core,
 * perfctr_nb, syscall, nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64,
 * _3dnowext, _3dnow, invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale,
 * vmcb_clean, flushbyasid, decodeassists, pause_filter, pfthreshold, xstore,
 * xstore_en, xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
 * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
 * avx512_vpintersect, x2avic
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 * @param[in]      flag_array  CPU feature flag names.
 * @param[in]      count   Number of flags in the list.
 *
 * @return         Boolean, true if all flags are available.
 */
AUD_API_EXPORT bool
au_cpuid_has_flags_all(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count);

/**
 * @brief          Allows caller to check if any of the flags are available.
 *
 * @details        List of supported flags: sse3, pclmulqdq, dtes64, monitor,
 *                 dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
 * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes, xsave,
 * osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc, msr, pae,
 * mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn, clflush, ds, acpi,
 * mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat, fsgsbase, tsc_adjust,
 * bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm, mpx, avx512f, avx512dq,
 * rdseed, adx, smap, avx512ifma, pcommit, clflushopt, clwb, avx512pf, avx512er,
 * avx512cd, sha_ni, avx512bw, avx512vl, avx512vbmi, umip, pku, ospke,
 * avx512_vpopcntdq, la57, rdpid, avx512_4vnniw, avx512_4fmaps, avx512_bf16,
 * avxvnni, xsaveopt, xsavec, xgetbv1, xsaves, lahf_lm, cmp_legacy, svm,
 * extapic, cr8legacy, abm, sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop,
 * skinit, wdt, lwp, fma4, tce, nodeid_msr, tbm, topoext, perfctr_core,
 * perfctr_nb, syscall, nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64,
 * _3dnowext, _3dnow, invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale,
 * vmcb_clean, flushbyasid, decodeassists, pause_filter, pfthreshold, xstore,
 * xstore_en, xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
 * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
 * avx512_vpintersect, x2avic
 *
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 * @param[in]      flag_array  CPU feature flag names.
 * @param[in]      count   Number of flags in the list.
 *
 * @return         Boolean, true if any of the flags are available.
 */
AUD_API_EXPORT bool
au_cpuid_has_flags_any(au_cpu_num_t      cpu_num,
                       const char* const flag_array[],
                       int               count);

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
 avx512vbmi2, movdiri, movdir64b, avx512_vpintersect, x2avic"
 *  @warning If cpu_num is not "AU_CURRENT_CPU_NUM", then calling this function
 *  will result in thread migration to the selected core.
 *
 * @param[in]      cpu_num   Any valid core number starting from 0.
 * @param[in]      flag_array  CPU feature flag names.
 * @param[in]      count   Number of flags in the list.
 *
 * @return         A boolean indicating presence or absence of the features
 combined. (It’s a logical AND of result of each flag).

 */
AUD_API_EXPORT bool
au_cpuid_has_flags(au_cpu_num_t      cpu_num,
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

AUD_EXTERN_C_END

#endif /* __AU_CPUID_CPUID_H__ */
