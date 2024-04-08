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

#include "Au/Cpuid/Cpuid.hh"
#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Interface/Cpuid/ICpu.hh"
#include <map>
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
        while (std::getline(ss, token, ',')) {                                 \
            token        = token.substr(1, token.length() - 1);                \
            flags[token] = flagsCounter++;                                     \
        }                                                                      \
        for (auto value : values) {                                            \
            os << flags[value] << ":";                                         \
        }                                                                      \
        return os;                                                             \
    }

#include <memory>

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

/**
 * @struct  VendorInfo
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
                x2avic);

class AUD_API_EXPORT X86Cpu final : public CpuInfo
{
  public:
    X86Cpu(CpuidUtils* cUtils, CpuNumT num);
    X86Cpu(CpuNumT num = 0);

    ~X86Cpu();

    /**
     * @brief Check if vendor is AMD
     *
     * @return true if 'num' was an AMD x86-64, false otherwise
     */
    bool isAMD() const;

    /**
     * @brief Checks if processor is x86_64-v2 compliant
     *
     * @details
     *       Based on GCC following flags account for x86_64-v2
     *          cx16       lahf_lm
     *          popcnt     sse4_1
     *          sse4_2     ssse3
     *
     * @return  true if cpu supports all features above,
     *          false otherwise
     */
    bool isX86_64v2() const;
    /**
     * @brief Checks if processor is x86_64-v3 compliant
     *
     * @details
     *      Based on GCC following flags account for x86_64-v3
     *      (in addition to x86_64-v2)
     *        avx    avx2    bmi1
     *        bmi2   f16c    fma
     *        abm    movbe   xsave
     *
     * @return  true if cpu supports all features above,
     *          false otherwise
     */
    bool isX86_64v3() const;
    /**
     * @brief Checks if processor is x86_64-v4 compliant
     *
     * @details
     *      Based on GCC following flags account for x86_64-v4
     *      (in addition to x86_64-v2 + x86_64-v3)
     *       avx512f   avx512bw  avx512cd
     *       avx512dq  avx512vl
     *
     * @return  true if cpu supports all features above,
     *          false otherwise
     */
    bool isX86_64v4() const;

    /**
     * @brief Check if vendor is Intel
     *
     * @return true if 'num' was an Intel x86-64, false otherwise
     */
    bool isIntel() const;

    /**
     * @brief   Check if a given x86 cpu has a needed flag
     *
     * @param[in] eflag    ECpuidFlag that needs to be checked
     *
     * @return true if eflag is present in the availableflags and usable flags,
     * false otherwise
     */
    bool hasFlag(ECpuidFlag const& eflag) const;

    /**
     * @brief   Check if a given x86 cpu has a needed flag
     *  Note: The api is deprecated. Use hasFlag instead.
     * @param[in] eflag    ECpuidFlag that needs to be checked
     *
     * @return true if eflag is present in the availableflags and usable flags,
     * false otherwise
     */
#ifdef AU_WARN_DEPRECATION
    __attribute__((deprecated("Use hasFlag instead."))) bool
#else
    bool
#endif
    isAvailable(ECpuidFlag const& eflag) const;
    /**
     * @brief  Get microarchitecture of CPU from CPUID instruction.
     * @return  Returns microarchitecture of CPU.
     */
    EUarch getUarch() const;

    /**
     * @brief   Checks microarchitecture from CPUID instruction and compare with
     * input.
     *
     * Like Zen, Zen2, Zen3 etc to be compared with specific input.
     *
     * @param[in] arch Microarchitecture input to check for.
     * @param[in] strict If true, then exact match is checked.
     * @return  Returns true if CPU microarchitecture is matched with input.
     */
    bool isUarch(EUarch uarch, bool strict = false) const;

    /**
     * @brief Get the VendorInfo object
     * @return VendorInfo
     */
    VendorInfo getVendorInfo() const;
    /**
     * @brief Re-read all the cpuid functions and upate internal structures
     *
     * @parame none
     *
     * @return none
     */
    void update();

    virtual EGoverner getGoverner() const override { return EGoverner::eUnknown; }

    CacheView getCacheView() const;

  private:
    class Impl;
    const Impl*           pImpl() const { return m_pimpl.get(); }
    Impl*                 pImpl() { return m_pimpl.get(); }
    std::unique_ptr<Impl> m_pimpl;
};

} // namespace Au
