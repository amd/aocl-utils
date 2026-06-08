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
        MockCpuidBase::SetUp();
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
    const auto        cpuType         = std::get<0>(params);
    const auto        expectedResults = std::get<1>(params);
    const auto        uarch           = std::get<2>(params);
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

    // Has flag tests using loadFlagVector helper
    // Assert non-empty to avoid false positives from missing/unreadable files
    auto trueFlags = loadFlagVector(cpuType, "FlagsT.txt");
    ASSERT_FALSE(trueFlags.empty()) << "FlagsT.txt is empty or missing for " << cpuType;
    for (const auto& flag : trueFlags) {
        resultT = resultT & cpu.hasFlag(flag);
    }
    results.push_back(resultT);

    auto falseFlags = loadFlagVector(cpuType, "FlagsF.txt");
    ASSERT_FALSE(falseFlags.empty()) << "FlagsF.txt is empty or missing for " << cpuType;
    for (const auto& flag : falseFlags) {
        resultF = resultF | cpu.hasFlag(flag);
    }
    results.push_back(!resultF);

    std::cout << "Checking Uarch" << std::endl;
    results.push_back(cpu.isUarch(uarch));

    std::cout << "Checking Zen Family" << std::endl;
    results.push_back(cpu.isZenFamily());
    EXPECT_EQ(results, expectedResults);
    EXPECT_EQ(cpu.getUarch(), uarch);
}

// ===================================================================
// Non-parameterized base fixture; inherits SetUp() from MockCpuidBase.
// Other fixtures inherit from this to share SetUp() behavior.
// ===================================================================
class MockX86CpuNonParam : public MockCpuidBase
{
  protected:
    void SetUp() override
    {
        MockCpuidBase::SetUp();
    }
};

// ===================================================================
// Parameterized fixture for extended tests (covers ALL CPUs in
// testParametersX86Cpu). Inherits SetUp() from MockX86CpuNonParam.
// ===================================================================
class MockX86CpuExtended
    : public MockX86CpuNonParam
    , public ::testing::WithParamInterface<
          std::tuple<String, std::vector<bool>, EUarch>>
{};

INSTANTIATE_TEST_SUITE_P(MockX86CpuExtendedSuite,
                         MockX86CpuExtended,
                         ::testing::ValuesIn(testParametersX86Cpu));

/**
 * HasFlagsModes - validate hasFlags(All/Classic/Any) for every CPU.
 * Loads FlagsT/FlagsF once and tests all three modes plus a mixed vector.
 * Per-CPU data completeness is enforced via ASSERT_FALSE on empty vectors,
 * which provides a content-based skip if the fixture data is incomplete.
 */
TEST_P(MockX86CpuExtended, HasFlagsModes)
{
    const auto params  = GetParam();
    const auto cpuType = std::get<0>(params);

    filename = cpuType;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // Load flag vectors and assert non-empty before creating BufferView
    // (BufferView asserts on nullptr which empty vector's data() returns)
    auto trueFlags = loadFlagVector(cpuType, "FlagsT.txt");
    ASSERT_FALSE(trueFlags.empty()) << "FlagsT.txt is empty or missing for " << cpuType;
    Au::Memory::BufferView<ECpuidFlag> trueFlagsView(trueFlags.data(),
                                                     trueFlags.size());
    EXPECT_TRUE(cpu.hasFlags(trueFlagsView, HasFlagsMode::All));
    EXPECT_TRUE(cpu.hasFlags(trueFlagsView, HasFlagsMode::Classic));
    EXPECT_TRUE(cpu.hasFlags(trueFlagsView, HasFlagsMode::Any));

    auto falseFlags = loadFlagVector(cpuType, "FlagsF.txt");
    ASSERT_FALSE(falseFlags.empty()) << "FlagsF.txt is empty or missing for " << cpuType;
    Au::Memory::BufferView<ECpuidFlag> falseFlagsView(falseFlags.data(),
                                                      falseFlags.size());
    EXPECT_FALSE(cpu.hasFlags(falseFlagsView, HasFlagsMode::All));
    EXPECT_FALSE(cpu.hasFlags(falseFlagsView, HasFlagsMode::Classic));
    EXPECT_FALSE(cpu.hasFlags(falseFlagsView, HasFlagsMode::Any));

    // Mixed: one true flag + one false flag
    std::vector<ECpuidFlag> mixed;
    mixed.push_back(trueFlags.front());
    mixed.push_back(falseFlags.front());
    Au::Memory::BufferView<ECpuidFlag> mixedView(mixed.data(), mixed.size());
    EXPECT_FALSE(cpu.hasFlags(mixedView, HasFlagsMode::All));  // one absent
    EXPECT_TRUE(cpu.hasFlags(mixedView, HasFlagsMode::Any));   // one present
}

