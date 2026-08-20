/*
 * Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
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
#include "Au/Cpuid/CacheInfo.hh"
#include "MockTest.hh"

namespace {

using namespace Au;

class MockCpuidUtil
    : public MockCpuidBase
    , public ::testing::WithParamInterface<std::tuple<String, VendorInfo>>
{};

INSTANTIATE_TEST_SUITE_P(CpuidUtilsTestSuite,
                         MockCpuidUtil,
                         ::testing::ValuesIn(testParametersCpuidUtils));
/* CpuidUtils test from mocked simnowdata, validates against VendorInfo expectations. */

TEST_P(MockCpuidUtil, CpuidUtilsTest)
{
    const auto       params          = GetParam();
    const String     cpuType         = std::get<0>(params);
    const VendorInfo expectedResults = std::get<1>(params);
    filename                         = cpuType;
    auto reqRespData                 = Configure();

    std::cout << "Mocking " << cpuType << std::endl;
    EXPECT_EQ(CpuidUtils::getMfgInfo(reqRespData[RequestT{ 0, 0, 0, 0 }]),
              (expectedResults.m_mfg));
    EXPECT_EQ(CpuidUtils::getFamily(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
              (expectedResults.m_family));
    EXPECT_EQ(CpuidUtils::getModel(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
              expectedResults.m_model);
    EXPECT_EQ(CpuidUtils::getStepping(reqRespData[RequestT{ 1, 0, 0, 0 }].eax),
              expectedResults.m_stepping);
}

// hasFlag: (expected & actual) == expected
struct HasFlagCase { CpuidRegs actual, expected; bool result; const char* desc; };
// clang-format off
static const HasFlagCase kHasFlagCases[] = {
    // actual                                         expected              result  description
    {{0,0,0x10000000,0},                              {0,0,0x10000000,0},   true,   "exact match"                      },
    {{0,0,0xFFFFFFFF,0},                              {0,0,0x10000000,0},   true,   "expected subset of actual"        },
    {{0,0,0x10000000,0},                              {0,0,0x00000000,0},   true,   "empty expected - vacuous truth"   },
    {{0,0,0x10000000,0},                              {0,0,0xFFFFFFFF,0},   false,  "expected has bits not in actual"  },
    {{0,0,0x10000000,0},                              {0,0,0x0FFFFFFF,0},   false,  "expected missing bit 28 in actual"},
    {{0,0,0,0},                                       {0,0,0,0},            true,   "both empty"                       },
    {{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},   {0,0,0,0},            true,   "empty expected with full actual"  },
    {{0,0xFF,0,0},                                    {0,0x20,0,0},         true,   "expected subset multi-register"   },
    {{0x1,0x2,0x4,0x8},                               {0x1,0x2,0x4,0x8},    true,   "multi all match"                  },
    {{0x1,0x2,0x4,0x8},                               {0x1,0x2,0,0x8},      true,   "expected subset - zero is subset" },
    {{0x1,0x2,0,0x8},                                 {0x1,0x2,0x4,0x8},    false,  "expected has ecx=4 not in actual" },
};
// clang-format on
class CpuidUtilsHasFlag : public ::testing::TestWithParam<HasFlagCase> {};
TEST_P(CpuidUtilsHasFlag, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ(CpuidUtils::hasFlag(p.expected, p.actual), p.result) << p.desc;
}
  INSTANTIATE_TEST_SUITE_P(All, CpuidUtilsHasFlag, ::testing::ValuesIn(kHasFlagCases));

// ---------------------------------------------------------------------------
// 2. CpuidUtilsGetMfgInfoOther - test getMfgInfo() with non-AMD/Intel values
// ---------------------------------------------------------------------------
struct GetMfgInfoCase { CpuidRegs regs; EVendor result; const char* desc; };
static const GetMfgInfoCase kGetMfgInfoCases[] = {
    {{0,0,0,0},                                     EVendor::Other, "all zeros"              },
    {{0,0x12345678,0x12345678,0x12345678},          EVendor::Other, "random values"          },
    {{0,0x68747541,0,0},                            EVendor::Other, "partial AMD"            },
    {{0,0x756e6547,0,0},                            EVendor::Other, "partial Intel"          },
    {{0,0x68747541,0x444d4164,0x69746e65},          EVendor::Other, "almost AMD ecx off"     },
    {{0,0x756e6547,0x6c65746e,0x49656e70},          EVendor::Other, "almost Intel edx off"   },
    {{0,0x68747541,0x444d4163,0x69746e66},          EVendor::Other, "AMD edx wrong"          },
    {{0,0x444d4163,0x68747541,0x69746e65},          EVendor::Other, "swapped AMD ebx ecx"    },
    {{0xFFFFFFFF,0x68747541,0x444d4163,0x69746e65}, EVendor::Amd,   "AMD with non-zero eax"  },
    {{0,0x756e6547,0x6c65746e,0x49656e69},          EVendor::Intel, "valid Intel GenuineIntel"},
};
class CpuidUtilsGetMfgInfo : public ::testing::TestWithParam<GetMfgInfoCase> {};
TEST_P(CpuidUtilsGetMfgInfo, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ(CpuidUtils::getMfgInfo(p.regs), p.result) << p.desc;
}
INSTANTIATE_TEST_SUITE_P(All, CpuidUtilsGetMfgInfo, ::testing::ValuesIn(kGetMfgInfoCases));

// ---------------------------------------------------------------------------
// 3. CpuidUtilsGetFamilyBoundary - test getFamily() edge/boundary values
// ---------------------------------------------------------------------------
// EAX leaf 1: ExtFam[27:20] BaseFam[11:8]; Family = BaseFam + ExtFam (if BaseFam >= 0xF)
struct GetFamilyCase { Uint32 eax; EFamily result; const char* desc; };
static const GetFamilyCase kGetFamilyCases[] = {
    {0x00800F00, EFamily::Family17h,     "Zen (Family 0x17)"           },
    {0x00000000, EFamily::Unknown, "zero"                        },
    {0x00000100, EFamily::Unknown, "very small (BaseFamily=1)"   },
    {0x00700F00, EFamily::Unknown, "just below Zen (Family 0x16)"},
    {0x00B00F00, EFamily::Family1Ah,    "Zen5 (Family 0x1A)"          },
    {0x00C00F00, EFamily::Family1Bh,    "Zen6 (Family 0x1B)"         },
    {0xFFFFFFFF, EFamily::Unknown, "all bits set"                },
};
class CpuidUtilsGetFamily : public ::testing::TestWithParam<GetFamilyCase> {};
TEST_P(CpuidUtilsGetFamily, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ(CpuidUtils::getFamily(p.eax), p.result) << p.desc;
}
INSTANTIATE_TEST_SUITE_P(All, CpuidUtilsGetFamily, ::testing::ValuesIn(kGetFamilyCases));

// ---------------------------------------------------------------------------
// 4. CpuidUtilsGetModelEdge - test getModel() edge values
// ---------------------------------------------------------------------------
TEST(CpuidUtilsStatic, GetModelZero)         { EXPECT_EQ(CpuidUtils::getModel(0x00000000), 0x00); }
TEST(CpuidUtilsStatic, GetModelAllBitsSet)   { EXPECT_EQ(CpuidUtils::getModel(0xFFFFFFFF), 0xFF); }
TEST(CpuidUtilsStatic, GetModelExtOnly)      { EXPECT_EQ(CpuidUtils::getModel(0x000F0000), 0xF0); }
TEST(CpuidUtilsStatic, GetModelBaseOnly)     { EXPECT_EQ(CpuidUtils::getModel(0x000000F0), 0x0F); }
TEST(CpuidUtilsStatic, GetModelMixed)        { EXPECT_EQ(CpuidUtils::getModel(0x00050030), 0x53); }

// ---------------------------------------------------------------------------
// 5. CpuidUtilsGetSteppingEdge - test getStepping() edge values
// ---------------------------------------------------------------------------
TEST(CpuidUtilsStatic, GetSteppingZero)          { EXPECT_EQ(CpuidUtils::getStepping(0x00000000), 0x0); }
TEST(CpuidUtilsStatic, GetSteppingMax)           { EXPECT_EQ(CpuidUtils::getStepping(0x0000000F), 0xF); }
TEST(CpuidUtilsStatic, GetSteppingHighBitsOnly)  { EXPECT_EQ(CpuidUtils::getStepping(0xFFFFFFF0), 0x0); }
TEST(CpuidUtilsStatic, GetSteppingAllBitsSet)    { EXPECT_EQ(CpuidUtils::getStepping(0xFFFFFFFF), 0xF); }
TEST(CpuidUtilsStatic, GetSteppingMidValue)      { EXPECT_EQ(CpuidUtils::getStepping(0x00000007), 0x7); }

// ---------------------------------------------------------------------------
// 6. CpuidUtilsUpdateCacheInfo - test updateCacheInfo() with Genoa simnow data
// ---------------------------------------------------------------------------
TEST(CpuidUtilsStatic, UpdateCacheInfo)
{
    // L1 DCache entry from EPYC-Genoa-v1 simnow data:
    // {0x8000001d,0x0,0x1,0x0}:{0x122,0x1c0003f,0x3f,0x1}
    CacheInfo cInfo{ CacheLevel::L1, CacheType::DCache };
    ResponseT resp{ 0x122, 0x1c0003f, 0x3f, 0x1 };
    CpuidUtils::updateCacheInfo(cInfo, resp);
    EXPECT_EQ(cInfo.getLevel(), CacheLevel::L1);
    EXPECT_EQ(cInfo.getType(), CacheType::DCache);
    EXPECT_EQ(cInfo.getLane(), 64u);
    EXPECT_EQ(cInfo.getWay(), 8u);
    EXPECT_EQ(cInfo.getSets(), 64u);
    EXPECT_EQ(cInfo.getSize(), 32768u);
}


// Family ID 0x1A (Venice/Venice-Dense) maps to EFamily::Family1Ah.
TEST(CpuidUtilsStatic, GetFamilyFamilyID0x1AVenice)
{
    // Venice-v1: Family 0x1A model 0x50 -> EAX 0x00B50F00
    EXPECT_EQ(CpuidUtils::getFamily(0x00B50F00), EFamily::Family1Ah);
}

TEST(CpuidUtilsStatic, GetFamilyFamilyID0x1AVeniceDense)
{
    // Venice-Dense-v1: Family 0x1A model 0x51 -> EAX 0x00B50F10
    EXPECT_EQ(CpuidUtils::getFamily(0x00B50F10), EFamily::Family1Ah);
}

TEST(CpuidUtilsStatic, GetModelFamilyID0x1AVenice)
{
    // Venice-v1 model 0x50
    EXPECT_EQ(CpuidUtils::getModel(0x00B50F00), 0x50);
}

TEST(CpuidUtilsStatic, GetModelFamilyID0x1AVeniceDense)
{
    // Venice-Dense-v1 model 0x51
    EXPECT_EQ(CpuidUtils::getModel(0x00B50F10), 0x51);
}

// ---------------------------------------------------------------------------
// 7. CpuidRegsOperators - test CpuidRegs struct operators
// ---------------------------------------------------------------------------
struct EqualityCase { CpuidRegs lhs, rhs; bool result; const char* desc; };
static const EqualityCase kEqualityCases[] = {
    {{1,2,3,4}, {1,2,3,4}, true,  "same"      },
    {{1,2,3,4}, {0,2,3,4}, false, "diff eax"  },
    {{1,2,3,4}, {1,0,3,4}, false, "diff ebx"  },
    {{1,2,3,4}, {1,2,0,4}, false, "diff ecx"  },
    {{1,2,3,4}, {1,2,3,0}, false, "diff edx"  },
    {{0,0,0,0}, {0,0,0,0}, true,  "all zeros" },
};
class CpuidRegsEquality : public ::testing::TestWithParam<EqualityCase> {};
TEST_P(CpuidRegsEquality, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ((p.lhs == p.rhs), p.result) << p.desc;
}
INSTANTIATE_TEST_SUITE_P(All, CpuidRegsEquality, ::testing::ValuesIn(kEqualityCases));

struct LessThanCase { CpuidRegs lhs, rhs; bool result; const char* desc; };
static const LessThanCase kLessThanCases[] = {
    {{1,0,0,0}, {2,0,0,0}, true,  "true"         },
    {{2,0,0,0}, {1,0,0,0}, false, "false"        },
    {{1,1,0,0}, {0,0,0,3}, true,  "diff fields"  },
    {{1,1,0,0}, {1,1,0,0}, false, "same operands"},
};
class CpuidRegsLessThan : public ::testing::TestWithParam<LessThanCase> {};
TEST_P(CpuidRegsLessThan, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ((p.lhs < p.rhs), p.result) << p.desc;
}
INSTANTIATE_TEST_SUITE_P(All, CpuidRegsLessThan, ::testing::ValuesIn(kLessThanCases));

struct BitwiseAndCase { CpuidRegs lhs, rhs, result; const char* desc; };
static const BitwiseAndCase kBitwiseAndCases[] = {
    {{0xFF,0xFF,0xFF,0xFF}, {0x0F,0x0F,0x0F,0x0F}, {0x0F,0x0F,0x0F,0x0F}, "masked"  },
    {{0xFF,0,0,0},          {0,0xFF,0,0},          {0,0,0,0},             "disjoint"},
};
class CpuidRegsBitwiseAnd : public ::testing::TestWithParam<BitwiseAndCase> {};
TEST_P(CpuidRegsBitwiseAnd, MatchesExpected) {
    const auto& p = GetParam();
    EXPECT_EQ((p.lhs & p.rhs), p.result) << p.desc;
}
INSTANTIATE_TEST_SUITE_P(All, CpuidRegsBitwiseAnd, ::testing::ValuesIn(kBitwiseAndCases));

} // namespace
