/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#define ALC_E_FLAG_SSSE3             1  // ECpuidFlag::sse3
#define ALC_E_FLAG_AES               24 // ECpuidFlag::aes
#define ALC_E_FLAG_AVX               26 // ECpuidFlag::avx
#define ALC_E_FLAG_RDRAND            29 // ECpuidFlag::rdrand
#define ALC_E_FLAG_TSC_ADJUST        63 // ECpuidFlag::tsc_adjust
#define ALC_E_FLAG_AVX2              66 // ECpuidFlag::avx2
#define ALC_E_FLAG_BMI2              68 // ECpuidFlag::bmi2
#define ALC_E_FLAG_TSX               71 // EFlag::rtm // its pointing to ALC_E_FLAG_RTM
#define ALC_E_FLAG_AVX512F           73  // ECpuidFlag::avx512f
#define ALC_E_FLAG_AVX512DQ          74  // ECpuidFlag::avx512dq
#define ALC_E_FLAG_RDSEED            75  // ECpuidFlag::rdseed
#define ALC_E_FLAG_ADX               76  // ECpuidFlag::adx
#define ALC_E_FLAG_AVX512_IFMA       78  // ECpuidFlag::avx512ifma
#define ALC_E_FLAG_AVX512PF          82  // ECpuidFlag::avx512pf
#define ALC_E_FLAG_AVX512ER          83  // ECpuidFlag::avx512er
#define ALC_E_FLAG_AVX512CD          84  // ECpuidFlag::avx512cd
#define ALC_E_FLAG_SHA               85  // ECpuidFlag::sha_ni
#define ALC_E_FLAG_SHA_NI            85  // ECpuidFlag::sha_ni
#define ALC_E_FLAG_AVX512BW          86  // ECpuidFlag::avx512bw
#define ALC_E_FLAG_AVX512VL          87  // ECpuidFlag::avx512vl
#define ALC_E_FLAG_AVX512VBMI        88  // ECpuidFlag::avx512vbmi
#define ALC_E_FLAG_AVX512_VPOPCNTDQ  92  // ECpuidFlag::avx512_vpopcntdq
#define ALC_E_FLAG_AVXVNNI           98  // ECpuidFlag::avxvnni
#define ALC_E_FLAG_VAES              155 // ECpuidFlag::vaes
#define ALC_E_FLAG_VPCLMULQDQ        156 // ECpuidFlag::vpclmulqdq
#define ALC_E_FLAG_AVX512_VNNI       157 // ECpuidFlag::avx512_vnni
#define ALC_E_FLAG_AVX512_BITALG     158 // ECpuidFlag::avx512_bitalg
#define ALC_E_FLAG_AVX512_VBMI2      159 // ECpuidFlag::avx512_vbmi2
#define ALC_E_FLAG_MOVDIRI           160 // ECpuidFlag::movdiri
#define ALC_E_FLAG_MOVDIR64B         161 // ECpuidFlag::movdir64b
#define ALC_E_FLAG_AVX512VPINTERSECT 162 // ECpuidFlag::avx512_vpintersect
#define ALC_E_FLAG_X2AVIC            163 // ECpuidFlag::x2avic
#define ALC_CPUID_FLAG_MAX           164 // ECpuidFlag::Max
