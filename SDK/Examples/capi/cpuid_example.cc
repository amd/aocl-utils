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

#include "Au/Cpuid/X86Cpu.hh"
#include "Capi/au/cpuid/cpuid.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace Au;
static void
display_core_info()
{
    int  core_num = 0;
    int  size     = sizeof(VendorInfo) * 2;
    char buf[size];

    if (au_cpuid_is_amd(core_num)) {
        cout << "AMD CPU detected..." << endl;
    } else {
        // Intel Apis are not exposed.
        cout << "Unknown CPU detected..." << endl;
    }

    cout << "\nGetting CPU information for core 0" << endl;
    au_cpuid_get_vendor(core_num, buf, size);
    String         vInfo(buf);
    String         token;
    stringstream   ss(vInfo);
    vector<String> vendorInfo = {};
    while (getline(ss, token, '\n')) {
        vendorInfo.push_back(token);
    }
    cout << "Vendor   : " << vendorInfo.at(0) << endl;
    cout << "Family   : " << vendorInfo.at(1) << endl;
    cout << "Model    : " << vendorInfo.at(2) << endl;
    cout << "Stepping : " << vendorInfo.at(3) << endl;
    cout << "Uarch    : " << vendorInfo.at(4) << endl;
}

static void
display_isa_info()
{
    int core_num = 0;
    cout << "\nGetting ISA information for core 0" << endl;
    if (au_cpuid_arch_is_zen5(core_num)) {
        cout << "CPU supports zen5 ISA" << endl;
    } else if (au_cpuid_arch_is_zen4(core_num)) {
        cout << "CPU supports zen4 ISA" << endl;
    } else if (au_cpuid_arch_is_zen3(core_num)) {
        cout << "CPU supports zen3 ISA" << endl;
    } else if (au_cpuid_arch_is_zen2(core_num)) {
        cout << "CPU supports zen2 ISA" << endl;
    } else if (au_cpuid_arch_is_zenplus(core_num)) {
        cout << "CPU supports zenplus ISA" << endl;
    } else if (au_cpuid_arch_is_zen(core_num)) {
        cout << "CPU supports zen ISA" << endl;
    } else {
        cout << "CPU does not support zen or higher ISA or this Cpu is not "
                "supported"
             << endl;
    }

    cout << "\nCpu Feature Identification..." << endl;
    // clang-format off
    char* flags_array[] = {"tsc_adjust", "avx", "avxvnni", "avx2", "avx512f"};
    auto result = au_cpuid_has_flag(core_num, flags_array, 5);
    cout << "TSC_ADJUST : " << (result[0] ? "yes" : "no") << endl;
    cout << "AVX        : " << (result[1] ? "yes" : "no") << endl;
    cout << "AVXVNNI    : " << (result[2] ? "yes" : "no") << endl;
    cout << "AVX2       : " << (result[3] ? "yes" : "no") << endl;
    cout << "AVX512     : " << (result[4] ? "yes" : "no") << endl;
    // clang-format on
}

int
main(void)
{
    display_core_info();

    display_isa_info();

    return 0;
}
