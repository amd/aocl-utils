/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#include <Python.h>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <vector>

#include "Au/Types.hh"
#include "CpuidTest.hh"
#include "QemuTest.hh"

namespace {
using namespace Au;
class QemuTest
    : public QemuTestBase
    , public ::testing::WithParamInterface<
          std::tuple<String, std::vector<bool>, EUarch>>
{
  protected:
    /**
     * @brief Run all the tests in testNames on the emulated cpu.
     * @param cpuType The type of the cpu to emulate
     * @param testNames The names of the tests to run
     *
     * @return std::vector<bool> The results of the tests
     */
    static std::vector<bool> testAll(const std::string&              cpuType,
                                     const std::vector<std::string>& testNames)
    {
        std::vector<bool> results;
        for (const auto& testName : testNames) {
            bool result = callQemuEmulator(cpuType.c_str(), testName.c_str());
            results.push_back(result);
        }
        return results;
    }

    /**
     * @brief Check if the emulated cpu has all the flags in the flags vector
     * @param cpuType The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    static bool hasFlagT(const std::string& cpuType)
    {
        String simnowdataPath = PROJECT_SOURCE_DIR;
        simnowdataPath += "/Library/Tests/Cpuid/Mock/simnowdata/";
        std::filesystem::copy(simnowdataPath + cpuType + "/FlagsT.txt",
                              "FlagsT.txt");
        return callQemuEmulator(cpuType.c_str(), "hasFlagT");
    }

    /**
     * @brief Check if the emulated cpu does not have any of the flags in the
     * flags vector
     * @param cpuType The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    static bool hasFlagF(const std::string& cpuType)
    {
        String simnowdataPath = PROJECT_SOURCE_DIR;
        simnowdataPath += "/Library/Tests/Cpuid/Mock/simnowdata/";
        std::filesystem::copy(simnowdataPath + cpuType + "/FlagsF.txt",
                              "FlagsF.txt");

        return callQemuEmulator(cpuType.c_str(), "hasFlagF");
    }

    /**
     * @brief Verifies the Uarch APIs for the emulated CPU
     * @param cpuType The type of the cpu to emulate
     * @param Uarch The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    static bool TestUarch(const String& cpuType, const EUarch Uarch)
    {
        writeToFile<EUarch>("Uarch.txt", { Uarch });
        return callQemuEmulator(cpuType.c_str(), "Uarch");
    }
};

INSTANTIATE_TEST_SUITE_P(QemuTestSuite,
                         QemuTest,
                         ::testing::ValuesIn(testParametersX86Cpu));

TEST_P(QemuTest, CpuTypeTest)
{
    const auto params          = GetParam();
    const auto cpuType         = std::get<0>(params);
    const auto expectedResults = std::get<1>(params);
    const auto uarch           = std::get<2>(params);

    std::cout << "Emulating " << cpuType << std::endl;
    const std::vector<std::string> testNames = {
        "X86Cpuid.DISABLED_isAMD",
        "X86Cpuid.DISABLED_isIntel",
        "X86Cpuid.DISABLED_isX86_64v2",
        "X86Cpuid.DISABLED_isX86_64v3",
        "X86Cpuid.DISABLED_isX86_64v4"
    };

    auto results = testAll(cpuType, testNames);
    results.push_back(hasFlagT(cpuType));
    results.push_back(hasFlagF(cpuType));
    results.push_back(TestUarch(cpuType, uarch));
    EXPECT_EQ(results, expectedResults);
    // Read the Uarch test result from the file
    // and compare it with the Uarch value of the emulated CPU.
    auto uarchResult = readFromFile<EUarch>("UarchResult.txt");
    EXPECT_EQ(uarchResult[0], uarch);
}

class QemuTestVendorInfo
    : public QemuTestBase
    , public ::testing::WithParamInterface<tuple<string, VendorInfo>>
{
  protected:
    /**
     * @brief Runs the getVendorInfo test on the emulated cpu.
     * @param cpuType The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    bool TestVendorInfo(const string& cpuType)
    {
        return callQemuEmulator(cpuType.c_str(), "DISABLED_getVendorInfo");
    }
};

INSTANTIATE_TEST_SUITE_P(QemuTestSuite,
                         QemuTestVendorInfo,
                         ::testing::ValuesIn(testParametersVendorInfo));
TEST_P(QemuTestVendorInfo, CpuTypeTest)
{
    const auto     params          = GetParam();
    const auto     cpuType         = std::get<0>(params);
    const auto     expectedResults = std::get<1>(params);
    vector<Uint32> vendorInfo      = {
        enumToValue<EVendor, Uint32>(expectedResults.m_mfg),
        enumToValue<EFamily, Uint32>(expectedResults.m_family),
        expectedResults.m_model,
        expectedResults.m_stepping,
        enumToValue<EUarch, Uint32>(expectedResults.m_uarch)
    };
    EXPECT_TRUE(TestVendorInfo(cpuType));
    vector<Uint32> vendorInfoResult = readFromFile<Uint32>("VendorInfo.txt");
    EXPECT_EQ(vendorInfo, vendorInfoResult);
}
} // namespace
