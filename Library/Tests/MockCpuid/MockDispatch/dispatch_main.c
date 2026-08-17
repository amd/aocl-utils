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
 *    without specific prior written permission.
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
 * A consumer of the CPUID C API, in the shape a real one has: ask which
 * features are available, pick a kernel, RUN IT.
 *
 * Built twice with this consumer source set, against the real library and
 * against the shim, and run under several AU_CPUID_MOCK_PROFILE settings by
 * the driver script. It includes only public headers and calls only public
 * entry points -- nothing about it knows which library it was linked against.
 *
 * The main and scalar translation units stay at baseline ISA. Only the two
 * vector kernel translation units receive elevated ISA flags in CMake.
 */

#include "Capi/au/cpuid/cpuid.h"
#include "kernels.h"

#include <stdio.h>
#include <stdlib.h>

/* Which kernel the dispatcher picked. Ordered by preference. */
typedef enum kernel_id
{
    KERNEL_AVX512 = 0,
    KERNEL_AVX2,
    KERNEL_SCALAR,
} kernel_id_t;

static const char* const k_kernel_names[] = { "avx512", "avx2", "scalar" };

/* The dispatch decision a consumer makes: keyed purely on the CPUID answer. */
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

/* Print "<kernel> <result> <ok|MISMATCH>" so the driver can assert on the
 * kernel actually taken, not merely on what was reported. Reaching this line
 * at all is the proof: the selected kernel already executed. */
int
main(int argc, char** argv)
{
    float       a[AU_TEST_N];
    float       got;
    float       want;
    kernel_id_t k;
    int         i;

    (void)argc;
    (void)argv;

    for (i = 0; i < AU_TEST_N; i++)
        a[i] = (float)(i + 1);
    want = (float)(AU_TEST_N * (AU_TEST_N + 1) / 2);

    k = select_kernel();

    switch (k) {
        case KERNEL_AVX512:
            got = au_mockdispatch_sum_avx512(a);
            break;
        case KERNEL_AVX2:
            got = au_mockdispatch_sum_avx2(a);
            break;
        default:
            got = au_mockdispatch_sum_scalar(a);
            break;
    }

    printf("%s %.1f %s\n",
           k_kernel_names[k],
           (double)got,
           (got == want) ? "ok" : "MISMATCH");
    return (got == want) ? EXIT_SUCCESS : EXIT_FAILURE;
}
