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

#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include "Au/Cpuid/Cpuid.hh"
#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Interface/Cpuid/ICpu.hh"
#include "Au/Memory/BufferView.hh"
#include "Au/StatusOr.hh"

#include <map>
#include <memory>
#include <sstream>
#include <vector>

#define AUD_DEFINE_ENUM(name, type, ...)                                           \
    enum class name : type                                                         \
    {                                                                              \
        Min,                                                                       \
        __VA_ARGS__,                                                               \
        Max,                                                                       \
    };                                                                             \
    inline std::stringstream& operator<<(std::stringstream&  os,                   \
                                         std::vector<String> values)               \
    {                                                                              \
        String                   str = #__VA_ARGS__;                               \
        std::map<String, Uint64> flags;                                            \
        Uint64                   flagsCounter = 1;                                 \
        std::stringstream        ss(str);                                          \
        String                   token;                                            \
        const size_t             maxEnumSize =                                     \
            static_cast<size_t>(name::Max) - 1; /* Maximum enum size */            \
                                                                                   \
        size_t enumCount = 0;                                                      \
                                                                                   \
        while (std::getline(ss, token, ',') && enumCount < maxEnumSize) {          \
            token        = (enumCount == 0) ? token                                \
                                            : token.substr(1, token.length() - 1); \
            flags[token] = flagsCounter++;                                         \
            enumCount++;                                                           \
        }                                                                          \
                                                                                   \
        for (const auto& value : values) {                                         \
            if (flags.find(value) != flags.end()) {                                \
                os << flags[value] << ":";                                         \
            }                                                                      \
        }                                                                          \
        return os;                                                                 \
    }                                                                              \
    inline uint64_t name##fromString(const std::string& str)                       \
    {                                                                              \
        std::string       s = #__VA_ARGS__;                                        \
        std::stringstream ss(s);                                                   \
        std::string       token;                                                   \
        uint64_t          index       = 1;                                         \
        const size_t      maxEnumSize = static_cast<size_t>(name::Max) - 1;        \
        size_t            enumCount   = 0;                                         \
        std::string       input       = str;                                       \
                                                                                   \
        /* First trim whitespace characters */                                     \
        auto trim = [](std::string& s) {                                           \
            auto start = s.find_first_not_of(" \t\n\r\f\v\n");                     \
            if (start == std::string::npos) {                                      \
                s.clear();                                                         \
                return;                                                            \
            }                                                                      \
            auto end = s.find_last_not_of(" \t\n\r\f\v\n");                        \
            s        = s.substr(start, end - start + 1);                           \
        };                                                                         \
                                                                                   \
        trim(input);                                                               \
                                                                                   \
        /* Handle empty or whitespace-only string */                               \
        if (input.empty()) {                                                       \
            return -1;                                                             \
        }                                                                          \
                                                                                   \
        /* Validate input - must contain only alphanumeric and _ chars */          \
        for (char c : input) {                                                     \
            if (!std::isalnum(c) && c != '_') {                                    \
                return -1;                                                         \
            }                                                                      \
        }                                                                          \
                                                                                   \
        /* Process enum values */                                                  \
        while (std::getline(ss, token, ',') && enumCount < maxEnumSize) {          \
            trim(token);                                                           \
            if (token == input) {                                                  \
                return index;                                                      \
            }                                                                      \
            index++;                                                               \
            enumCount++;                                                           \
        }                                                                          \
        return -1;                                                                 \
    }                                                                              \
    inline std::string name##toString(Uint64 value)                                \
    {                                                                              \
        String            str = #__VA_ARGS__;                                      \
        std::stringstream ss(str);                                                 \
        Uint64            current = 1;                                             \
        String            token;                                                   \
        const size_t      maxEnumSize = static_cast<size_t>(name::Max) - 1;        \
        size_t            enumCount   = 0;                                         \
                                                                                   \
        if (value == 0) {                                                          \
            return "UNDEF";                                                        \
        }                                                                          \
                                                                                   \
        while (std::getline(ss, token, ',') && enumCount < maxEnumSize) {          \
            if (current == value) {                                                \
                if (enumCount == 0) {                                              \
                    return token;                                                  \
                }                                                                  \
                token = token.substr(1, token.length() - 1);                       \
                return token;                                                      \
            }                                                                      \
            current++;                                                             \
            enumCount++;                                                           \
        }                                                                          \
        return "UNDEF";                                                            \
    }

