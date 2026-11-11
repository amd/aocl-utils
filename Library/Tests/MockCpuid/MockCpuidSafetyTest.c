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
 * The subset property, checked over EVERY flag rather than a chosen few.
 *
 * Built twice from this source -- once against the real library, once against
 * the shim -- and run as a pair by the driver. The real-linked binary prints
 * the host's true flag set; the shim-linked one prints what the shim reports
 * under whatever AU_CPUID_MOCK_PROFILE setting it was given. The driver then
 * asserts that the reported set is a subset of the true host set.
 *
 * This is a stronger check than testing avx512f by hand: a masking bug that
 * happened to raise some unrelated flag (a mis-cleared bit corrupting a
 * neighbour) shows up here and would not show up in a spot check.
 *
 * Output: one "name=0|1" per line, in flag order, so the driver can diff.
 */

#include "Capi/au/cpuid/cpuid.h"

#include <stdio.h>

/* Same list, same order, as AU_CPUID_FLAG_LIST in cpuid_flags.h. Queried by
 * NAME through the public API, so this test needs no internal header and sees
 * exactly what a consumer would see. */
static const char* const k_flag_names[] = {
#define AU_TEST_FLAG_NAME(n) #n,
    AU_CPUID_FLAG_LIST(AU_TEST_FLAG_NAME)
#undef AU_TEST_FLAG_NAME
};

int
main(int argc, char** argv)
{
    const size_t n = sizeof(k_flag_names) / sizeof(k_flag_names[0]);
    size_t       i;
    char         vend[64];

    (void)argc;
    (void)argv;

    /* Identity first, so the driver can also check uarch substitution. */
    au_cpuid_get_vendor(AU_CPUID_CURRENT_CPU, vend, sizeof(vend));
    for (i = 0; vend[i] != '\0'; i++) {
        if (vend[i] == '\n')
            vend[i] = ' ';
    }
    printf("#identity %s\n", vend);
    printf("#level v2=%d v3=%d v4=%d\n",
           au_cpuid_arch_is_x86_64v2(AU_CPUID_CURRENT_CPU) ? 1 : 0,
           au_cpuid_arch_is_x86_64v3(AU_CPUID_CURRENT_CPU) ? 1 : 0,
           au_cpuid_arch_is_x86_64v4(AU_CPUID_CURRENT_CPU) ? 1 : 0);

    for (i = 0; i < n; i++) {
        const char* one[1];
        one[0] = k_flag_names[i];
        printf("%s=%d\n",
               k_flag_names[i],
               au_cpuid_has_flags_all(AU_CPUID_CURRENT_CPU, one, 1) ? 1 : 0);
    }
    return 0;
}
