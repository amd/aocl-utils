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

#include "alci/cxx/cpuidrawdata.hh"

namespace alci {

#include "alci/macros.h"

// clang-format off
struct _cpuid_values cpuid_values[ALC_CPUID_MAX] = {
    { 0x1, 0x0 },        /* eax = 1, ecx=0   ALC_CPUID_EAX_1*/
    { 0x7, 0x0 },        /* eax = 7,  -"-    ALC_CPUID_EAX_7*/
    { 0x80000001, 0x0 }, /* eax = 0x80000001 ALC_CPUID_EAX_8_01*/
    { 0x80000007, 0x0 }, /* eax = 0x80000007 ALC_CPUID_EAX_8_07*/
    { 0x80000008, 0x0 }, /* eax = 0x80000008 ALC_CPUID_EAX_8_08*/
    { 0x8000001D, 0x0 }, /* eax = 0x8000001D, ecx = 0x0 ALC_CPUID_EAX_8_1D_ECX_00*/
    { 0x8000001D, 0x1 }, /* eax = 0x8000001D, ecx = 0x1 ALC_CPUID_EAX_8_1D_ECX_01*/
    { 0x8000001D, 0x2 }, /* eax = 0x8000001D, ecx = 0x2 ALC_CPUID_EAX_8_1D_ECX_02*/
    { 0x8000001D, 0x3 }  /* eax = 0x8000001D, ecx = 0x3 ALC_CPUID_EAX_8_1D_ECX_03*/
};
// clang-format on

struct _cpuid_flag cpuid_flags[ALC_CPUID_FLAG_MAX] = {
    { ALC_CPUID_EAX_1, cpuid_reg::ecx, ALC_CPUID_BIT_SSSE3 },
    { ALC_CPUID_EAX_1, cpuid_reg::ecx, ALC_CPUID_BIT_AVX },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX2 },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_TSX },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_SHA },
    { ALC_CPUID_EAX_1, cpuid_reg::ecx, ALC_CPUID_BIT_AES },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_VAES },
    { ALC_CPUID_EAX_1, cpuid_reg::ecx, ALC_CPUID_BIT_RDRND },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_RDSEED },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_ADX },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_BMI2 },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512F },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512DQ },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512BW },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512ER },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512CD },
    { ALC_CPUID_EAX_7,
      cpuid_reg::ebx,
      static_cast<alc_flag_bit_t>(ALC_CPUID_BIT_AVX512VL) },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512PF },
    { ALC_CPUID_EAX_7, cpuid_reg::ebx, ALC_CPUID_BIT_AVX512_IFMA },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_AVX512_VNNI },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_AVX512_BITALG },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_AVX512_VBMI },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_AVX512_VBMI2 },
    { ALC_CPUID_EAX_7, cpuid_reg::ecx, ALC_CPUID_BIT_AVX512_VPOPCNTDQ }
};

/**
 * \brief   Function to query CPUID instruction based on EAX input parameter.
 *
 * Function which has assembly code to query CPUID instruction
 * based on EAX input value. EAX should have valid value to
 * get CPUID information and other EBX, ECX, EDX should have 0s.
 * In some cases, ECX also will have valid values.
 *
 * \param[in] eax EAX value.
 * \param[out] out regs pointer which has EAX, EBX, ECX, EDX values.
 */
static inline void
cpuid_1(uint32_t eax, CpuIdRawData::regs& out)
{
    asm volatile("cpuid"
                 : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3])
                 : "0"(eax));
}

/**
 * \brief   Function to query CPUID instruction based on EAX, ECX input
 * parameters.
 *
 * Function which has assembly code to query CPUID instruction
 * based on EAX, ECX input values. EAX should have valid value to
 * get CPUID information and other EBX, ECX, EDX should have 0s.
 * In some cases, ECX also will have valid values.
 *
 * \param[in] eax EAX value.
 * \param[in] ecx ECX value.
 * \param[out] out regs pointer which has EAX, EBX, ECX, EDX values.
 */
static inline void
cpuid_2(uint32_t eax, uint32_t ecx, CpuIdRawData::regs& out)
{
    asm volatile("cpuid"
                 : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3])
                 : "0"(eax), "2"(ecx));
}