namespace Au {

/**
 * @brief Processor micro-architecture info.
 *
 * @note  Mirror of the C enum @c au_uarch_t in
 *        SDK/Include/Capi/au/cpuid/cpuid_flags.h. The two MUST stay
 *        value-for-value identical (the C core resolves the uarch; the C++
 *        wrapper casts au_uarch_t -> EUarch). If you edit this enum, update
 *        au_uarch_t too, and vice versa.
 */
enum class EUarch : Uint16
{
    Unknown = 0,
    Zen,
    ZenPlus,
    Zen2,
    Zen3,
    Zen4,
    Zen5,
    Zen6,
    Max = Zen6,
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
 *
 * @note  Mirror of the C enum @c au_cpuid_flag_t (generated from the
 *        @c AU_CPUID_FLAG_LIST X-macro) in
 *        SDK/Include/Capi/au/cpuid/cpuid_flags.h. The order and values MUST
 *        stay identical -- they are the bitmap indices and the CPUID_MAP
 *        feature ids shared by the C core. If you add/reorder a flag here,
 *        make the SAME change to AU_CPUID_FLAG_LIST, and vice versa.
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
                x2avic,
                clzero,
                avx512_bmm,
                avx512_fp16,
                avx_ifma,
                avx_vnni_int8,
                avx_ne_convert,
                avx_vnni_int16,
                avx10)

class AUD_API_EXPORT X86Cpu final : public CpuInfo
{
  public:
    /**
     * @brief  Test-only constructor from CpuidUtils object.
     * @param[in] cUtils  CpuidUtils object to use for fetching CPUID info.
     * @param[in] num     CPU number to fetch info from.
     */
    X86Cpu(CpuidUtils* cUtils, CpuNumT num);

    /**
     * @brief   Best-effort constructor probing a deterministic core.
     * @details Probes CPUID with a deterministic, cross-platform contract. Never
     *          fails; degrades to the current thread on error. For a variant that
     *          reports failure, use buildFromCore(). Resolves and pins: AU_CURRENT
     *          -> lowest-allowed (or E-core for hybrid Intel); specific in-mask ->
     *          that core; specific out-of-mask -> degrade to lowest.
     * @warning If a concrete num is honored, the calling thread is pinned to that
     *          core for the probe, then restored.
     * @param[in] num  Core number, or AU_CURRENT_CPU_NUM (default) for crash-safe.
     */
    X86Cpu(CpuNumT num = AU_CURRENT_CPU_NUM);

    /**
     * @brief   Strict factory: build an X86Cpu, reporting affinity failures.
     * @details Validates the core request; returns InvalidArgument if the core is
     *          outside the affinity mask. AU_CURRENT_CPU_NUM never fails.
     * @param[in] num  Concrete core number, or AU_CURRENT_CPU_NUM.
     * @return    StatusOr<X86Cpu>: the object on success, or InvalidArgument.
     */
    static StatusOr<X86Cpu> buildFromCore(CpuNumT num);

    /**
     * @brief   Move constructor (explicit; required for StatusOr<X86Cpu>).
     */
    X86Cpu(X86Cpu&&) noexcept;

    /**
     * @brief   Destructor.
     */
    ~X86Cpu();

    /**
     * @brief     Check if vendor is AMD.
     * @return    true if AMD x86-64, false otherwise.
     */
    bool isAMD() const;

    /**
     * @brief     Checks if processor is x86_64-v2 compliant.
     * @details   x86-64-v2 = x86-64 + cx16, lahf_lm, popcnt, sse4_1, sse4_2, ssse3.
     * @return    true if all features present, false otherwise.
     */
    bool isX86_64v2() const;

