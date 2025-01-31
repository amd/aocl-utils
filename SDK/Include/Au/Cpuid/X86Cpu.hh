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

#ifdef WIN32
#include <Windows.h>
#endif

#include "Au/Cpuid/Cpuid.hh"
#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Interface/Cpuid/ICpu.hh"
#include "Au/Memory/BufferView.hh"

#include <map>
#include <memory>
#include <sstream>

#define AUD_DEFINE_ENUM(name, type, ...)                                       \
    enum class name : type                                                     \
    {                                                                          \
        Min,                                                                   \
        __VA_ARGS__,                                                           \
        Max,                                                                   \
    };                                                                         \
    inline std::stringstream& operator<<(std::stringstream&  os,               \
                                         std::vector<String> values)           \
    {                                                                          \
        String                   str = #__VA_ARGS__;                           \
        std::map<String, Uint64> flags;                                        \
        Uint64                   flagsCounter = 1;                             \
        std::stringstream        ss(str);                                      \
        String                   token;                                        \
        std::getline(ss, token, ',');                                          \
        flags[token] = flagsCounter++;                                         \
        while (std::getline(ss, token, ',')) {                                 \
            token        = token.substr(1, token.length() - 1);                \
            flags[token] = flagsCounter++;                                     \
        }                                                                      \
        for (auto value : values) {                                            \
            os << flags[value] << ":";                                         \
        }                                                                      \
        return os;                                                             \
    }                                                                          \
    inline uint64_t name##fromString(const std::string& str)                   \
    {                                                                          \
        std::string       s = #__VA_ARGS__;                                    \
        std::stringstream ss(s);                                               \
        std::string       token;                                               \
        uint64_t          index = 1; /** Start after Min */                    \
        while (std::getline(ss, token, ',')) {                                 \
            /**  Remove any leading whitespace */                              \
            size_t start = token.find_first_not_of(" ");                       \
            if (start != std::string::npos) {                                  \
                token = token.substr(start);                                   \
            }                                                                  \
            if (token == str) {                                                \
                return index;                                                  \
            }                                                                  \
            index++;                                                           \
        }                                                                      \
        return -1; /**  Return Max if not found */                             \
    }                                                                          \
    inline std::string name##toString(Uint64 value)                            \
    {                                                                          \
        String            str = #__VA_ARGS__;                                  \
        std::stringstream ss(str);                                             \
        Uint64            current = 1;                                         \
        String            token;                                               \
                                                                               \
        if (value == 0) {                                                      \
            /* 0th value does not exist */                                     \
            return "UNDEF";                                                    \
        }                                                                      \
        while (std::getline(ss, token, ',')) {                                 \
            if (current == value) {                                            \
                break;                                                         \
            }                                                                  \
            current++;                                                         \
        }                                                                      \
        token = token.substr(1, token.length() - 1);                           \
        return token;                                                          \
    }

