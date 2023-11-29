/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Cpuid/Cpuid.hh"

#include <memory>

namespace Au {

/**
 * @enum ECpuidFlag
 * @brief Flags supported by CPU as features.
 */
enum class ECpuidFlag : Uint64
{
    /* EAX = 0x0000_0001 , result in ECX and EDX */
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

    /* EAX = 0x0000_0006 */
    arat,

    /* EAX = 0x0000_0007, result in EBX/ECX/EDX */
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

    /* EAX = 0x0000_000D , result in EAX */
    xsaveopt,
    xsavec,
    xgetbv1,
    xsaves,

    /* EAX = 0x8000_0001 result in ECX/EDX */
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

    /* EAX = 0x8000_0007 , result in EDX */
    invtsc,

    /* EAX = 0x8000_000A, result in EDX */
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

    /* EAX = 0xC000_0001, result in EDX */
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

    MAX
};


class AUD_API_EXPORT X86Cpu final : public CpuInfo
{
  public:
    X86Cpu(CpuNumT num = 0);
    ~X86Cpu();

    /**
     * @brief Check if vendor is AMD
     *
     * @return true if 'num' was an AMD x86-64, false otherwise
     */
    bool isAMD() const;

    bool isX86_64v2() const;
    bool isX86_64v3() const;
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
     * @param[in] ef    ECpuidFlag that needs to be checked
     *
     * @return true if 'num' was an AMD x86-64, false otherwise
     */
    bool hasFlag(ECpuidFlag const& ef) const;

    /**
     * @brief Re-read all the cpuid functions and upate internal structures
     *
     * @parame none
     *
     * @return none
     */
    void update();

    virtual EGoverner getGoverner() const override;

    CacheView getCacheView() const;

  private:
    class Impl;
    const Impl*           pImpl() const { return m_pimpl.get(); }
    Impl*                 pImpl() { return m_pimpl.get(); }
    std::unique_ptr<Impl> m_pimpl;
};

} // namespace Au
