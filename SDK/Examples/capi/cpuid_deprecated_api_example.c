/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/enum.h"

#include <stdio.h>
#include <string.h>

static void
display_core_info()
{
    int   core_num = 0;
    int   size     = 16;
    char  buf[size];
    char* token = NULL;
    int   index = 0;

    memset(buf, 0, size * sizeof(char));
    const char* vendorInfo[13] = { "Vendor   : ",
                                   "Family   : ",
                                   "Model    : ",
                                   "Stepping : ",
                                   "Uarch    : " };

    if (alci_is_amd(core_num)) {
        printf("AMD CPU detected...\n");
    } else {
        // Intel Apis are not exposed.
        printf("Unknown CPU detected...\n");
    }

    printf("\nGetting CPU information for core 0\n");
    alci_cpu_get_vendor(core_num, buf, size);

    token = strtok(buf, "\r\n");
    while (token != NULL) {
        printf("%s", vendorInfo[index++]);
        printf("%s\n", token);
        token = strtok(NULL, "\r\n");
    }
}

static void
display_isa_info()
{
    int core_num = 0;
    printf("\nGetting ISA information for core 0\n");
    if (alci_arch_is_zen4(core_num)) {
        printf("CPU supports zen4 ISA\n");
    } else if (alci_arch_is_zen3(core_num)) {
        printf("CPU supports zen3 ISA\n");
    } else if (alci_arch_is_zen2(core_num)) {
        printf("CPU supports zen2 ISA\n");
    } else if (alci_arch_is_zen(core_num)) {
        printf("CPU supports zen ISA\n");
    } else {
        printf("CPU does not support zen or higher ISA or this Cpu is not "
               "supported\n");
    }
    printf("\nCpu Feature Identification...\n");
    // clang-format off
        printf("TSC_ADJUST : %s\n", (alci_cpu_has_flag(core_num, ALC_E_FLAG_TSC_ADJUST) ? "yes" : "no"));
        printf("AVX        : %s\n", (alci_cpu_has_flag(core_num, ALC_E_FLAG_AVX) ? "yes" : "no"));
        printf("AVXVNNI    : %s\n", (alci_cpu_has_flag(core_num, ALC_E_FLAG_AVXVNNI) ? "yes" : "no"));
        printf("AVX2       : %s\n", (alci_cpu_has_flag(core_num, ALC_E_FLAG_AVX2) ? "yes" : "no"));
        printf("AVX512     : %s\n", (alci_cpu_has_flag(core_num, ALC_E_FLAG_AVX512F) ? "yes" : "no"));
    // clang-format on
}

int
main(void)
{
    display_core_info();

    display_isa_info();

    return 0;
}
