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

/* Exercise the programmatic strict policy path. Venice is the highest-uarch
 * profile in the table, so it is an upward identity request on current hosts.
 */

#include "Capi/au/cpuid/mockcpuid.h"

#include <stdio.h>

static const char* const k_flag_names[] = {
#define AU_TEST_FLAG_NAME(name) #name,
    AU_CPUID_FLAG_LIST(AU_TEST_FLAG_NAME)
#undef AU_TEST_FLAG_NAME
};

static int
has_flag(const char* name)
{
    const char* flags[] = { name };
    return au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, flags, 1) ? 1 : 0;
}

int
main(void)
{
    const size_t flag_count = sizeof(k_flag_names) / sizeof(k_flag_names[0]);
    bool         host_flags[sizeof(k_flag_names) / sizeof(k_flag_names[0])];
    const char*  profile;
    au_mock_status_t status;
    int              host_amd;
    size_t           i;
    unsigned         host_count = 0;
    unsigned         mock_count = 0;

    profile  = "Venice-v1";
    host_amd = au_cpuid_is_amd(AU_CPUID_CURRENT_CPU) ? 1 : 0;
    for (i = 0; i < flag_count; i++) {
        host_flags[i] = has_flag(k_flag_names[i]) != 0;
        host_count += host_flags[i] ? 1u : 0u;
    }

    /* With strict at its default (or AU_CPUID_MOCK_STRICT=1), this call aborts
     * before returning. With strict=0 it returns IDENTITY_UPWARD and applies
     * the safe subset. */
    status = au_mock_apply_profile(AU_CPUID_MOCK_ALL_CPUS, profile);
    if (status != AU_MOCK_ERR_IDENTITY_UPWARD) {
        fprintf(stderr,
                "programmatic profile '%s' returned unexpected status %d\n",
                profile,
                (int)status);
        return 1;
    }

    for (i = 0; i < flag_count; i++) {
        const int reported = has_flag(k_flag_names[i]);

        if (reported && !host_flags[i]) {
            fprintf(stderr,
                    "programmatic profile reported host-absent '%s'\n",
                    k_flag_names[i]);
            return 1;
        }
        mock_count += reported ? 1u : 0u;
    }

    if (host_amd) {
        status = au_mock_reset(AU_CPUID_MOCK_ALL_CPUS);
        if (status != AU_MOCK_OK) {
            fprintf(stderr, "reset before unclassified probe failed\n");
            return 1;
        }
        status = au_mock_apply_profile(AU_CPUID_MOCK_ALL_CPUS, "phenom-v1");
        if (status != AU_MOCK_ERR_IDENTITY_UNCLASSIFIED) {
            fprintf(stderr,
                    "pre-Zen profile returned unexpected status %d\n",
                    (int)status);
            return 1;
        }
        printf("unclassified_status=%d\n", (int)status);
    } else {
        printf("unclassified_status=SKIP\n");
    }

    printf("continued profile=%s status=%d host_features=%u "
           "mocked_features=%u\n",
           profile,
           (int)status,
           host_count,
           mock_count);
    return 0;
}
