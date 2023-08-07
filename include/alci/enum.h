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

#ifndef ALCI_ENUM_H
#define ALCI_ENUM_H

#include "alci/macros.h"

#include <stdint.h>

EXTERN_C_BEGIN

typedef uint32_t alc_core_num_t;

/**
 * \enum alc_cpu_id
 * \brief CPUID values for EAX, ECX.
 */
typedef enum
{
    ALC_CPUID_EAX_1 = 0,       /**< EAX = 1, ECX = 0. */
    ALC_CPUID_EAX_7,           /**< EAX = 7, ECX = 0. */
    ALC_CPUID_EAX_8_01,        /**< EAX = 0x80000001, ECX = 0. */
    ALC_CPUID_EAX_8_07,        /**< EAX = 0x80000007, ECX = 0. */
    ALC_CPUID_EAX_8_08,        /**< EAX = 0x80000008, ECX = 0. */
    ALC_CPUID_EAX_8_1D_ECX_00, /**< EAX = 0x8000001D, ECX = 0. */
    ALC_CPUID_EAX_8_1D_ECX_01, /**< EAX = 0x8000001D, ECX = 1. */
    ALC_CPUID_EAX_8_1D_ECX_02, /**< EAX = 0x8000001D, ECX = 2. */
    ALC_CPUID_EAX_8_1D_ECX_03, /**< EAX = 0x8000001D, ECX = 3. */
    /* Last entry */
    ALC_CPUID_MAX /**< Last entry. */
} alc_cpu_id;

/**
 * \enum alc_flag_bit_t
 * \brief Different flags of EAX, EBX, ECX, EDX output values of CPUID.
 */
