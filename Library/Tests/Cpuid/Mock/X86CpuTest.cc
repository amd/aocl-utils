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
#include "Capi/au/cpuid/cpuid_inline.h"
#include "MockTest.hh"
#include <cstdio>
#include <cstring>
#include <vector>

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
/* X86Cpu test from mocked simnowdata, validates against expected results. */
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

/* HasFlagsModes: validate all three hasFlags modes (All/Classic/Any) per CPU. */
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

/* IsUarchStrict: validate strict isUarch (exact match only). */
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

/* IsUarchNonStrictLower: non-strict isUarch returns true for uarch <= actual. */
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

/* IsAvailable: validate deprecated isAvailable() matches hasFlag(). */
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

/* IsUarchOutOfRange: out-of-range uarch returns false. */
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

/* HasFlagsEmpty: empty flag set edge case. */
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

/* Parity tests: C core and C++ X86Cpu must decode identically (same resolver). */
namespace parity_fold {

struct DecodedTuple
{
    bool     isAmd = false;
    bool     isIntel = false;
    uint16_t family = 0;
    uint16_t model = 0;
    int      uarch = -1;
    bool     v2 = false, v3 = false, v4 = false, zen = false;
    bool     avx2 = false, avx512f = false, sse2 = false;

    bool operator==(const DecodedTuple& o) const
    {
        return isAmd == o.isAmd && isIntel == o.isIntel && family == o.family
               && model == o.model && uarch == o.uarch && v2 == o.v2
               && v3 == o.v3 && v4 == o.v4 && zen == o.zen && avx2 == o.avx2
               && avx512f == o.avx512f && sse2 == o.sse2;
    }
};

struct CMockEntry
{
    uint32_t        eax;
    uint32_t        ecx;
    au_cpuid_regs_t regs;
};

/* Independent parser (separate from MockCpuidBase::parseCSV). */
class CMockTable
{
  public:
    void load(const std::string& path)
    {
        std::ifstream in(path);
        if (!in.is_open()) {
            ADD_FAILURE() << "cannot open simnow file: " << path;
            return;
        }
        std::string line;
        while (std::getline(in, line))
            parseLine(line);
    }

    void lookup(uint32_t eax, uint32_t ecx, au_cpuid_regs_t* out) const
    {
        memset(out, 0, sizeof(*out));
        for (const auto& e : m_entries) {
            if (e.eax == eax && e.ecx == ecx) {
                *out = e.regs;
                return;
            }
        }
    }

  private:
    void parseLine(const std::string& line)
    {
        unsigned reqEax, reqEbx, reqEcx, reqEdx, eax, ebx, ecx, edx;
        int      n = sscanf(line.c_str(),
                        "{%x,%x,%x,%x}:{%x,%x,%x,%x}",
                        &reqEax,
                        &reqEbx,
                        &reqEcx,
                        &reqEdx,
                        &eax,
                        &ebx,
                        &ecx,
                        &edx);
        if (n != 8)
            return;
        m_entries.push_back(
            CMockEntry{ reqEax, reqEcx, { eax, ebx, ecx, edx } });
    }

    std::vector<CMockEntry> m_entries;
};

void
cMockRawFn(uint32_t eax, uint32_t ecx, au_cpuid_regs_t* out, void* ctx)
{
    static_cast<CMockTable*>(ctx)->lookup(eax, ecx, out);
}

DecodedTuple
decodeViaC(const std::string& simnowPath, int cpuNum = AU_CPUID_CURRENT_CPU)
{
    CMockTable table;
    table.load(simnowPath);

    au_cpu_info_t c;
    memset(&c, 0, sizeof(c));
    c.raw_fn  = cMockRawFn;
    c.raw_ctx = &table;
    /* Mock path never pins; cpuNum only verifies specific vs CURRENT decode identically. */
    au_cpuid_init(&c, cpuNum, false);

    DecodedTuple t;
    t.isAmd   = au_cpuid_info_is_amd(&c);
    t.isIntel = au_cpuid_info_is_intel(&c);
    t.family  = c.family;
    t.model   = c.model;
    t.uarch   = (int)c.uarch;
    t.v2      = au_cpuid_info_is_x86_64v2(&c);
    t.v3      = au_cpuid_info_is_x86_64v3(&c);
    t.v4      = au_cpuid_info_is_x86_64v4(&c);
    t.zen     = au_cpuid_info_is_zen_family(&c);
    t.avx2    = au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx2"));
    t.avx512f =
        au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx512f"));
    t.sse2 = au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("sse2"));
    return t;
}

