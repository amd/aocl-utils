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

#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Cpuid/X86Cpu.hh"

namespace alci = Au;
using Cpu      = Au::X86Cpu;
namespace Au {
using Vendor = EVendor;
#define eIntel Intel
#define eAmd   Amd
#define Other  Other

using Uarch = EUarch;

#define eZen   Zen
#define eZen2  Zen2
#define eZen3  Zen3
#define eZen4  Zen4
#define eZen5  Zen5
#define eOther Unknown

#ifdef ALC_E_FLAG_SSSE3
#undef ALC_E_FLAG_SSSE3
#endif
#define ALC_E_FLAG_SSSE3 ECpuidFlag::sse3

#ifdef ALC_E_FLAG_AES
#undef ALC_E_FLAG_AES
#endif
#define ALC_E_FLAG_AES ECpuidFlag::aes

#ifdef ALC_E_FLAG_AVX
#undef ALC_E_FLAG_AVX
#endif
#define ALC_E_FLAG_AVX ECpuidFlag::avx

#ifdef ALC_E_FLAG_RDRAND
#undef ALC_E_FLAG_RDRAND
#endif
#define ALC_E_FLAG_RDRAND ECpuidFlag::rdrand

#ifdef ALC_E_FLAG_TSC_ADJUST
#undef ALC_E_FLAG_TSC_ADJUST
#endif
#define ALC_E_FLAG_TSC_ADJUST ECpuidFlag::tsc_adjust

#ifdef ALC_E_FLAG_AVX2
#undef ALC_E_FLAG_AVX2
#endif
#define ALC_E_FLAG_AVX2 ECpuidFlag::avx2

#ifdef ALC_E_FLAG_BMI2
#undef ALC_E_FLAG_BMI2
#endif
#define ALC_E_FLAG_BMI2 ECpuidFlag::bmi2

#ifdef ALC_E_FLAG_AVX512F
#undef ALC_E_FLAG_AVX512F
#endif
#define ALC_E_FLAG_AVX512F ECpuidFlag::avx512f

#ifdef ALC_E_FLAG_AVX512DQ
#undef ALC_E_FLAG_AVX512DQ
#endif
#define ALC_E_FLAG_AVX512DQ ECpuidFlag::avx512dq

#ifdef ALC_E_FLAG_RDSEED
#undef ALC_E_FLAG_RDSEED
#endif
#define ALC_E_FLAG_RDSEED ECpuidFlag::rdseed

#ifdef ALC_E_FLAG_ADX
#undef ALC_E_FLAG_ADX
#endif
#define ALC_E_FLAG_ADX ECpuidFlag::adx

#ifdef ALC_E_FLAG_AVX512_IFMA
#undef ALC_E_FLAG_AVX512_IFMA
#endif
#define ALC_E_FLAG_AVX512_IFMA ECpuidFlag::avx512ifma

#ifdef ALC_E_FLAG_AVX512PF
#undef ALC_E_FLAG_AVX512PF
#endif
#define ALC_E_FLAG_AVX512PF ECpuidFlag::avx512pf

#ifdef ALC_E_FLAG_AVX512ER
#undef ALC_E_FLAG_AVX512ER
#endif
#define ALC_E_FLAG_AVX512ER ECpuidFlag::avx512er

#ifdef ALC_E_FLAG_AVX512CD
#undef ALC_E_FLAG_AVX512CD
#endif
#define ALC_E_FLAG_AVX512CD ECpuidFlag::avx512cd

#ifdef ALC_E_FLAG_SHA
#undef ALC_E_FLAG_SHA
#endif
#define ALC_E_FLAG_SHA ECpuidFlag::sha_ni

#ifdef ALC_E_FLAG_SHA_NI
#undef ALC_E_FLAG_SHA_NI
#endif
#define ALC_E_FLAG_SHA_NI ECpuidFlag::sha_ni

#ifdef ALC_E_FLAG_AVX512BW
#undef ALC_E_FLAG_AVX512BW
#endif
#define ALC_E_FLAG_AVX512BW ECpuidFlag::avx512bw

#ifdef ALC_E_FLAG_AVX512VL
#undef ALC_E_FLAG_AVX512VL
#endif
#define ALC_E_FLAG_AVX512VL ECpuidFlag::avx512vl

#ifdef ALC_E_FLAG_AVX512VBMI
#undef ALC_E_FLAG_AVX512VBMI
#endif
#define ALC_E_FLAG_AVX512VBMI ECpuidFlag::avx512vbmi

#ifdef ALC_E_FLAG_AVX512_VPOPCNTDQ
#undef ALC_E_FLAG_AVX512_VPOPCNTDQ
#endif
#define ALC_E_FLAG_AVX512_VPOPCNTDQ ECpuidFlag::avx512_vpopcntdq

#ifdef ALC_E_FLAG_AVXVNNI
#undef ALC_E_FLAG_AVXVNNI
#endif
#define ALC_E_FLAG_AVXVNNI ECpuidFlag::avxvnni

#ifdef ALC_E_FLAG_VAES
#undef ALC_E_FLAG_VAES
#endif
#define ALC_E_FLAG_VAES ECpuidFlag::vaes

#ifdef ALC_E_FLAG_VPCLMULQDQ
#undef ALC_E_FLAG_VPCLMULQDQ
#endif
#define ALC_E_FLAG_VPCLMULQDQ ECpuidFlag::vpclmulqdq

#ifdef ALC_E_FLAG_AVX512_VNNI
#undef ALC_E_FLAG_AVX512_VNNI
#endif
#define ALC_E_FLAG_AVX512_VNNI ECpuidFlag::avx512_vnni

#ifdef ALC_E_FLAG_AVX512_BITALG
#undef ALC_E_FLAG_AVX512_BITALG
#endif
#define ALC_E_FLAG_AVX512_BITALG ECpuidFlag::avx512_bitalg

#ifdef ALC_E_FLAG_AVX512_VBMI2
#undef ALC_E_FLAG_AVX512_VBMI2
#endif
#define ALC_E_FLAG_AVX512_VBMI2 ECpuidFlag::avx512_vbmi2

#ifdef ALC_E_FLAG_MOVDIRI
#undef ALC_E_FLAG_MOVDIRI
#endif
#define ALC_E_FLAG_MOVDIRI ECpuidFlag::movdiri

#ifdef ALC_E_FLAG_MOVDIR64B
#undef ALC_E_FLAG_MOVDIR64B
#endif
#define ALC_E_FLAG_MOVDIR64B ECpuidFlag::movdir64b

#ifdef ALC_E_FLAG_AVX512VPINTERSECT
#undef ALC_E_FLAG_AVX512VPINTERSECT
#endif
#define ALC_E_FLAG_AVX512VPINTERSECT ECpuidFlag::avx512_vpintersect

#ifdef ALC_E_FLAG_X2AVIC
#undef ALC_E_FLAG_X2AVIC
#endif
#define ALC_E_FLAG_X2AVIC ECpuidFlag::x2avic

#ifdef ALC_CPUID_FLAG_MAX
#undef ALC_CPUID_FLAG_MAX
#endif
#define ALC_CPUID_FLAG_MAX ECpuidFlag::Max

} // namespace Au