namespace Au {

/* Processor Micro architecure info */
enum class EUarch : Uint16
{
    Unknown = 0,
    Zen,
    ZenPlus,
    Zen2,
    Zen3,
    Zen4,
    Zen5,
    Max = Zen5,
};

enum class HasFlagsMode
{
    Classic,
    All,
    Any,
};

/**
 * @struct  VendorInfo
 *
 * @brief   CPU core info.
 */
class VendorInfo
{
    /* TODO: Make this private and provide accessors */
  public:
    EVendor m_mfg;      /**< CPU manufacturing vendor. */
    EFamily m_family;   /**< CPU family ID. */
    Uint16  m_model;    /**< CPU model number. */
    Uint16  m_stepping; /**< CPU stepping. */
    EUarch  m_uarch;    /**< CPU microarchitecture. */
};

/**
 * @enum ECpuidFlag
 *
 * @brief Flags supported by CPU as features.
 */
AUD_DEFINE_ENUM(ECpuidFlag,
                Uint64,
                sse3,
                pclmulqdq,
                dtes64,
                monitor,
                dscpl,
                vmx,
                smx,
                est,
                tm2,
                ssse3,
                cid,
                fma,
                cx16,
                xtpr,
                pdcm,
                pcid,
                dca,
                sse4_1,
                sse4_2,
                x2apic,
                movbe,
                popcnt,
                tsc_deadline,
                aes,
                xsave,
                osxsave,
                avx,
                f16c,
                rdrand,
                hypervisor,
                fpu,
                vme,
                de,
                pse,
                tsc,
                msr,
                pae,
                mce,
                cx8,
                apic,
                sep,
                mtrr,
                pge,
                mca,
                cmov,
                pat,
                pse36,
                pn,
                clflush,
                ds,
                acpi,
                mmx,
                fxsr,
                sse,
                sse2,
                ss,
                ht,
                tm,
                ia64,
                pbe,
                arat,
                fsgsbase,
                tsc_adjust,
                bmi1,
                hle,
                avx2,
                smep,
                bmi2,
                erms,
                invpcid,
                rtm,
                mpx,
                avx512f,
                avx512dq,
                rdseed,
                adx,
                smap,
                avx512ifma,
                pcommit,
                clflushopt,
                clwb,
                avx512pf,
                avx512er,
                avx512cd,
                sha_ni,
                avx512bw,
                avx512vl,
                avx512vbmi,
                umip,
                pku,
                ospke,
                avx512_vpopcntdq,
                la57,
                rdpid,
                avx512_4vnniw,
                avx512_4fmaps,
                avx512_bf16,
                avxvnni,
                xsaveopt,
                xsavec,
                xgetbv1,
                xsaves,
                lahf_lm,
                cmp_legacy,
                svm,
                extapic,
                cr8legacy,
                abm,
                sse4a,
                misalignsse,
                _3dnowprefetch,
                osvw,
                ibs,
                xop,
                skinit,
                wdt,
                lwp,
                fma4,
                tce,
                nodeid_msr,
                tbm,
                topoext,
                perfctr_core,
                perfctr_nb,
                syscall,
                nxxd,
                mmxext,
                fxsr_opt,
                pdpe1gb,
                rdtscp,
                lmi64,
                _3dnowext,
                _3dnow,
                invtsc,
                npt,
                lbrv,
                svm_lock,
                nrip_save,
                tsc_scale,
                vmcb_clean,
                flushbyasid,
                decodeassists,
                pause_filter,
                pfthreshold,
                xstore,
                xstore_en,
                xcrypt,
                xcrypt_en,
                ace2,
                ace2_en,
                phe,
                phe_en,
                pmm,
                pmm_en,
                vaes,
                vpclmulqdq,
                avx512_vnni,
                avx512_bitalg,
                avx512vbmi2,
                movdiri,
                movdir64b,
                avx512_vpintersect,
                x2avic)

class AUD_API_EXPORT X86Cpu final : public CpuInfo
{
  public:
    /**
     * @brief  Constructor from CpuidUtils object.
     *
     * @warning This API is used only for testing, does not serve any other
     * purpose.
     *
     * @param[in] cUtils  CpuidUtils object to use for fetching CPUID info.
     * @param[in] num     CPU number to fetch info from.
     */
    X86Cpu(CpuidUtils* cUtils, CpuNumT num);

    /**
     * @brief   Default constructor.
     *
     * @details This constructor sets the CPU number to info from.
     *
     * @warning If num is not "AU_CURRENT_CPU_NUM", then calling this
     * constructor will result in thread migration to the selected core.
     *
     * @param[in] num  Any valid core number starting from 0.
     *
     * @note    Default behaviour is invoked by passing AU_CURRENT_CPU_NUM,
     *          In default behaviour the thread is not pinned to any core,
     *          cpuid fetches the current CPU thread on which the code is
     *          running.
     */
    X86Cpu(CpuNumT num = AU_CURRENT_CPU_NUM);

    /**
     * @brief   Destructor.
     */
    ~X86Cpu();

