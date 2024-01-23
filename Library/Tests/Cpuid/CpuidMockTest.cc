#include "Au/Cpuid/X86Cpu.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>

namespace {

using namespace Au;
class MockCpuidUtils : public CpuidUtils
{
  public:
    MockCpuidUtils()
        : CpuidUtils()
    {
    }
    MOCK_METHOD(ResponseT, __raw_cpuid, (RequestT & req), (override)){};
};

class MockCpuid : public ::testing::Test
{
  public:
    MockCpuid()
        : mockCpuidUtils()
    {
    }
    MockCpuidUtils mockCpuidUtils;

  protected:
    void SetUp() override
    {
        ON_CALL(mockCpuidUtils, __raw_cpuid(RequestT{ 0, 0, 0, 0 }))
            .WillByDefault(testing::Return(ResponseT{ 0, 0, 0, 0 }));
        // There are only 10 calls to the function which is fixed
        EXPECT_CALL(mockCpuidUtils, __raw_cpuid(testing::_)).Times(10);
    }
};
TEST_F(MockCpuid, getVendorInfo)
{
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // Test the behavior
    EXPECT_EQ(mockCpuidUtils.getMfgInfo(RequestT{ 0, 0, 0, 0 }),
              (EVendor::Other));
    ASSERT_FALSE(cpu.isAMD());
    ASSERT_FALSE(cpu.isIntel());
}
TEST_F(MockCpuid, getMfgInfo)
{
    // 86Cpu cpu{ &mockCpuidUtils, 0 };
    // ASSERT_TRUE(cpu.isIntel());
}
} // namespace
