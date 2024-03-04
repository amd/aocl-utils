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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Au/Cpuid/X86Cpu.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace Au;

/**
 * Test parameters for X86Cpu Mock Test
<<<<<<< Updated upstream
 * 1. Name of the CPU model to be mocked using simnowdata
 * 2. Vector contains the following parameters:
=======
 * Vector containing the following parameters:
 * 1. Name of the CPU model to be mocked using simnowdata
 * 2. Vector containing the following parameters:
>>>>>>> Stashed changes
 *    1. Name of the CPU model to be mocked using simnowdata
 *    2. Vector of boolean values that marks the expected results of
 *       {is_AMD, is_Intel, is_X86_64v2, is_X86_64v3, is_X86_64v4}  APIs
 *     and true for hasFlags(T/F) and isUarch api tests
 * 3. Expected architecture of the CPU
 */
// clang-format off
auto isAmd=true, isIntel=true, isX86_64v2=true, isX86_64v3=true, isX86_64v4=true, hasFlagsT=true,hasFlagsF=true, isUarch=true;
const vector<tuple<String, vector<bool>, EUarch>> testParametersX86Cpu = {
    { "EPYC-Genoa-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen4 },
    { "EPYC-Milan-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen3 },
    { "EPYC-Milan-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen3 },
    { "EPYC-Rome-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v3", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v4", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v3", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v4", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "Opteron_G1-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Opteron_G2-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Opteron_G3-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Opteron_G4-v1", { isAmd, !isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Opteron_G5-v1", { isAmd, !isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "phenom-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Broadwell-v1", { !isAmd, isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Denverton-v1", { !isAmd, isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT , hasFlagsF, isUarch }, EUarch::Unknown },
    { "Conroe-v1", { !isAmd, isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown },
    { "Skylake-Server-v1", { !isAmd, isIntel, isX86_64v2, isX86_64v3, isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Unknown }
};
// clang-format on
/**
 * Test parameters for CpuidUtils Mock Test
 * Vector contains the following parameters:
 * 1. Name of the CPU model to be mocked using simnowdata
 * 2. VendorInfo structure containing the expected results.
 */
// clang-format off
const vector<tuple<String, VendorInfo>> testParametersCpuidUtils = {
    { "EPYC-Genoa-v1", { VendorInfo{ EVendor::Amd, EFamily::Zen4, 0x11, 0 } } },
    { "EPYC-Milan-v1", { VendorInfo{ EVendor::Amd, EFamily::Zen4, 0x1, 1 } } },
    { "EPYC-Milan-v2", { VendorInfo{ EVendor::Amd, EFamily::Zen4, 0x1, 0x1 } } },
    { "EPYC-Rome-v1",  { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x31, 0x0 } } },
    { "EPYC-Rome-v2",  { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x31, 0x0 } } },
    { "EPYC-Rome-v3",  { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x31, 0x0 } } },
    { "EPYC-Rome-v4",  { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x31, 0x0 } } },
    { "EPYC-v1", { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x1, 0x2 } } },
    { "EPYC-v2", { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x1, 0x2 } } },
    { "EPYC-v3", { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x1, 0x2 } } },
    { "EPYC-v4", { VendorInfo{ EVendor::Amd, EFamily::Zen2, 0x1, 0x2 } } },
    { "Opteron_G1-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x6, 0x1 } } },
    { "Opteron_G2-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x6, 0x1 } } },
    { "Opteron_G3-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x2, 0x3 } } },
    { "Opteron_G4-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x1, 0x2 } } },
    { "Opteron_G5-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x2, 0x0 } } },
    { "phenom-v1", { VendorInfo{ EVendor::Amd, EFamily::Unknown, 0x2, 0x3 } } },
    { "Broadwell-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0X3D, 0x2 } } },
    { "Denverton-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x5F, 0x1 } } },
    { "Conroe-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0xF, 0x3 } } },
    { "Skylake-Server-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x55, 0x4 } }
 },
    // clang-format on
};

/**
 *  The MockCpuidUtils class is used to mock the CpuidUtils class
 */
class MockCpuidUtils : public CpuidUtils
{
  public:
    MockCpuidUtils()
        : CpuidUtils()
    {
    }
    MOCK_METHOD(ResponseT, __raw_cpuid, (RequestT & req), (override)){};
};

/**
 * The Base class for both X86Cpu and CpuidUtils Mock tests
 * It contains the common functions and data for both the tests
 */
class MockCpuidBase : public testing::Test
{
  private:
    /**
     * @brief parseCSV function is used to parse the CSV file and populate the
     * map with the request and response data
     * The format of the file is assumed to be:
     * {1,0,0,0}:{329300,2048,4294586883,126614525}
     * where the first{} represents the request and the second{} represents the
     * response
     * @param filename is the name of the file to be parsed
     *
     * @return map<RequestT, ResponseT> is the map containing the request and
     * response data
     */
    map<RequestT, ResponseT> parseCSV(const String& filename)
    {
        map<RequestT, ResponseT> data;

        cout << "Processing" << filename << std::endl;
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return data; // Return an empty vector if the file cannot be opened
        }

        String line;
        while (getline(file, line)) {
            istringstream lineStream(line);
            String        requestStr, respStr;

            // Assuming the CSV structure is
            // {1,0,0,0}:{329300,2048,4294586883,126614525}
            getline(lineStream, requestStr, ':');
            getline(lineStream, respStr, ':');
            // Extract values between curly braces and populate structures
            ResponseT request{}; // The request needs to be updated, hence not
                                 // using RequestT here.
            ResponseT response{};

            sscanf(requestStr.c_str(),
                   "{%x,%x,%x,%x}",
                   &request.eax,
                   &request.ebx,
                   &request.ecx,
                   &request.edx);
            sscanf(respStr.c_str(),
                   "{%x,%x,%x,%x}",
                   &response.eax,
                   &response.ebx,
                   &response.ecx,
                   &response.edx);

            data[request] = response;
        }

        file.close();
        return data;
    }

  protected:
    /**
     * @brief Configure  Mocks the __raw_cpuid function by specifying the
     * expected request and response data by parsing the CSV file corresponding
     * to the CPU.
     *
     */
    map<RequestT, ResponseT> Configure()
    {
        String projectDir   = PROJECT_SOURCE_DIR;
        String testDir      = "/Library/Tests/Cpuid/Mock/simnowdata/";
        String dataFilename = projectDir + testDir + filename + "/" + filename;

        map<RequestT, ResponseT> csvData = parseCSV(dataFilename);
        for (const auto& entry : csvData) {
            ON_CALL(mockCpuidUtils, __raw_cpuid(entry.first))
                .WillByDefault(testing::Return(entry.second));
        }
        return csvData;
    }

  public:
    MockCpuidBase()
        : mockCpuidUtils()
        , filename{}
    {
    }
    MockCpuidUtils mockCpuidUtils;
    String         filename;
};
} // namespace