    /**
     * @brief     Check if vendor is AMD
     *
     * @details   This function will work on all AMD processors.
     *            |   AOCL 5.0.1  |  isAMD()  |
     *            |:-------------:|:---------:|
     *            |   Skylake     |   False   |
     *            |  Bulldozer    |   True    |
     *            |     Zen1      |   True    |
     *            |     Zen2      |   True    |
     *            |     Zen3      |   True    |
     *            |     Zen4      |   True    |
     *            |    Zen[X>4]   |   True    |
     *
     * @return    true if 'num' was an AMD x86-64, false otherwise
     */
    bool isAMD() const;

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
     *            For more info refer to
     *            https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
     *
     *            |   AOCL 5.0.1   |      isX86_64v2()      |
     *            |:-------------:|:----------------------:|
     *            |  Sandybridge  |         True           |
     *            |   Broadwell   |         True           |
     *            |    Skylake    |         True           |
     *            |   Bulldozer   |         True           |
     *            |     Zen1      |         True           |
     *            |     Zen2      |         True           |
     *            |     Zen3      |         True           |
     *            |     Zen4      |         True           |
     *            |     Zen5      |         True           |
     *
     * @param     none
     *
     * @return    true if cpu supports all features above,
     *            false otherwise
     */
    bool isX86_64v2() const;

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
     *            For more info refer to
     *            https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
     *
     *            |   AOCL 5.0.1   |      isX86_64v3()      |
     *            |:--------------:|:----------------------:|
     *            |  Sandybridge   |         False          |
     *            |   Broadwell    |         True           |
     *            |    Skylake     |         True           |
     *            |   Bulldozer    |         True           |
     *            |     Zen1       |         True           |
     *            |     Zen2       |         True           |
     *            |     Zen3       |         True           |
     *            |     Zen4       |         True           |
     *            |     Zen5       |         True           |
     *
     * @param   none
     *
     * @return  true if cpu supports all features above,
     *          false otherwise
     */
    bool isX86_64v3() const;

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
     *            For more info refer to
     *            https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
     *
     *            |  AOCL 5.0.1  |  isX86_64v4()                 |
     *            |:-----------:|:-----------------------------:|
     *            | Sandybridge |            False              |
     *            | Broadwell   |            False              |
     *            | Skylake     |            True               |
     *            | Bulldozer   |            True               |
     *            | Zen1        |            True               |
     *            | Zen2        |            True               |
     *            | Zen3        |            True               |
     *            | Zen4        |            True               |
     *            | Zen5        |            True               |
     *
     * @param     none
     *
     * @return    true if cpu supports all features above,
     *            false otherwise
     */
    bool isX86_64v4() const;

    /**
     * @brief     Check if vendor is Intel
     *
     * @details   This function will work on all Intel processors.
     *            |   AOCL 5.0.1   |  isIntel()  |
     *            |:--------------:|:-----------:|
     *            |    Skylake     |    True     |
     *            |   Bulldozer    |    False    |
     *            |     Zen1       |    False    |
     *            |     Zen2       |    False    |
     *            |     Zen3       |    False    |
     *            |     Zen4       |    False    |
     *            |    Zen[X>4]    |    False    |
     *
     * @param     none
     *
     * @return    true if 'num' was an Intel x86-64, false otherwise
     */
    bool isIntel() const;

    /**
     * @brief     Check if the flag is suppored by the CPU ideintified by num.
     *
     * @details        List of supported flags: sse3, pclmulqdq, dtes64,
     * monitor, dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
     * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes,
     * xsave, osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc,
     * msr, pae, mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn,
     * clflush, ds, acpi, mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat,
     * fsgsbase, tsc_adjust, bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm,
     * mpx, avx512f, avx512dq, rdseed, adx, smap, avx512ifma, pcommit,
     * clflushopt, clwb, avx512pf, avx512er, avx512cd, sha_ni, avx512bw,
     * avx512vl, avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57, rdpid,
     * avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt, xsavec,
     * xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic, cr8legacy, abm,
     * sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop, skinit, wdt, lwp,
     * fma4, tce, nodeid_msr, tbm, topoext, perfctr_core, perfctr_nb, syscall,
     * nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow,
     * invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale, vmcb_clean,
     * flushbyasid, decodeassists, pause_filter, pfthreshold, xstore, xstore_en,
     * xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
     * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
     * avx512_vpintersect, x2avic
     *
     * @param[in] eflag    ECpuidFlag that needs to be checked
     *
     * @return    true if eflag is present in the availableflags and usable
     *            flags, false otherwise
     */
    bool hasFlag(ECpuidFlag const& eflag) const;

