
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
#include "CpuidTest.hh"
#include "gtest/gtest.h"
#include <cstdlib>
#include <fstream>

namespace {
using namespace Au;

#if 0
template<>
vector<String>
readFromFile<String>(const String& fileName)
{
    ifstream       file(fileName);
    String         data;
    vector<String> items;

    if (!file.is_open()) {
        cout << "Error opening file" << fileName << endl;
    }

    while (getline(file, data)) {
        auto value = data.substr(0, data.find(' '));
        items.push_back(value);
    }

    file.close();
    return items;
}
#endif
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
#if 0
TEST(CapiX86Cpuid, DISABLED_hasFlagT)
{
    auto flags  = readFromFile<String>("FlagsT.txt");
    auto result = au_cpuid_has_flag(0, flags);
    for (const auto& pair : result)
        EXPECT_TRUE(pair.second);
}

TEST(CapiX86Cpuid, DISABLED_hasFlagF)
{
    auto flags  = readFromFile<String>("FlagsF.txt");
    auto result = au_cpuid_has_flag(0, flags);
    for (const auto& pair : result)
        EXPECT_FALSE(pair.second);
}
#endif
TEST(CapiX86Cpuid, hasFlags)
{
    vector<char*> testFlags = { "" };
    EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 1));
    testFlags = { "Test" };
    EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 1));
    EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 0));
    testFlags = { "avx512f", "Test" };
    EXPECT_ANY_THROW(au_cpuid_has_flag(0, testFlags.data(), 2));
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

    String         vInfo(buf);
    String         token;
    stringstream   ss(vInfo);
    vector<Uint32> vendorInfo = {};

    while (getline(ss, token, '\n')) {
        vendorInfo.push_back(stoi(token));
    }
    auto iter = vendorInfo.begin();

    writeToFile<vector<Uint32>::iterator>(
        "VendorInfoC.txt", { iter, iter + 1, iter + 2, iter + 3, iter + 4 });
}
} // namespace
