
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

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/enum.h"
#include "CpuidTest.hh"
#include "gtest/gtest.h"
#include <cstdlib>
#include <fstream>

namespace {
using namespace Au;

/**
 * Tests that can run on host, DISABLED by default since tests doesn't add any
 * value as the result is unknown on a physical machine
 * The disabled tests are used by the QemuTest to verify the results of the
 * Cpuid emulation in Qemu.
 */
TEST(CapiX86Cpuid, DISABLED_isAMD)
{
    EXPECT_TRUE(au_cpuid_is_amd(0));
}
TEST(CapiX86Cpuid, DISABLED_hasFlagPositive)
{
    std::vector<String>      flags = readFromFile<String>("FlagsT.txt");
    std::vector<const char*> flagsArray;
    flagsArray.reserve(flags.size() + 1);

    for (size_t i = 0; i < flags.size(); ++i)
        flagsArray.push_back(flags[i].c_str());

    auto result = au_cpuid_has_flag(0, flagsArray.data(), flags.size());
    for (size_t i = 0; i < flags.size(); ++i)
        EXPECT_TRUE(result[i]);
}

TEST(CapiX86Cpuid, DISABLED_hasFlagNegative)
{
    std::vector<String>      flags = readFromFile<String>("FlagsF.txt");
    std::vector<const char*> flagsArray;

    flagsArray.reserve(flags.size() + 1);
    for (size_t i = 0; i < flags.size(); ++i)
        flagsArray.push_back(flags[i].c_str());

    auto result = au_cpuid_has_flag(0, flagsArray.data(), flags.size());
    for (size_t i = 0; i < flags.size(); ++i)
        EXPECT_FALSE(result[i]);
}

TEST(CapiX86Cpuid, hasFlags)
{
    if (AU_ENABLE_ASSERTIONS) {
        std::vector<char*> testFlags = { "" };
        EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 1));
        testFlags = { "Test" };
        EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 1));
        EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 0));
        testFlags = { "avx512f", "Test" };
        EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 2));
    }
}

TEST(CapiX86Cpuid, DISABLED_isUarch)
{
    EUarch uarch = readFromFile<EUarch>("Uarch.txt").front();

    // verify the uarch passed from the qemu testcase.
    if (uarch == EUarch::Zen) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_FALSE(au_cpuid_arch_is_zenplus(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen2(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen3(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen4(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen5(0));
    } else if (uarch == EUarch::ZenPlus) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_TRUE(au_cpuid_arch_is_zenplus(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen2(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen3(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen4(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen5(0));
    } else if (uarch == EUarch::Zen2) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_TRUE(au_cpuid_arch_is_zenplus(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen2(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen3(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen4(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen5(0));
    } else if (uarch == EUarch::Zen3) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_TRUE(au_cpuid_arch_is_zenplus(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen2(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen3(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen4(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen5(0));
    } else if (uarch == EUarch::Zen4) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_TRUE(au_cpuid_arch_is_zenplus(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen2(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen3(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen4(0));
        EXPECT_FALSE(au_cpuid_arch_is_zen5(0));
    } else if (uarch == EUarch::Zen5) {
        EXPECT_TRUE(au_cpuid_arch_is_zen(0));
        EXPECT_TRUE(au_cpuid_arch_is_zenplus(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen2(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen3(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen4(0));
        EXPECT_TRUE(au_cpuid_arch_is_zen5(0));
    }
}

TEST(CapiX86Cpuid, DISABLED_getVendorInfo)
{
    char buf[sizeof(VendorInfo) + 1];
    au_cpuid_get_vendor(0, buf, sizeof(buf));

    String              vInfo(buf);
    String              token;
    std::stringstream   ss(vInfo);
    std::vector<Uint32> vendorInfo = {};
    int                 element    = 0;
    while (getline(ss, token, '\n')) {
        element++;
        vendorInfo.push_back(stoi(token));
        if (element == 5) {
            break;
        }
    }
    auto iter = vendorInfo.begin();

    writeToFile<std::vector<Uint32>::iterator>(
        "VendorInfoC.txt", { iter, iter + 1, iter + 2, iter + 3, iter + 4 });
}
TEST(CapiX86Cpuid, BCTEST)
{
    int core_num = 0;
    if (alci_is_amd(core_num)) {
        printf("AMD CPU detected...\n");
    } else {
        // Intel Apis are not exposed.
        printf("Unknown CPU detected...\n");
    }
    printf("\nGetting ISA information for core 0\n");
    if (alci_arch_is_zen5(core_num)) {
        printf("CPU supports zen5 ISA\n");
    } else if (alci_arch_is_zen4(core_num)) {
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
    printf("checking alcpu apis\n");
    if (alcpu_is_amd()) {
        printf("AMD CPU detected...\n");
    } else {
        // Intel Apis are not exposed.
        printf("Unknown CPU detected...\n");
    }
    printf("\nGetting ISA information for core 0\n");
    if (alcpu_arch_is_zen5()) {
        printf("CPU supports zen5 ISA\n");
    } else if (alcpu_arch_is_zen4()) {
        printf("CPU supports zen4 ISA\n");
    } else if (alcpu_arch_is_zen3()) {
        printf("CPU supports zen3 ISA\n");
    } else if (alcpu_arch_is_zen2()) {
        printf("CPU supports zen2 ISA\n");
    } else if (alcpu_arch_is_zen()) {
        printf("CPU supports zen ISA\n");
    } else {
        printf("CPU does not support zen or higher ISA or this Cpu is not "
               "supported\n");
    }
    printf("\nCpu Feature Identification...\n");
    // clang-format off
        printf("TSC_ADJUST : %s\n", (alcpu_flag_is_available(ALC_E_FLAG_TSC_ADJUST) ? "yes" : "no"));
        printf("AVX        : %s\n", (alcpu_flag_is_available(ALC_E_FLAG_AVX) ? "yes" : "no"));
        printf("AVXVNNI    : %s\n", (alcpu_flag_is_available(ALC_E_FLAG_AVXVNNI) ? "yes" : "no"));
        printf("AVX2       : %s\n", (alcpu_flag_is_available(ALC_E_FLAG_AVX2) ? "yes" : "no"));
        printf("AVX512     : %s\n", (alcpu_flag_is_available(ALC_E_FLAG_AVX512F) ? "yes" : "no"));
}
} // namespace