typedef enum
{
    /*
     * ALC_CPUID
     * EAX = 1, ECX = 0 ALC_CPUID
     */

    /* ECX Values */
    ALC_CPUID_BIT_SSE3   = (1u << 0),
    ALC_CPUID_BIT_SSSE3  = (1u << 9),
    ALC_CPUID_BIT_FMA    = (1u << 12),
    ALC_CPUID_BIT_SSE4_1 = (1u << 19),
    ALC_CPUID_BIT_SSE4_2 = (1u << 20),
    ALC_CPUID_BIT_AES    = (1u << 25),
    ALC_CPUID_BIT_AVX    = (1u << 28),
    ALC_CPUID_BIT_RDRND  = (1u << 30),

    /* EDX Values */
    ALC_CPUID_BIT_FPU  = (1u << 0),
    ALC_CPUID_BIT_MMX  = (1u << 23),
    ALC_CPUID_BIT_FXSR = (1u << 24),
    ALC_CPUID_BIT_SSE  = (1u << 25),
    ALC_CPUID_BIT_SSE2 = (1u << 26),
    ALC_CPUID_BIT_SS   = (1u << 27),

    /*
     * ALC_CPUID
     * EAX = 0x7, ECX = 0
     */

    /* EBX values */
    ALC_CPUID_BIT_FSGSBASE    = (1u << 0),
    ALC_CPUID_BIT_TSC_ADJUST  = (1u << 1),
    ALC_CPUID_BIT_SGX         = (1u << 2),
    ALC_CPUID_BIT_BMI1        = (1u << 3),
    ALC_CPUID_BIT_HLE         = (1u << 4),
    ALC_CPUID_BIT_AVX2        = (1u << 5),
    ALC_CPUID_BIT_SMEP        = (1u << 7),
    ALC_CPUID_BIT_BMI2        = (1u << 8),
    ALC_CPUID_BIT_ERMS        = (1u << 9),
    ALC_CPUID_BIT_INVPCID     = (1u << 10),
    ALC_CPUID_BIT_RTM         = (1u << 11),
    ALC_CPUID_BIT_TSX         = ALC_CPUID_BIT_RTM,
    ALC_CPUID_BIT_PQM         = (1u << 12),
    ALC_CPUID_BIT_MPX         = (1u << 14),
    ALC_CPUID_BIT_PQE         = (1u << 15),
    ALC_CPUID_BIT_AVX512F     = (1u << 16),
    ALC_CPUID_BIT_AVX512DQ    = (1u << 17),
    ALC_CPUID_BIT_RDSEED      = (1u << 18),
    ALC_CPUID_BIT_ADX         = (1u << 19),
    ALC_CPUID_BIT_SMAP        = (1u << 20),
    ALC_CPUID_BIT_AVX512_IFMA = (1u << 21),
    ALC_CPUID_BIT_CLFLUSHOPT  = (1u << 22),
    ALC_CPUID_BIT_CLWB        = (1u << 24),
    ALC_CPUID_BIT_TRACE       = (1u << 25),
    ALC_CPUID_BIT_AVX512PF    = (1u << 26),
    ALC_CPUID_BIT_AVX512ER    = (1u << 27),
    ALC_CPUID_BIT_AVX512CD    = (1u << 28),
    ALC_CPUID_BIT_SHA         = (1u << 29),
    ALC_CPUID_BIT_AVX512BW    = (1u << 30),
#define ALC_CPUID_BIT_AVX512VL (1u << 31)

    /* ECX Values*/
    ALC_CPUID_BIT_PREFETCHWT1      = (1u << 0),
    ALC_CPUID_BIT_AVX512_VBMI      = (1u << 1),
    ALC_CPUID_BIT_UMIP             = (1u << 2),
    ALC_CPUID_BIT_PKU              = (1u << 3),
    ALC_CPUID_BIT_OSPKE            = (1u << 4),
    ALC_CPUID_BIT_WAITPKG          = (1u << 5),
    ALC_CPUID_BIT_AVX512_VBMI2     = (1u << 6),
    ALC_CPUID_BIT_SHSTK            = (1u << 7),
    ALC_CPUID_BIT_GFNI             = (1u << 8),
    ALC_CPUID_BIT_VAES             = (1u << 9),
    ALC_CPUID_BIT_VPCLMULQDQ       = (1u << 10),
    ALC_CPUID_BIT_AVX512_VNNI      = (1u << 11),
    ALC_CPUID_BIT_AVX512_BITALG    = (1u << 12),
    ALC_CPUID_BIT_AVX512_VPOPCNTDQ = (1u << 14),
    ALC_CPUID_BIT_RDPID            = (1u << 22),
    ALC_CPUID_BIT_CLDEMOTE         = (1u << 25),
    ALC_CPUID_BIT_MOVDIRI          = (1u << 27),
    ALC_CPUID_BIT_MOVDIR64B        = (1u << 28),
    ALC_CPUID_BIT_SGX_LC           = (1u << 30),

    /* EDX Values */
    ALC_CPUID_BIT_AVX512_4VNNIW = (1u << 2),
    ALC_CPUID_BIT_AVX512_4FMAPS = (1u << 3),
    ALC_CPUID_BIT_FSRM          = (1u << 4),
    ALC_CPUID_BIT_PCONFIG       = (1u << 18),
    ALC_CPUID_BIT_IBT           = (1u << 20),
    ALC_CPUID_BIT_IBRS_IBPB     = (1u << 26),
    ALC_CPUID_BIT_STIBP         = (1u << 27),
    ALC_CPUID_BIT_CAPABILITIES  = (1u << 29),
#define ALC_CPUID_BIT_SSBD (1u << 31)

    /*
     * ALC_CPUID
     * EAX = 0x1D
     */

    ALC_CPUID_BIT_WBINVD            = (1u << 0),
    ALC_CPUID_BIT_CACHE_INCLUSIVE   = (1u << 1),
    ALC_CPUID_BIT_SELF_INITIALIZING = (1u << 8),
    ALC_CPUID_BIT_FULLYASSOCIATIVE  = (1u << 9)
} alc_flag_bit_t;

