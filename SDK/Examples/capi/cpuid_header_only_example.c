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

/* Header-only CPUID API (no library link). Contrast with cpuid_example.c. */

#include "Capi/au/cpuid/cpuid_inline.h"

#include <stdio.h>
#include <string.h>

int
main(void)
{
    const int core_num = 0;

    if (au_cpuid_is_amd(core_num)) {
        printf("AMD CPU detected (header-only build).\n");
    } else {
        printf("Non-AMD / unknown CPU detected (header-only build).\n");
    }

    char buf[16];
    memset(buf, 0, sizeof(buf));
    au_cpuid_get_vendor(core_num, buf, sizeof(buf));
    printf("Raw vendor block:\n%s", buf);

    const char* const flags[] = { "sse2", "avx2" };
    const int         nflags  = (int)(sizeof(flags) / sizeof(flags[0]));
    printf("has sse2+avx2: %s\n",
           au_cpuid_has_flags_all(core_num, flags, nflags) ? "yes" : "no");

    return 0;
}
