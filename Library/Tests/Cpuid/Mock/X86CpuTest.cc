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

#include "Au/Cpuid/X86Cpu.hh"
#include "../CpuidTest.hh"
#include "MockTest.hh"

namespace {

using namespace Au;
class MockX86Cpu
    : public MockCpuidBase
    , public ::testing::WithParamInterface<
          std::tuple<String, std::vector<bool>, EUarch>>
{
  protected:
    void SetUp() override
    {
        // The number of times __raw_cpuid expected to in the code flow.
        auto callCount = 12;
        EXPECT_CALL(mockCpuidUtils, __raw_cpuid(testing::_)).Times(callCount);
    }
};

INSTANTIATE_TEST_SUITE_P(MockX86CpuTestSuite,
                         MockX86Cpu,
                         ::testing::ValuesIn(testParametersX86Cpu));
/**
 * Testcase for X86Cpu
 * Tests all the X86Cpu Functions
 * from the mocked data in the simnowdata folder.
 * Takes testParametersX86Cpu as input containing the CPU type and the expected
 * results vector.
 */
TEST_P(MockX86Cpu, MockX86CpuTest)
{

    const auto        params          = GetParam();
    const auto        cpuType         = get<0>(params);
    const auto        expectedResults = get<1>(params);
    const auto        uarch           = get<2>(params);
    auto              resultT         = true;
    auto              resultF         = false;
    std::vector<bool> results;

    filename           = cpuType;
    auto   reqRespData = Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    std::cout << "Mocking " << cpuType << std::endl;
    results.push_back(cpu.isAMD());
    results.push_back(cpu.isIntel());
    results.push_back(cpu.isX86_64v2());
    results.push_back(cpu.isX86_64v3());
    results.push_back(cpu.isX86_64v4());

    // Has flag tests
    String            srcDir     = PROJECT_SOURCE_DIR;
    String            simnowData = "/Library/Tests/Cpuid/Mock/simnowdata/";
    String            absPath = srcDir + simnowData + cpuType + "/FlagsT.txt";
    auto              flags   = readFromFile<String>(absPath);
    String            token;
    std::stringstream ss;
    ss << flags;
    while (std::getline(ss, token, ':')) {
        auto flag       = stoi(token);
        auto cpuid_flag = valueToEnum<ECpuidFlag, Uint64>(flag);
        resultT         = resultT & (cpu.hasFlag(cpuid_flag));
    }
    results.push_back(resultT);

    absPath = srcDir + simnowData + cpuType + "/FlagsF.txt";
    flags   = readFromFile<String>(absPath);
    ss.clear();
    ss << flags;

    while (std::getline(ss, token, ':')) {
        auto flag       = stoi(token);
        auto cpuid_flag = valueToEnum<ECpuidFlag, Uint64>(flag);
        resultF         = resultF | (cpu.hasFlag(cpuid_flag));
    }
    results.push_back(!resultF);

    std::cout << "Checking Uarch" << std::endl;
    results.push_back(cpu.isUarch(uarch));
    EXPECT_EQ(results, expectedResults);
    EXPECT_EQ(cpu.getUarch(), uarch);
}
} // namespace