/* todo: New enum is added with the supported flags,
         support for remaining flags will be added on need basis.
typedef enum {
  ALC_E_FLAG_FPU,
  ALC_E_FLAG_VME,
  ALC_E_FLAG_DE,
  ALC_E_FLAG_PSE,
  ALC_E_FLAG_TSC,
  ALC_E_FLAG_MSR,
  ALC_E_FLAG_PAE,
  ALC_E_FLAG_MCE,
  ALC_E_FLAG_CX8,
  ALC_E_FLAG_APIC,
  ALC_E_FLAG_SEP,
  ALC_E_FLAG_MTRR,
  ALC_E_FLAG_PGE,
  ALC_E_FLAG_MCA,
  ALC_E_FLAG_CMOV,
  ALC_E_FLAG_PAT,
  ALC_E_FLAG_PSE36,
  ALC_E_FLAG_CLFLUSH,
  ALC_E_FLAG_MMX,
  ALC_E_FLAG_FXSR,
  ALC_E_FLAG_SSE,
  ALC_E_FLAG_SSE2,
  ALC_E_FLAG_HT,
  ALC_E_FLAG_SYSCALL,
  ALC_E_FLAG_NX,
  ALC_E_FLAG_MMXEXT,
  ALC_E_FLAG_FXSR_OPT,
  ALC_E_FLAG_PDPE1GB,
  ALC_E_FLAG_RDTSCP,
  ALC_E_FLAG_LM,
  ALC_E_FLAG_REP_GOOD,
  ALC_E_FLAG_NOPL,
  ALC_E_FLAG_CPUID,
  ALC_E_FLAG_EXTD_APICID,
  ALC_E_FLAG_PNI,
  ALC_E_FLAG_PCLMULQDQ,
  ALC_E_FLAG_SSSE3,
  ALC_E_FLAG_FMA,
  ALC_E_FLAG_CX16,
  ALC_E_FLAG_SSE4_1,
  ALC_E_FLAG_SSE4_2,
  ALC_E_FLAG_MOVBE,
  ALC_E_FLAG_POPCNT,
  ALC_E_FLAG_AES,
  ALC_E_FLAG_XSAVE,
  ALC_E_FLAG_AVX,
  ALC_E_FLAG_F16C,
  ALC_E_FLAG_RDRAND,
  ALC_E_FLAG_HYPERVISOR,
  ALC_E_FLAG_LAHF_LM,
  ALC_E_FLAG_CMP_LEGACY,
  ALC_E_FLAG_CR8_LEGACY,
  ALC_E_FLAG_ABM,
  ALC_E_FLAG_SSE4A,
  ALC_E_FLAG_MISALIGNSSE,
  ALC_E_FLAG_3DNOWPREFETCH,
  ALC_E_FLAG_OSVW,
  ALC_E_FLAG_TOPOEXT,
  ALC_E_FLAG_SSBD,
  ALC_E_FLAG_IBPB,
  ALC_E_FLAG_VMMCALL,
  ALC_E_FLAG_FSGSBASE,
  ALC_E_FLAG_BMI1,
  ALC_E_FLAG_AVX2,
  ALC_E_FLAG_SMEP,
  ALC_E_FLAG_BMI2,
  ALC_E_FLAG_RDSEED,
  ALC_E_FLAG_ADX,
  ALC_E_FLAG_SMAP,
  ALC_E_FLAG_CLFLUSHOPT,
  ALC_E_FLAG_SHA_NI,
  ALC_E_FLAG_XSAVEOPT,
  ALC_E_FLAG_XSAVEC,
  ALC_E_FLAG_XGETBV1,
  ALC_E_FLAG_XSAVES,
  ALC_E_FLAG_XSAVEERPTR,
  ALC_E_FLAG_VIRT_SSBD,
  ALC_E_FLAG_ARAT,

  // AVX512 Specific
  ALC_E_FLAG_AVX512F,
  ALC_E_FLAG_AVX512DQ,
  ALC_E_FLAG_AVX512_IFMA,
  ALC_E_FLAG_AVX512PF,
  ALC_E_FLAG_AVX512ER,
  ALC_E_FLAG_AVX512CD,
  ALC_E_FLAG_AVX512BW,
  ALC_E_FLAG_AVX512VL,
  ALC_E_FLAG_AVX512_VBMI,
  ALC_E_FLAG_AVX512_VBMI2,
  ALC_E_FLAG_AVX512_VNNI,
  ALC_E_FLAG_AVX512_BITALG,
  ALC_E_FLAG_AVX512_VPOPCNTDQ,
  ALC_E_FLAG_AVX512_4VNNIW,
  ALC_E_FLAG_AVX512_4FMAPS
} alc_e_flag_t;
*/