    /**
     * @brief     Checks if processor is x86_64-v3 compliant.
     * @details   x86-64-v3 = v2 + avx, avx2, bmi1, bmi2, f16c, fma, abm, movbe, xsave.
     * @return    true if all features present, false otherwise.
     */
    bool isX86_64v3() const;

    /**
     * @brief     Checks if processor is x86_64-v4 compliant.
     * @details   x86-64-v4 = v3 + avx512f, avx512bw, avx512cd, avx512dq, avx512vl.
     * @return    true if all features present, false otherwise.
     */
    bool isX86_64v4() const;

    /**
     * @brief     Check if vendor is Intel.
     * @return    true if Intel x86-64, false otherwise.
     */
    bool isIntel() const;

    /**
     * @brief     Check if the flag is supported by the CPU.
     * @param[in] eflag    ECpuidFlag to check.
     * @return    true if flag is available and usable, false otherwise.
     */
    bool hasFlag(ECpuidFlag const& eflag) const;

    /**
     * @brief     Check if any/all flags are supported.
     * @param[in] eflags    List of ECpuidFlag to check.
     * @param[in] mode      Any (default) or All.
     * @return    true if condition met, false otherwise.
     */
    bool hasFlags(Au::Memory::BufferView<ECpuidFlag> const& eflags,
                  HasFlagsMode const& mode = HasFlagsMode::Any) const;

    /**
     * @brief     Check if all flags are supported.
     * @param[in] eflags    List of ECpuidFlag to check.
     * @return    true if all flags are present and usable, false otherwise.
     */
    bool hasAllFlags(Au::Memory::BufferView<ECpuidFlag> const& eflags) const;

    /**
     * @brief     Deprecated. Use hasFlag instead.
     * @param[in] eflag    ECpuidFlag to check.
     * @return    true if flag is available and usable, false otherwise.
     */
#ifdef AU_WARN_DEPRECATION
    [[deprecated("Use hasFlag instead.")]] bool
#else
    bool
#endif
    isAvailable(ECpuidFlag const& eflag) const;

    /**
     * @brief     Get microarchitecture from CPUID.
     * @details   Returns Zen/Zen2/Zen3/Zen4/Zen5/Zen6 for AMD; Unknown for others.
     * @return    Microarchitecture of CPU.
     */
    EUarch getUarch() const;

    /**
     * @brief     Check if CPU microarchitecture matches input.
     * @details   strict=false: Zen4 matches Zen/Zen2/Zen3/Zen4. strict=true: exact match only.
     * @param[in] uarch   Microarchitecture to check.
     * @param[in] strict  If true, exact match required.
     * @return    true if match, false otherwise.
     */
    bool isUarch(EUarch uarch, bool strict = false) const;

    /**
     * @brief     Check if CPU is Zen family (Zen, Zen2, Zen3, Zen4, Zen5, Zen6).
     * @return    true if Zen family, false otherwise.
     */
    bool isZenFamily() const;

    /**
     * @brief     Get the VendorInfo object.
     * @details   Contains vendor, family, model, stepping, and microarchitecture.
     * @return    VendorInfo
     */
    VendorInfo getVendorInfo() const;

    /**
     * @brief     Execute CPUID and update internal data.
     */
    void update();

    virtual EGoverner getGoverner() const override
    {
        return EGoverner::eUnknown;
    }

    CacheView getCacheView() const;

  private:
    /**
     * @brief   Strict-aware delegating constructor (internal).
     * @details Forwards to C core resolver (au_cpuid_init). strict=true reports
     *          failures; strict=false degrades. All selection/pinning in C core.
     */
    X86Cpu(CpuNumT num, bool strict);

    class Impl;
    const Impl*           pImpl() const { return m_pimpl.get(); }
    Impl*                 pImpl() { return m_pimpl.get(); }
    std::unique_ptr<Impl> m_pimpl;

    /* True on successful resolve; false only when strict specific-core is out-of-mask. */
    bool m_resolved = true;

    /* All affinity logic now lives in the pure-C core resolver (au_cpuid_init). */
};

} // namespace Au
