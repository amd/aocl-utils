/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

/*
 * Profile-masked dispatch, using libaoclutils_mock.
 *
 * This source is an ORDINARY CPUID consumer. It includes only the public
 * header and calls only public entry points -- there is nothing shim-specific
 * in it, which is the point: you link libaoclutils_mock instead of
 * libaoclutils and your existing dispatch code starts taking the fallback.
 *
 * Run it three ways to see the whole story (see SDK/Examples/Readme.md):
 *
 *   ./cpuid_mockcpuid_example_c                                      # host
 *   AU_CPUID_MOCK_PROFILE=Conroe-v1 AU_CPUID_MOCK_STRICT=0 \
 *       ./cpuid_mockcpuid_example_c                                  # fallback
 *
 * The second invocation is the one that matters. The kernel it selects is not
 * merely *reported* as taken -- it is executed, on real silicon, and produces
 * the same answer. That is what the shim guarantees and what no amount of
 * claiming an unsupported CPUID feature can offer: the reported feature set
 * is always a subset of the host's, so every path you can select is a path the
 * machine can run.
 */

/* Links a library (the real one or the shim) -- the shim only exists at link
 * time, so this include is exactly what a normal consumer writes. */
#include "Capi/au/cpuid/cpuid_shared.h"

#include <stdio.h>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

#define AU_EXAMPLE_N 16

/*
 * A dispatcher, in the shape real AOCL consumers have: ask what is available,
 * pick the best kernel, run it.
 */

typedef enum kernel_id
{
    KERNEL_AVX512 = 0,
    KERNEL_AVX2,
    KERNEL_SCALAR,
} kernel_id_t;

static const char* const kernel_names[] = { "AVX-512", "AVX2", "scalar" };

static kernel_id_t
select_kernel(void)
{
    static const char* const avx512_flags[] = {
        "avx512f", "avx512dq", "avx512bw", "avx512vl"
    };
    static const char* const avx2_flags[] = { "avx2", "fma" };

    if (au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, avx512_flags, 4))
        return KERNEL_AVX512;
    if (au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, avx2_flags, 2))
        return KERNEL_AVX2;
    return KERNEL_SCALAR;
}

/* Three ways to sum 16 floats. The arithmetic is exact in binary32 on every
 * path, so all three must produce the identical answer -- which is how you can
 * see that the fallback is correct, not merely reachable. */

static float
sum_scalar(const float* a)
{
    float s = 0.0f;
    int   i;

    for (i = 0; i < AU_EXAMPLE_N; i++)
        s += a[i];
    return s;
}

#if defined(__x86_64__) || defined(_M_X64)

/* VEX-encoded: these instructions fault on a CPU without AVX2. */
__attribute__((target("avx2,fma"))) static float
sum_avx2(const float* a)
{
    __m256 v0 = _mm256_loadu_ps(a);
    __m256 v1 = _mm256_loadu_ps(a + 8);
    __m256 s  = _mm256_add_ps(v0, v1);
    __m128 lo = _mm256_castps256_ps128(s);
    __m128 hi = _mm256_extractf128_ps(s, 1);
    __m128 q  = _mm_add_ps(lo, hi);
    float  out[4];

    _mm_storeu_ps(out, q);
    return out[0] + out[1] + out[2] + out[3];
}

/* EVEX-encoded: faults on anything without AVX-512F. */
__attribute__((target("avx512f"))) static float
sum_avx512(const float* a)
{
    __m512 v = _mm512_loadu_ps(a);
    return _mm512_reduce_add_ps(v);
}

#else

static float
sum_avx2(const float* a)
{
    return sum_scalar(a);
}
static float
sum_avx512(const float* a)
{
    return sum_scalar(a);
}

#endif

static void
display_reported_features(void)
{
    static const char* const interesting[] = {
        "avx", "avx2", "fma", "avx512f", "avx512dq", "avx512bw", "avx512vl"
    };
    const int n = (int)(sizeof(interesting) / sizeof(interesting[0]));
    char      vendor[16];
    int       i;

    printf("CPU as reported to this process\n");
    printf("-------------------------------\n");

    au_cpuid_get_vendor(AU_CPUID_CURRENT_CPU, vendor, sizeof(vendor));
    printf("  vendor/family/model/stepping/uarch : ");
    for (i = 0; vendor[i] != '\0'; i++)
        putchar(vendor[i] == '\n' ? ' ' : vendor[i]);
    putchar('\n');

    printf("  x86-64 level                       : ");
    if (au_cpuid_arch_is_x86_64v4(AU_CPUID_CURRENT_CPU))
        printf("v4\n");
    else if (au_cpuid_arch_is_x86_64v3(AU_CPUID_CURRENT_CPU))
        printf("v3\n");
    else if (au_cpuid_arch_is_x86_64v2(AU_CPUID_CURRENT_CPU))
        printf("v2\n");
    else
        printf("below v2\n");

    printf("  features                           :");
    for (i = 0; i < n; i++) {
        const char* one[1];
        one[0] = interesting[i];
        if (au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, one, 1))
            printf(" %s", interesting[i]);
    }
    printf("\n\n");
}

int
main(void)
{
    float       a[AU_EXAMPLE_N];
    float       expected;
    float       result;
    kernel_id_t k;
    int         i;

    for (i = 0; i < AU_EXAMPLE_N; i++)
        a[i] = (float)(i + 1);
    /* 1 + 2 + ... + 16 */
    expected = (float)(AU_EXAMPLE_N * (AU_EXAMPLE_N + 1) / 2);

    display_reported_features();

    k = select_kernel();

    printf("Dispatch\n");
    printf("--------\n");
    printf("  selected kernel                    : %s\n", kernel_names[k]);

    switch (k) {
        case KERNEL_AVX512:
            result = sum_avx512(a);
            break;
        case KERNEL_AVX2:
            result = sum_avx2(a);
            break;
        default:
            result = sum_scalar(a);
            break;
    }

    /* Printing this line proves the kernel ran: had the shim reported a
     * feature the CPU lacks, the process would have died with SIGILL inside
     * the call above rather than reaching here. */
    printf("  kernel executed, result            : %.1f (expected %.1f) %s\n",
           (double)result,
           (double)expected,
           (result == expected) ? "OK" : "MISMATCH");

    if (result != expected) {
        printf("\nThe selected kernel produced the wrong answer.\n");
        return 1;
    }

    printf("\nRe-run with AU_CPUID_MOCK_PROFILE=Conroe-v1 and\n"
           "AU_CPUID_MOCK_STRICT=0 to force the fallback.\n"
           "Requires linking libaoclutils_mock; against the real library\n"
           "these variables do nothing.\n");
    return 0;
}