    /**
     * @brief     Check if the flag is suppored by the CPU ideintified by num.
     *            This function is used to check any of the flags are available.
     *
     * @details        List of supported flags: sse3, pclmulqdq, dtes64,
     * monitor, dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
     * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes,
     * xsave, osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc,
     * msr, pae, mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn,
     * clflush, ds, acpi, mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat,
     * fsgsbase, tsc_adjust, bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm,
     * mpx, avx512f, avx512dq, rdseed, adx, smap, avx512ifma, pcommit,
     * clflushopt, clwb, avx512pf, avx512er, avx512cd, sha_ni, avx512bw,
     * avx512vl, avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57, rdpid,
     * avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt, xsavec,
     * xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic, cr8legacy, abm,
     * sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop, skinit, wdt, lwp,
     * fma4, tce, nodeid_msr, tbm, topoext, perfctr_core, perfctr_nb, syscall,
     * nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow,
     * invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale, vmcb_clean,
     * flushbyasid, decodeassists, pause_filter, pfthreshold, xstore, xstore_en,
     * xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
     * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
     * avx512_vpintersect, x2avic
     *
     * @param[in] eflags    List of ECpuidFlag that needs to be checked
     *
     * @return    true if any eflags are present in the availableflags and
     *            usable
     *
     */
    bool hasFlags(Au::Memory::BufferView<ECpuidFlag> const& eflags,
                  HasFlagsMode const& mode = HasFlagsMode::Any) const;

    /**
     * @brief     Check if the flag is suppored by the CPU ideintified by num.
     *           This function is used to check all of the flags are available.
     *
     * @details        List of supported flags: sse3, pclmulqdq, dtes64,
     * monitor, dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
     * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes,
     * xsave, osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc,
     * msr, pae, mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn,
     * clflush, ds, acpi, mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat,
     * fsgsbase, tsc_adjust, bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm,
     * mpx, avx512f, avx512dq, rdseed, adx, smap, avx512ifma, pcommit,
     * clflushopt, clwb, avx512pf, avx512er, avx512cd, sha_ni, avx512bw,
     * avx512vl, avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57, rdpid,
     * avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt, xsavec,
     * xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic, cr8legacy, abm,
     * sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop, skinit, wdt, lwp,
     * fma4, tce, nodeid_msr, tbm, topoext, perfctr_core, perfctr_nb, syscall,
     * nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow,
     * invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale, vmcb_clean,
     * flushbyasid, decodeassists, pause_filter, pfthreshold, xstore, xstore_en,
     * xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
     * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
     * avx512_vpintersect, x2avic
     *
     * @param[in] eflags    List of ECpuidFlag that needs to be checked
     *
     * @return    true if all eflags are present in the availableflags and
     *            usable
     *
     */
    bool hasAllFlags(Au::Memory::BufferView<ECpuidFlag> const& eflags) const;