/**
 * \enum alc_flag_t
 * \brief Flags supported by CPU as features.
 */
typedef enum
{
    ALC_E_FLAG_SSSE3 = 0,
    ALC_E_FLAG_AVX,
    ALC_E_FLAG_AVX2,
    ALC_E_FLAG_TSX,
    ALC_E_FLAG_SHA_NI,
    ALC_E_FLAG_AES,
    ALC_E_FLAG_VAES,
    ALC_E_FLAG_RDRAND,
    ALC_E_FLAG_RDSEED,
    ALC_E_FLAG_ADX,
    ALC_E_FLAG_BMI2,
    ALC_E_FLAG_AVX512F,
    ALC_E_FLAG_AVX512DQ,
    ALC_E_FLAG_AVX512BW,
    ALC_E_FLAG_AVX512ER,
    ALC_E_FLAG_AVX512CD,
    ALC_E_FLAG_AVX512VL,
    ALC_E_FLAG_AVX512PF,
    ALC_E_FLAG_AVX512_IFMA,
    ALC_E_FLAG_AVX512_VNNI,
    ALC_E_FLAG_AVX512_BITALG,
    ALC_E_FLAG_AVX512_VBMI,
    ALC_E_FLAG_AVX512_VBMI2,
    ALC_E_FLAG_AVX512_VPOPCNTDQ,
    ALC_CPUID_FLAG_MAX
} alc_flag_t;

/**
 * \enum alc_cache_level_t
 * \brief Cache levels.
 */
typedef enum
{
    ALC_E_L1 = 1, /**< L1 cache.*/
    ALC_E_L2,     /**< L2 cache.*/
    ALC_E_L3,     /**< L3 cache.*/
    ALC_E_L4,     /**< L4 cache.*/
    ALC_E_L5,     /**< L5 cache.*/
    ALC_E_UNKNOWN /**< Unknown cache.*/
} alc_cache_level_t;

/**
 * \enum alc_cache_type_t
 * \brief Cache types.
 */
typedef enum
{
    ALC_E_TYPE_ICACHE = 1, /**< Instruction Cache. */
    ALC_E_TYPE_DCACHE,     /**< Data Cache. */
    ALC_E_TYPE_UNIFIED,    /**< Unified Cache. */
    ALC_E_TYPE_UNKNOWN     /**< Unable to determine. */
} alc_cache_type_t;

/**
 * \enum alc_cpuid_error_t
 * \brief CPUID error status.
 */
typedef enum
{
    ALC_E_FAILURE = 0, /**< Failure status.*/
    ALC_E_SUCCESS      /**< Success status.*/
} alc_cpuid_error_t;

/**
 * \struct cpuid_values
 * \brief  EAX and ECX values required for performing CPUID instruction.
 */
struct _cpuid_values
{
    uint32_t eax; /**< EAX register value. */
    uint32_t ecx; /**< ECX register value. */
};

/** \enum cpuid_reg
 *  \brief CPUID registers.
 */
typedef enum
{
    eax = 0, /**< EAX register. */
    ebx,     /**< EBX register. */
    ecx,     /**< ECX register. */
    edx      /**< EDX register. */
} cpuid_reg;

// clang-format off
/**
 * \struct cpuid_flag
 * \brief  The available feature flags details of CPU.
 */
struct _cpuid_flag
{
    alc_cpu_id id; /**< CPUID register values index for pointing to cpuid_values
                      structure. */
    cpuid_reg      reg; /**< CPUID registers. */
    alc_flag_bit_t flg; /**< Flags of output register values of CPUID. */
};
// clang-format on

EXTERN_C_END

#endif /* ALCI_ENUM_H */