DecodedTuple
decodeViaCpp(X86Cpu& cpu)
{
    DecodedTuple t;
    t.isAmd         = cpu.isAMD();
    t.isIntel       = cpu.isIntel();
    VendorInfo vi   = cpu.getVendorInfo();
    t.family        = (uint16_t)vi.m_family;
    t.model         = vi.m_model;
    t.uarch         = (int)vi.m_uarch;
    t.v2            = cpu.isX86_64v2();
    t.v3            = cpu.isX86_64v3();
    t.v4            = cpu.isX86_64v4();
    t.zen           = cpu.isZenFamily();
    t.avx2          = cpu.hasFlag(ECpuidFlag::avx2);
    t.avx512f       = cpu.hasFlag(ECpuidFlag::avx512f);
    t.sse2          = cpu.hasFlag(ECpuidFlag::sse2);
    return t;
}

bool
fileContainsFlagLine(const std::string& path, const std::string& flag)
{
    std::ifstream in(path);
    std::string   line;
    while (std::getline(in, line)) {
        if (line == flag)
            return true;
    }
    return false;
}

/* Reads FlagsT/FlagsF, asserts no contradiction. */
bool
oracleFlag(const std::string& model, const std::string& flag)
{
    const std::string base =
        std::string(PROJECT_SOURCE_DIR) + "/Library/Tests/Cpuid/Mock/simnowdata/"
        + model + "/";
    bool inTrue  = fileContainsFlagLine(base + "FlagsT.txt", flag);
    bool inFalse = fileContainsFlagLine(base + "FlagsF.txt", flag);
    if (inTrue == inFalse) {
        ADD_FAILURE() << "oracle inconsistency for " << model << "/" << flag;
        return false;
    }
    return inTrue;
}

/* Builds oracle from MockTest.hh tables and FlagsT/F.txt ground truth. */
DecodedTuple
buildOracle(const std::string& model)
{
    DecodedTuple t;
    for (const auto& p : testParametersCpuidUtils) {
        if (std::get<0>(p) != model)
            continue;
        const VendorInfo& vi = std::get<1>(p);
        t.isAmd               = (vi.m_mfg == EVendor::Amd);
        t.isIntel              = (vi.m_mfg == EVendor::Intel);
        t.family               = (uint16_t)vi.m_family;
        t.model                = vi.m_model;
        break;
    }
    for (const auto& p : testParametersX86Cpu) {
        if (std::get<0>(p) != model)
            continue;
        const auto& results = std::get<1>(p);
        t.v2                 = results[2];
        t.v3                 = results[3];
        t.v4                 = results[4];
        t.zen                = results[8];
        t.uarch              = (int)std::get<2>(p);
        break;
    }
    t.avx2    = oracleFlag(model, "avx2");
    t.avx512f = oracleFlag(model, "avx512f");
    t.sse2    = oracleFlag(model, "sse2");
    return t;
}

} // namespace parity_fold

class CpuidParity
    : public MockCpuidBase
    , public ::testing::WithParamInterface<std::string>
{};

INSTANTIATE_TEST_SUITE_P(CpuidParitySuite,
                         CpuidParity,
                         ::testing::Values(std::string("EPYC-Genoa-v1"),
                                           std::string("EPYC-Milan-v1"),
                                           std::string("EPYC-Rome-v1"),
                                           std::string("Venice-v1"),
                                           std::string("EPYC-Turin-v1"),
                                           std::string("Opteron_G1-v1"),
                                           std::string("Conroe-v1")));