    /**
     * @brief     Check if the flag is suppored by the CPU ideintified by num.
     *
     * @details        List of supported flags: sse3, pclmulqdq, dtes64,
     * monitor, dscpl, vmx, smx, est, tm2, ssse3, cid, fma, cx16, xtpr, pdcm,
     * pcid, dca, sse4_1, sse4_2, x2apic, movbe, popcnt, tsc_deadline, aes,
     * xsave, osxsave, avx, f16c, rdrand, hypervisor, fpu, vme, de, pse, tsc,
     * msr, pae, mce, cx8, apic, sep, mtrr, pge, mca, cmov, pat, pse36, pn,
     * clflush, ds, acpi, mmx, fxsr, sse, sse2, ss, ht, tm, ia64, pbe, arat,
     * fsgsbase, tsc_adjust, bmi1, hle, avx2, smep, bmi2, erms, invpcid, rtm,
     * mpx, avx512f, avx512dq, rdseed, adx, smap, avx512ifma, pcommit,
     * clflushopt, clwb, avx512pf, avx512er, avx512cd, sha_ni, avx512bw,
     * avx512vl, avx512vbmi, umip, pku, ospke, avx512_vpopcntdq, la57, rdpid,
     * avx512_4vnniw, avx512_4fmaps, avx512_bf16, avxvnni, xsaveopt, xsavec,
     * xgetbv1, xsaves, lahf_lm, cmp_legacy, svm, extapic, cr8legacy, abm,
     * sse4a, misalignsse, _3dnowprefetch, osvw, ibs, xop, skinit, wdt, lwp,
     * fma4, tce, nodeid_msr, tbm, topoext, perfctr_core, perfctr_nb, syscall,
     * nxxd, mmxext, fxsr_opt, pdpe1gb, rdtscp, lmi64, _3dnowext, _3dnow,
     * invtsc, npt, lbrv, svm_lock, nrip_save, tsc_scale, vmcb_clean,
     * flushbyasid, decodeassists, pause_filter, pfthreshold, xstore, xstore_en,
     * xcrypt, xcrypt_en, ace2, ace2_en, phe, phe_en, pmm, pmm_en, vaes,
     * vpclmulqdq, avx512_vnni, avx512_bitalg, avx512vbmi2, movdiri, movdir64b,
     * avx512_vpintersect, x2avic
     *
     *            Note: The api is deprecated. Use hasFlag instead.
     *
     * @param[in] eflag    ECpuidFlag that needs to be checked
     *
     * @return    true if eflag is present in the availableflags and usable
     *            flags, false otherwise
     */
#ifdef AU_WARN_DEPRECATION
    [[deprecated("Use hasFlag instead.")]] bool
#else
    bool
#endif
    isAvailable(ECpuidFlag const& eflag) const;

    /**
     * @brief     Get microarchitecture of CPU from CPUID instruction.
     *
     * @details   The microarchitecture of the CPU can be
     *            Zen, ZenPlus, Zen2, Zen3, Zen4, Zen5
     *
     *            Will return Unknown if the microarchitecture is not
     *            supported
     *
     *            |  AOCL 5.0.1  |      getUarch()      |
     *            |:------------:|:--------------------:|
     *            |   Skylake    |       Unknown        |
     *            |  Bulldozer   |       Unknown        |
     *            |     Zen1     |         Zen          |
     *            |    Zen1+     |       ZenPlus        |
     *            |     Zen2     |         Zen2         |
     *            |     Zen3     |         Zen3         |
     *            |     Zen4     |         Zen4         |
     *            |     Zen5     |         Zen5         |
     *            |   Zen[X>5]   |       Unknown        |
     *
     * @param     none
     *
     * @return    Returns microarchitecture of CPU.
     */
    EUarch getUarch() const;