/**
 * \brief   Get CPU manufacturing info from CPUID instruction.
 *
 * It gives CPU Family, Model & Stepping.
 *
 * \param[in]   cpuid_regs  CpuId registers result values.
 * \param[out]  mfg_info    Updates CPU manufacturing info.
 */
static void
get_mfg_info(CpuIdRawData::regs cpuid_regs, struct VendorInfo* mfg_info)
{
    uint16_t model;
    uint16_t family;

    if (mfg_info) {
        family = alc_cpuid_get_family(cpuid_regs[0]);
        model  = alc_cpuid_get_model(cpuid_regs[0]);

        if (family >= ALC_CPU_FAMILY_ZEN) {
            mfg_info->m_family = family;
            mfg_info->m_model  = model;
        }

        mfg_info->m_stepping = alc_cpuid_get_stepping(cpuid_regs[0]);
    }
}

/**
 * \brief   Updates CPU vendor info details.
 *
 * Initialize CPU Vendor info with Vendor name, manufacturing info and
 * all CPUID results with different eax, ecx register values.
 * It gives CPU Family, Model & Stepping.
 *
 */
CpuIdRawData::CpuIdRawData()
{
    update();
}

/**
 * \brief  Update Raw Data by re-reading the registers
 *
 * \return None
 */
void
CpuIdRawData::update()
{
    int                 arr_size = ARRAY_SIZE(cpuid_values);
    VendorInfo*         mfg_info = &m_vendor_info;
    CpuIdRawData::regs  regs     = at(0x0);

    /* "AuthenticAMD" */
    if (regs[1] == 0x68747541 && regs[2] == 0x444d4163
        && regs[3] == 0x69746e65) {
        m_vendor_info.m_mfg = Vendor::eAmd;
    }

    for (int i = 0; i < arr_size; i++) {
        m_cpuid[i] = at(cpuid_values[i].eax, cpuid_values[i].ecx);
    }

    get_mfg_info(m_cpuid[ALC_CPUID_EAX_1], mfg_info);

    /*
     * Globally disable some *_USEABLE flags, so that all ifunc's
     * sees them
     */
    if (m_vendor_info.m_mfg == Vendor::eAmd
        && m_vendor_info.m_family >= ALC_CPU_FAMILY_ZEN) {
        /* todo */
    }

    // initialized = INITIALIZED_MAGIC;

#if defined(DEVELOPER)
    /* We override any from  */
    const char* env_cpuid_str = ALC_ENV_ALC_CPUID;
    alc_env_get(env_cpuid_str, sizeof(env_cpuid_str), mfg_info);
#endif
}

/**
 * \brief       Convert given cpuid to offset by setting top most bit.
 *
 * \param[in]   eax   EAX register value.
 *
 * \return      Returns converted offset value.
 */
constexpr uint32_t
CpuIdRawData::toOffset(uint32_t eax)
{
    return (eax & ~0x8000'0000);
}

/**
 * \brief       Convert offset to cpuid register value.
 *
 * \param[in]   offset   Offset value.
 *
 * \return      Returns converted cpuid value.
 */
constexpr uint32_t
CpuIdRawData::toExtCpuId(uint32_t offset)
{
    return (offset | 0x8000'0000);
}

/**
 * \brief       Get CPUID output based on eax register value as input.
 *
 * \param[in]   eax   EAX register value.
 *
 * \return      Returns register values after performing cpuid.
 */
CpuIdRawData::regs
CpuIdRawData::at(uint32_t eax)
{
    CpuIdRawData::regs result;
    cpuid_1(eax, result);

    return result;
}

/**
 * \brief       Get CPUID output based on eax, ecx register values as input.
 *
 * \param[in]   eax   EAX register value.
 * \param[in]   ecx   ECX register value.
 *
 * \return      Returns register values after performing cpuid.
 */
CpuIdRawData::regs
CpuIdRawData::at(uint32_t eax, uint32_t ecx)
{
    CpuIdRawData::regs result;
    cpuid_2(eax, ecx, result);

    return result;
}
} // namespace alci
