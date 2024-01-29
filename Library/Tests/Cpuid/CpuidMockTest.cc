#include "Au/Cpuid/X86Cpu.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

using namespace Au;
using namespace std;
class MockCpuidUtils : public CpuidUtils
{
  public:
    MockCpuidUtils()
        : CpuidUtils()
    {
    }
    MOCK_METHOD(ResponseT, __raw_cpuid, (RequestT & req), (override)){};
};

class MockCpuidBase : public testing::Test
{
  protected:
    vector<pair<RequestT, ResponseT>> parseCSV(const string& filename)
    {
        vector<pair<RequestT, ResponseT>> data;

        cout << "Processing" << filename << std::endl;
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return data; // Return an empty vector if the file cannot be opened
        }

        string line;
        while (getline(file, line)) {
            istringstream lineStream(line);
            string        requestStr, respStr;

            // Assuming the CSV structure is
            // {1,0,0,0}:{329300,2048,4294586883,126614525}
            getline(lineStream, requestStr, ':');
            getline(lineStream, respStr, ':');
            // Extract values between curly braces and populate structures
            ResponseT request{}; // The request needs to be updated, hence not
                                 // using RequestT here.
            ResponseT response{};

            sscanf(requestStr.c_str(),
                   "{%u,%u,%u,%u}",
                   &request.eax,
                   &request.ebx,
                   &request.ecx,
                   &request.edx);
            sscanf(respStr.c_str(),
                   "{%u,%u,%u,%u}",
                   &response.eax,
                   &response.ebx,
                   &response.ecx,
                   &response.edx);

            // Insert into the vector
            data.emplace_back(request, response);
        }

        file.close();
        return data;
    }

  public:
    MockCpuidBase()
        : mockCpuidUtils()
    {
    }
    MockCpuidUtils mockCpuidUtils;
};
class MockCpuidUtil
    : public MockCpuidBase
    , public ::testing::WithParamInterface<tuple<string, VendorInfo>>
{
  public:
    MockCpuidUtil()
        : filename{}
    {
    }

  protected:
    string filename;
    void   Configure()
    {
        vector<pair<RequestT, ResponseT>> csvData = parseCSV(filename);
        /*TODO Define the Return values for each utility function*/
    }
};
const vector<tuple<string, VendorInfo>> testParametersCpuidUtils = {
    { "EPYC-Genoa-v1", { VendorInfo{ EVendor::Amd, EFamily::Zen4, 0, 0 } } },
    { "Skylake-Server-v1", { VendorInfo{ EVendor::Intel, EFamily::Zen4, 0, 0 } }
      /*Define the expected outputs */ },
};

INSTANTIATE_TEST_SUITE_P(CupidUtilstTestSuite,
                         MockCpuidUtil,
                         ::testing::ValuesIn(testParametersCpuidUtils));

TEST_P(MockCpuidUtil, CpuidUtilsTest)
{
    // Test the behavior
    const auto   params  = GetParam();
    const string cpuType = get<0>(params);
    EXPECT_EQ(mockCpuidUtils.getMfgInfo(ResponseT{ 0, 0, 0, 0 }),
              (EVendor::Other));
    EXPECT_EQ(mockCpuidUtils.getFamily(0), (EFamily::Max));
    EXPECT_EQ(mockCpuidUtils.getModel(0), 0);
    EXPECT_EQ(mockCpuidUtils.getStepping(0), 0);
}
class MockX86Cpu
    : public MockCpuidBase
    , public ::testing::WithParamInterface<tuple<string, vector<bool>>>
{
  public:
    MockX86Cpu()
        : filename{}
    {
    }

  protected:
    string filename;
    void   Configure()
    {
        vector<pair<RequestT, ResponseT>> csvData = parseCSV(filename);
        for (const auto& entry : csvData) {
            // ON_CALL(mockCpuidUtils,
            //         __raw_cpuid(RequestT{ entry.first.eax,
            //                               entry.first.ebx,
            //                               entry.first.ecx,
            //                               entry.first.edx }))
            //     .WillByDefault(testing::Return(ResponseT{ entry.second.eax,
            //                                               entry.second.ebx,
            //                                               entry.second.ecx,
            //                                               entry.second.edx
            //                                               }));
            ON_CALL(mockCpuidUtils, __raw_cpuid(entry.first))
                .WillByDefault(testing::Return(entry.second));
        }
    }
};
const vector<tuple<string, vector<bool>>> testParametersX86Cpu = {
    { "EPYC-Genoa-v1", { true, false, true, true, false } },
    { "EPYC-Milan-v1", { true, false, true, true, false } },
    { "EPYC-Milan-v2", { true, false, true, true, false } },
    { "EPYC-Rome-v1", { true, false, true, true, false } },
    { "EPYC-Rome-v2", { true, false, true, true, false } },
    { "EPYC-Rome-v3", { true, false, true, true, false } },
    { "EPYC-Rome-v4", { true, false, true, true, false } },
    { "EPYC-v1", { true, false, true, true, false } },
    { "EPYC-v2", { true, false, true, true, false } },
    { "EPYC-v3", { true, false, true, true, false } },
    { "EPYC-v4", { true, false, true, true, false } },
    { "Opteron_G1-v1", { true, false, false, false, false } },
    { "Opteron_G2-v1", { true, false, false, false, false } },
    { "Opteron_G3-v1", { true, false, false, false, false } },
    { "Opteron_G4-v1", { true, false, true, false, false } },
    { "Opteron_G5-v1", { true, false, true, false, false } },
    { "phenom-v1", { true, false, false, false, false } },
    { "Broadwell-v1", { false, true, true, true, false } },
    { "Denverton-v1", { false, true, true, false, false } },
    { "Conroe-v1", { false, true, false, false, false } },
    { "Skylake-Server-v1", { false, true, true, true, true } }
};

INSTANTIATE_TEST_SUITE_P(MockX86CpuTestSuite,
                         MockX86Cpu,
                         ::testing::ValuesIn(testParametersX86Cpu));
TEST_P(MockX86Cpu, MockX86CpuTest)
{
    // There are only 11 calls to the function which is fixed
    // EXPECT_CALL(mockCpuidUtils, __raw_cpuid(testing::_)).Times(12);

    const auto         params          = GetParam();
    const vector<bool> expectedResults = get<1>(params);
    const string       cpuType         = get<0>(params);
    vector<bool>       results;
    bool               result;
    string             projectDir   = PROJECT_SOURCE_DIR;
    string             testDir      = "/Library/Tests/Cpuid/simnowdata/";
    string             dataFilename = projectDir + testDir + cpuType;

    cout << "Mocking " << cpuType << endl;
    filename = dataFilename;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };
    result = cpu.isAMD();
    results.push_back(result);
    result = cpu.isIntel();
    results.push_back(result);
    result = cpu.isX86_64v2();
    results.push_back(result);
    result = cpu.isX86_64v3();
    results.push_back(result);
    result = cpu.isX86_64v4();
    results.push_back(result);
    EXPECT_EQ(results, expectedResults);
}
} // namespace