/**
 * IsUarchStrict - validate isUarch(uarch, true) for every CPU.
 * Exact match returns true; every other uarch in [Unknown..Max] returns false.
 * Iterates the full EUarch range so newly-added enum values are exercised
 * automatically without test maintenance.
 */
TEST_P(MockX86CpuExtended, IsUarchStrict)
{
    const auto params  = GetParam();
    const auto cpuType = std::get<0>(params);
    const auto uarch   = std::get<2>(params);

    filename           = cpuType;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // Strict match with the CPU's own uarch must succeed
    EXPECT_TRUE(cpu.isUarch(uarch, true));

    // Any other uarch in strict mode must fail. Iterate the full EUarch range
    // (Unknown..Max) so new enum entries are automatically covered.
    // Note: ZenPlus is treated as equivalent to Zen in isUarch() due to a
    // temporary workaround in X86RawData.cc, so we skip ZenPlus<->Zen pairs.
    for (auto v = static_cast<Uint16>(EUarch::Unknown);
         v <= static_cast<Uint16>(EUarch::Max);
         ++v) {
        auto candidate = static_cast<EUarch>(v);
        if (candidate == uarch)
            continue;
        // ZenPlus equivalence: isUarch(ZenPlus) == isUarch(Zen)
        if (candidate == EUarch::ZenPlus && uarch == EUarch::Zen)
            continue;
        if (candidate == EUarch::Zen && uarch == EUarch::ZenPlus)
            continue;
        EXPECT_FALSE(cpu.isUarch(candidate, true))
            << "strict isUarch should reject " << static_cast<int>(candidate)
            << " for " << cpuType;
    }
}

/**
 * IsUarchNonStrictLower - verify that non-strict isUarch returns true for
 * all uarchs at or below the CPU's actual uarch, and false otherwise.
 * Iterates the full EUarch range so newly-added enum values are exercised
 * automatically without test maintenance.
 */
TEST_P(MockX86CpuExtended, IsUarchNonStrictLower)
{
    const auto params  = GetParam();
    const auto cpuType = std::get<0>(params);
    const auto uarch   = std::get<2>(params);

    filename = cpuType;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // Every uarch <= actual should return true in non-strict mode.
    // Note: ZenPlus is treated as equivalent to Zen in isUarch() due to a
    // temporary workaround in X86RawData.cc, so normalize before comparing.
    auto effectiveUarch = (uarch == EUarch::ZenPlus) ? EUarch::Zen : uarch;
    for (auto v = static_cast<Uint16>(EUarch::Unknown);
         v <= static_cast<Uint16>(EUarch::Max);
         ++v) {
        auto candidate          = static_cast<EUarch>(v);
        auto effectiveCandidate = (candidate == EUarch::ZenPlus)
                                      ? EUarch::Zen
                                      : candidate;
        if (effectiveCandidate <= effectiveUarch) {
            EXPECT_TRUE(cpu.isUarch(candidate))
                << "non-strict isUarch should accept "
                << static_cast<int>(candidate) << " for " << cpuType;
        } else {
            EXPECT_FALSE(cpu.isUarch(candidate))
                << "non-strict isUarch should reject "
                << static_cast<int>(candidate) << " for " << cpuType;
        }
    }
}

/**
 * IsAvailable - validate the deprecated isAvailable() API returns the same
 * result as hasFlag() for every true and false flag per CPU.
 * Per-CPU data completeness is enforced via ASSERT_FALSE on empty vectors,
 * which provides a content-based skip if the fixture data is incomplete.
 */
TEST_P(MockX86CpuExtended, IsAvailable)
{
    const auto params  = GetParam();
    const auto cpuType = std::get<0>(params);

    filename           = cpuType;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // Verify equivalence with hasFlag for true flags
    auto trueFlags = loadFlagVector(cpuType, "FlagsT.txt");
    ASSERT_FALSE(trueFlags.empty())
        << "Failed to load true flag data for CPU: " << cpuType;
    for (const auto& flag : trueFlags)
        EXPECT_EQ(cpu.isAvailable(flag), cpu.hasFlag(flag));

    // Verify equivalence with hasFlag for false flags
    auto falseFlags = loadFlagVector(cpuType, "FlagsF.txt");
    ASSERT_FALSE(falseFlags.empty())
        << "Failed to load false flag data for CPU: " << cpuType;
    for (const auto& flag : falseFlags)
        EXPECT_EQ(cpu.isAvailable(flag), cpu.hasFlag(flag));
}