    // clang-format off
    /**
     * @brief     Checks microarchitecture from CPUID instruction and compare
     *            with input Like Zen, Zen2, Zen3 etc.
     *
     * @details   Given a microarchitecture, this function will check if the
     *            CPU microarchitecture is matched with input.
     *
     *  | AOCL 5.0.1 | isUarch(Zen) | isUarch(ZenPlus) | isUarch(Zen2) | isUarch(Zen3) | isUarch(Zen4) | isUarch(Zen5) |
     *  |:----------:|:------------:|:----------------:|:-------------:|:-------------:|:-------------:|:-------------:|
     *  |  Skylake   |    False     |      False       |     False     |     False     |     False     |     False     |
     *  | Bulldozer  |    False     |      False       |     False     |     False     |     False     |     False     |
     *  |   Zen1     |    True      |      False       |     False     |     False     |     False     |     False     |
     *  |   Zen1+    |    True      |       True       |     False     |     False     |     False     |     False     |
     *  |   Zen2     |    True      |       True       |      True     |     False     |     False     |     False     |
     *  |   Zen3     |    True      |       True       |      True     |      True     |     False     |     False     |
     *  |   Zen4     |    True      |       True       |      True     |      True     |      True     |     False     |
     *  |   Zen5     |    True      |       True       |      True     |      True     |      True     |      True     |
     *  |  Zen[X>5]  |    False     |      False       |     False     |     False     |     False     |     False     |
     *
     *  When given strict as true, it will check for exact match.
     *
     *  | AOCL 5.0.1 | isUarch(Zen,1) | isUarch(ZenPlus,1) | isUarch(Zen2,1) | isUarch(Zen3,1) | isUarch(Zen4,1) | isUarch(Zen5,1) |
     *  |:----------:|:--------------:|:------------------:|:---------------:|:---------------:|:---------------:|:---------------:|
     *  |  Skylake   |      False     |        False       |       False     |       False     |       False     |       False     |
     *  | Bulldozer  |      False     |        False       |       False     |       False     |       False     |       False     |
     *  |   Zen1     |      True      |        False       |       False     |       False     |       False     |       False     |
     *  |   Zen1+    |      False     |         True       |       False     |       False     |       False     |       False     |
     *  |   Zen2     |      False     |        False       |        True     |       False     |       False     |       False     |
     *  |   Zen3     |      False     |        False       |       False     |        True     |       False     |       False     |
     *  |   Zen4     |      False     |        False       |       False     |       False     |        True     |       False     |
     *  |   Zen5     |      False     |        False       |       False     |       False     |       False     |        True     |
     *  |  Zen[X>5]  |      False     |        False       |       False     |       False     |       False     |       False     |
     *
     *  <a href="#cpuid-c-apis"> C++-API Behaviour Summary </a>
     *
     * @param[in] arch   Microarchitecture input to check for.
     * @param[in] strict If true, then exact match is checked.
     *
     * @return    Returns true if CPU microarchitecture is matched with input.
     */
    bool isUarch(EUarch uarch, bool strict = false) const;
    // clang-format on

    /**
     * @brief     Check if the CPU is Zen family
     * @details   This function will check if the CPU is Zen family.
     *            Zen family includes Zen, ZenPlus, Zen2, Zen3, Zen4, Zen5
     *            microarchitectures.
     *            |   AOCL 5.0.1  |  isZenFamily()  |
     *            |:-------------:|:---------------:|
     *            |   Skylake     |     False       |
     *            |  Bulldozer    |     False       |
     *            |     Zen1      |     True        |
     *            |     Zen1+     |     True        |
     *            |     Zen2      |     True        |
     *            |     Zen3      |     True        |
     *            |     Zen4      |     True        |
     *            |     Zen5      |     True        |
     *            |   Zen[X>5]    |     True        |
     * @return    true if CPU is Zen family, false otherwise
     * @note      This function will return true for Zen family processors
     *            only.
     */
    bool isZenFamily() const;

    /**
     * @brief     Get the VendorInfo object
     *
     * @details   This function will return the VendorInfo object which contains
     *            the vendor, family, model, stepping and microarchitecture of
     *            the CPU.
     *
     * VendorInfo object contains the following fields:
     * 1. EVendor m_mfg;      // CPU manufacturing vendor.
     * 2. EFamily m_family;   // CPU family ID.
     * 3. Uint16  m_model;    // CPU model number.
     * 4. Uint16  m_stepping; // CPU stepping.
     * 5. EUarch  m_uarch;    // CPU microarchitecture.
     *
     * @param     none
     *
     * @return    VendorInfo
     */
    VendorInfo getVendorInfo() const;

    /**
     * @brief     Execute CPUID instruction and update the internal data.
     *
     * @param     none
     *
     * @return    none
     */
    void update();

    virtual EGoverner getGoverner() const override
    {
        return EGoverner::eUnknown;
    }

    CacheView getCacheView() const;

  private:
    class Impl;
    const Impl*           pImpl() const { return m_pimpl.get(); }
    Impl*                 pImpl() { return m_pimpl.get(); }
    std::unique_ptr<Impl> m_pimpl;
};

} // namespace Au