/* CAndCppAndOracleAgree: C core, C++ X86Cpu, and oracle must all match. */
TEST_P(CpuidParity, CAndCppAndOracleAgree)
{
    const std::string cpuType = GetParam();

    filename = cpuType;
    Configure();
    X86Cpu cpu{ &mockCpuidUtils, 0 };

    const std::string simnowPath = std::string(PROJECT_SOURCE_DIR)
                                   + "/Library/Tests/Cpuid/Mock/simnowdata/"
                                   + cpuType + "/" + cpuType;

    parity_fold::DecodedTuple cTuple   = parity_fold::decodeViaC(simnowPath);
    parity_fold::DecodedTuple cppTuple = parity_fold::decodeViaCpp(cpu);
    parity_fold::DecodedTuple oracle   = parity_fold::buildOracle(cpuType);

    EXPECT_TRUE(cTuple == oracle) << "C vs oracle mismatch for " << cpuType;
    EXPECT_TRUE(cppTuple == oracle) << "C++ vs oracle mismatch for "
                                    << cpuType;
    EXPECT_TRUE(cTuple == cppTuple) << "C vs C++ mismatch for " << cpuType;
}

/* CAndCppAgreeForSpecificNum: specific core number must match via C and C++. */
TEST_P(CpuidParity, CAndCppAgreeForSpecificNum)
{
    const std::string cpuType = GetParam();

    filename = cpuType;
    Configure();

    const std::string simnowPath = std::string(PROJECT_SOURCE_DIR)
                                   + "/Library/Tests/Cpuid/Mock/simnowdata/"
                                   + cpuType + "/" + cpuType;

    // A specific core number through both paths (mock => no pin, pure decode).
    const int          kNum = 3;
    X86Cpu             cppCpu{ &mockCpuidUtils, (CpuNumT)kNum };
    parity_fold::DecodedTuple cNum   = parity_fold::decodeViaC(simnowPath, kNum);
    parity_fold::DecodedTuple cppNum = parity_fold::decodeViaCpp(cppCpu);
    parity_fold::DecodedTuple oracle = parity_fold::buildOracle(cpuType);

    EXPECT_TRUE(cNum == oracle)
        << "C(specific num) vs oracle mismatch for " << cpuType;
    EXPECT_TRUE(cppNum == oracle)
        << "C++(specific num) vs oracle mismatch for " << cpuType;
    EXPECT_TRUE(cNum == cppNum)
        << "C vs C++ mismatch for specific num on " << cpuType;

    // Specific num and the CURRENT sentinel decode identically on the mock
    // path -- both go through the single resolver and neither pins.
    parity_fold::DecodedTuple cCurrent =
        parity_fold::decodeViaC(simnowPath, AU_CPUID_CURRENT_CPU);
    EXPECT_TRUE(cNum == cCurrent)
        << "specific num vs CURRENT decode diverged for " << cpuType;
}

/* OneResolverIsIdempotent: C and C++ share one resolver, re-init is idempotent. */
TEST_P(CpuidParity, OneResolverIsIdempotent)
{
    const std::string cpuType = GetParam();

    filename = cpuType;
    Configure();

    const std::string simnowPath = std::string(PROJECT_SOURCE_DIR)
                                   + "/Library/Tests/Cpuid/Mock/simnowdata/"
                                   + cpuType + "/" + cpuType;

    parity_fold::CMockTable table;
    table.load(simnowPath);

    au_cpu_info_t c;
    memset(&c, 0, sizeof(c));
    c.raw_fn  = parity_fold::cMockRawFn;
    c.raw_ctx = &table;

    au_cpuid_init(&c, AU_CPUID_CURRENT_CPU, false);
    const uint16_t   fam1  = c.family;
    const au_uarch_t ua1   = c.uarch;
    const bool       amd1  = au_cpuid_info_is_amd(&c);
    const bool       v4_1  = au_cpuid_info_is_x86_64v4(&c);

    // Re-resolve the SAME handle (as X86Cpu::update() does): identical output.
    au_cpuid_init(&c, c.cpu_num, false);
    EXPECT_EQ(c.family, fam1) << cpuType;
    EXPECT_EQ(c.uarch, ua1) << cpuType;
    EXPECT_EQ(au_cpuid_info_is_amd(&c), amd1) << cpuType;
    EXPECT_EQ(au_cpuid_info_is_x86_64v4(&c), v4_1) << cpuType;
}

} // namespace
