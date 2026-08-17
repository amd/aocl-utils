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

#include "Capi/au/cpuid/mockcpuid.h"

#include <stdio.h>

static int
has_flag(const char* name)
{
    const char* flags[] = { name };
    return au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, flags, 1) ? 1 : 0;
}

static void
print_state(const char* label)
{
    printf("%s avx2=%d avx512f=%d zen2=%d zen3=%d amd=%d intel=%d\n",
           label,
           has_flag("avx2"),
           has_flag("avx512f"),
           au_cpuid_arch_is_zen2(AU_CPUID_CURRENT_CPU) ? 1 : 0,
           au_cpuid_arch_is_zen3(AU_CPUID_CURRENT_CPU) ? 1 : 0,
           au_cpuid_is_amd(AU_CPUID_CURRENT_CPU) ? 1 : 0,
           au_cpuid_is_amd(AU_CPUID_CURRENT_CPU) ? 0 : 1);
}

int
main(void)
{
    int              host_avx2;
    int              host_avx512f;
    int              host_zen3;
    int              host_amd;
    au_mock_status_t profile_status;
    au_mock_status_t status;

    status = au_mock_reset(AU_CPUID_MOCK_ALL_CPUS);
    if (status != AU_MOCK_OK) {
        fprintf(stderr, "reset before test returned %d\n", (int)status);
        return 1;
    }

    host_avx2    = has_flag("avx2");
    host_avx512f = has_flag("avx512f");
    host_zen3    = au_cpuid_arch_is_zen3(AU_CPUID_CURRENT_CPU) ? 1 : 0;
    host_amd     = au_cpuid_is_amd(AU_CPUID_CURRENT_CPU) ? 1 : 0;
    print_state("host");

    /* A reserved specific-core selector must be a no-op. */
    status = au_mock_apply_profile((au_cpu_num_t)0, "Conroe-v1");
    if (status != AU_MOCK_ERR_PERCPU_UNSUPPORTED
        || has_flag("avx2") != host_avx2
        || has_flag("avx512f") != host_avx512f) {
        fprintf(stderr, "specific-core apply was not a no-op\n");
        return 1;
    }
    printf("sentinel_apply=%d\n", (int)status);

    status = au_mock_reset((au_cpu_num_t)0);
    if (status != AU_MOCK_ERR_PERCPU_UNSUPPORTED
        || has_flag("avx2") != host_avx2) {
        fprintf(stderr, "specific-core reset was not a no-op\n");
        return 1;
    }
    printf("sentinel_reset=%d\n", (int)status);

    /* Rome is a real generated profile. Its captured feature surplus is
     * ordinary downward narrowing and must not be an error under either strict
     * setting. */
    status = au_mock_apply_profile(AU_CPUID_MOCK_ALL_CPUS, "rome");
    if (status != AU_MOCK_OK) {
        fprintf(stderr,
                "generated rome profile returned unexpected status %d\n",
                (int)status);
        return 1;
    }
    profile_status = status;
    print_state("rome");

    if (host_avx512f && has_flag("avx512f")) {
        fprintf(stderr, "rome retained a host AVX-512 feature\n");
        return 1;
    }
    if (host_amd && host_zen3
        && (!au_cpuid_arch_is_zen2(AU_CPUID_CURRENT_CPU)
            || au_cpuid_arch_is_zen3(AU_CPUID_CURRENT_CPU))) {
        fprintf(stderr, "rome did not present a Zen2 identity\n");
        return 1;
    }

    status = au_mock_reset(AU_CPUID_MOCK_ALL_CPUS);
    if (status != AU_MOCK_OK || has_flag("avx2") != host_avx2
        || has_flag("avx512f") != host_avx512f
        || (au_cpuid_arch_is_zen3(AU_CPUID_CURRENT_CPU) ? 1 : 0) != host_zen3) {
        fprintf(stderr, "reset did not restore host truth\n");
        return 1;
    }
    print_state("reset");
    printf("profile_status=%d reset_status=%d\n",
           (int)profile_status,
           (int)status);
    return 0;
}