// ===================================================================
// Data-driven isUarch test cases - add new cases here when adding CPUs
// ===================================================================
struct IsUarchCase { const char* cpu; EUarch query; bool strict; bool result; const char* desc; };
// clang-format off
static const IsUarchCase kIsUarchCases[] = {
    // Zen4 CPU asked about higher uarchs
    {"EPYC-Genoa-v1",    EUarch::Zen5,    false, false, "Zen4 -> Zen5 non-strict"   },
    {"EPYC-Genoa-v1",    EUarch::Zen6,    false, false, "Zen4 -> Zen6 non-strict"   },
    {"EPYC-Genoa-v1",    EUarch::Zen4,    true,  true,  "Zen4 -> Zen4 strict"       },
    // Zen5 CPU
    {"EPYC-Turin-v1",    EUarch::Zen6,    false, false, "Zen5 -> Zen6 non-strict"   },
    {"EPYC-Turin-v1",    EUarch::Zen4,    false, true,  "Zen5 -> Zen4 non-strict"   },
    // Zen6 CPU (Venice)
    {"Venice-v1",        EUarch::Zen5,    false, true,  "Zen6 -> Zen5 non-strict"   },
    {"Venice-v1",        EUarch::Zen4,    false, true,  "Zen6 -> Zen4 non-strict"   },
    {"Venice-v1",        EUarch::Zen6,    false, true,  "Zen6 -> Zen6 non-strict"   },
    {"Venice-v1",        EUarch::Zen6,    true,  true,  "Zen6 -> Zen6 strict"       },
    {"Venice-Dense-v1",  EUarch::Zen6,    true,  true,  "Zen6 Dense -> Zen6 strict" },
    {"Venice-Dense-v1",  EUarch::Zen5,    true,  false, "Zen6 Dense -> Zen5 strict" },
    // Intel CPUs (Unknown uarch)
    {"Conroe-v1",        EUarch::Zen,     false, false, "Intel -> Zen non-strict"   },
    {"Conroe-v1",        EUarch::Unknown, false, true,  "Intel -> Unknown non-strict"},
    {"Conroe-v1",        EUarch::Zen6,    false, false, "Intel -> Zen6 non-strict"  },
    {"Denverton-v1",     EUarch::Zen,     false, false, "Denverton -> Zen"          },
    {"Denverton-v1",     EUarch::Zen4,    false, false, "Denverton -> Zen4"         },
    {"Denverton-v1",     EUarch::Zen6,    false, false, "Denverton -> Zen6"         },
    {"Denverton-v1",     EUarch::Unknown, false, true,  "Denverton -> Unknown"      },
    // Zen1 CPU asked about higher uarchs
    {"EPYC-v1",          EUarch::Zen2,    false, false, "Zen1 -> Zen2 non-strict"   },
    {"EPYC-v1",          EUarch::Zen6,    false, false, "Zen1 -> Zen6 non-strict"   },
    {"EPYC-v1",          EUarch::Zen,     true,  true,  "Zen1 -> Zen strict"        },
};
// clang-format on

class MockX86CpuIsUarch
    : public MockX86CpuNonParam
    , public ::testing::WithParamInterface<IsUarchCase>
{};

TEST_P(MockX86CpuIsUarch, MatchesExpected)
{
    const auto& p = GetParam();
    filename      = p.cpu;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };
    EXPECT_EQ(cpu.isUarch(p.query, p.strict), p.result) << p.desc;
}

INSTANTIATE_TEST_SUITE_P(All,
                         MockX86CpuIsUarch,
                         ::testing::ValuesIn(kIsUarchCases));

/**
 * IsUarchOutOfRange - out-of-range uarch (beyond EUarch::Max) must return false.
 */
TEST_F(MockX86CpuNonParam, IsUarchOutOfRange)
{
    auto aboveMax = static_cast<EUarch>(static_cast<Uint16>(EUarch::Max) + 1);

    filename = "EPYC-Genoa-v1";
    Configure();
    X86Cpu cpu1{ &mockCpuidUtils, 0 };
    EXPECT_FALSE(cpu1.isUarch(aboveMax));
    EXPECT_FALSE(cpu1.isUarch(aboveMax, true));

    filename = "Venice-v1";
    Configure();
    X86Cpu cpu2{ &mockCpuidUtils, 0 };
    EXPECT_FALSE(cpu2.isUarch(aboveMax));
    EXPECT_FALSE(cpu2.isUarch(aboveMax, true));
}

/**
 * HasFlagsEmpty - edge case: empty flag set (CPU-independent).
 */
TEST_F(MockX86CpuNonParam, HasFlagsEmpty)
{
    filename = "EPYC-Genoa-v1";
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    // TODO(BufferView): default-constructed BufferView() should represent an
    // empty view, but currently asserts on nullptr. As a workaround we pass a
    // valid pointer with size 0. Remove this once BufferView supports empty
    // initialization natively.
    ECpuidFlag                         dummy{};
    Au::Memory::BufferView<ECpuidFlag> emptyView(&dummy, 0);

    // All-mode on empty set: vacuous truth
    EXPECT_TRUE(cpu.hasFlags(emptyView, HasFlagsMode::All));
    // Any-mode on empty set: no match possible
    EXPECT_FALSE(cpu.hasFlags(emptyView, HasFlagsMode::Any));
}

} // namespace
