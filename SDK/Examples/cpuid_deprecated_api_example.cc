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

#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Cpuid/X86Cpu.hh"

#include <iostream>

using namespace Au;
using namespace std;

static void
display_core_info()
{
    X86Cpu cpu{ 0 };
    if (cpu.isAMD()) {
        cout << "AMD CPU detected..." << endl;
    } else if (cpu.isIntel()) {
        cout << "Intel CPU detected..." << endl;
    } else {
        cout << "Unknown CPU detected..." << endl;
    }

    cout << "\nGetting CPU information for core 0" << endl;
    auto vInfo = cpu.getVendorInfo();
    cout << "Vendor   : " << static_cast<Uint32>(vInfo.m_mfg) << endl;
    cout << "Family   : " << static_cast<Uint32>(vInfo.m_family) << endl;
    cout << "Model    : " << vInfo.m_model << endl;
    cout << "Stepping : " << vInfo.m_stepping << endl;
    cout << "Uarch    : " << static_cast<Uint32>(vInfo.m_uarch) << endl;
}

static void
display_isa_info()
{
    X86Cpu cpu{ 0 };

    cout << "\nGetting ISA information for core 0" << endl;
    if (cpu.isX86_64v4()) {
        cout << "CPU supports x86-64v4 ISA" << endl;
    }
    if (cpu.isX86_64v3()) {
        cout << "CPU supports x86-64v3 ISA" << endl;
    }
    if (cpu.isX86_64v2()) {
        cout << "CPU supports x86-64v2 ISA" << endl;
    } else {
        cout << "CPU does not support x86-64v2 or higher ISA" << endl;
    }
    cout << "\nCpu Feature Identification..." << endl;
    // clang-format off
    cout << "TSC_ADJUST : " << (cpu.isAvailable(ECpuidFlag::tsc_adjust) ? "yes" : "no") << endl;
    cout << "AVX        : " << (cpu.isAvailable(ECpuidFlag::avx) ? "yes" : "no") << endl;
    cout << "AVXVNNI    : " << (cpu.isAvailable(ECpuidFlag::avxvnni) ? "yes" : "no") << endl;
    cout << "AVX2       : " << (cpu.isAvailable(ECpuidFlag::avx2) ? "yes" : "no") << endl;
    cout << "AVX512     : " << (cpu.isAvailable(ECpuidFlag::avx512f) ? "yes" : "no") << endl;
    // clang-format on
}

int
main(void)
{
    // Note: Only isAvailable API is deprecated. Use hasFlag API instead.
    display_core_info();

    display_isa_info();

    return 0;
}
