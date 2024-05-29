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
#include "MockTest.hh"

namespace {

using namespace Au;

class MockCpuidUtil
    : public MockCpuidBase
    , public ::testing::WithParamInterface<std::tuple<String, VendorInfo>>
{};

INSTANTIATE_TEST_SUITE_P(CupidUtilstTestSuite,
                         MockCpuidUtil,
                         ::testing::ValuesIn(testParametersCpuidUtils));
/**
 * Testcase for CpuidUtils
 * Tests all the CpuidUtils Functions
 * from the mocked data in the simnowdata folder.
 * Takes testParametersCpuidUtils as input
 * Containing the CPU type and the expected results in VendorInfo structure.
 */

TEST_P(MockCpuidUtil, CpuidUtilsTest)
{
    const auto       params          = GetParam();
    const String     cpuType         = std::get<0>(params);
    const VendorInfo expectedResults = std::get<1>(params);
    filename                         = cpuType;
    auto reqRespData                 = Configure();

    std::cout << "Mocking " << cpuType << std::endl;
    EXPECT_EQ(mockCpuidUtils.getMfgInfo(reqRespData[RequestT{ 0, 0, 0, 0 }]),
              (expectedResults.m_mfg));
    EXPECT_EQ(mockCpuidUtils.getFamily(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
              (expectedResults.m_family));
    EXPECT_EQ(mockCpuidUtils.getModel(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
              expectedResults.m_model);
    EXPECT_EQ(
        mockCpuidUtils.getStepping(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
        expectedResults.m_stepping);
}

} // namespace
