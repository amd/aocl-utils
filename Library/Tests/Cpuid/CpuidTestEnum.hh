/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

#pragma once

// Standard C++ headers
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <sstream>

// AU headers
#include "Au/Cpuid/Cpuid.hh"

#include "Au/Cpuid/X86Cpu.hh"

namespace Au { namespace Test {

    // Test enum definition using the macro
    AUD_DEFINE_ENUM(ETestFlag, Uint64, flag1, flag2, flag3, flag4, flag5)

    class CpuidEnumTest : public ::testing::Test
    {
      protected:
        void SetUp() override
        {
            // Initialize test vectors
            validFlags   = { "flag1", "flag2", "flag3", "flag4", "flag5" };
            invalidFlags = { "flag0", "flag6", "invalid" };
        }

        std::vector<String> validFlags;
        std::vector<String> invalidFlags;
    };

    // Test string to enum conversion
    TEST_F(CpuidEnumTest, FromStringValidFlags)
    {
        for (size_t i = 0; i < validFlags.size(); i++) {
            EXPECT_EQ(ETestFlagfromString(validFlags[i]), i + 1)
                << "Failed for flag: " << validFlags[i];
        }
    }

    TEST_F(CpuidEnumTest, FromStringInvalidFlags)
    {
        for (const auto& flag : invalidFlags) {
            EXPECT_EQ(ETestFlagfromString(flag), (uint64_t)-1)
                << "Failed for invalid flag: " << flag;
        }
    }

    // Test enum to string conversion
    TEST_F(CpuidEnumTest, ToStringValidValues)
    {
        for (size_t i = 0; i < validFlags.size(); i++) {
            EXPECT_EQ(ETestFlagtoString(i + 1), validFlags[i])
                << "Failed for value: " << (i + 1);
        }
    }

    TEST_F(CpuidEnumTest, ToStringInvalidValues)
    {
        EXPECT_EQ(ETestFlagtoString(0), "UNDEF") << "Failed for value: 0";
        EXPECT_EQ(ETestFlagtoString(validFlags.size() + 1), "UNDEF")
            << "Failed for out of range value";
    }

    // Test string stream operator
    TEST_F(CpuidEnumTest, StreamOperator)
    {
        std::stringstream   ss;
        std::vector<String> testFlags = { "flag1", "flag3", "flag5" };
        ss << testFlags;
        EXPECT_EQ(ss.str(), "1:3:5:");
    }

    // Test overflow protection
    TEST_F(CpuidEnumTest, OverflowProtection)
    {
        std::stringstream   ss;
        std::vector<String> largeVector(2000, "flag1"); // Try to overflow
        ss << largeVector;
        // Should not crash and should produce valid output
        EXPECT_FALSE(ss.str().empty());
    }

    // Test edge cases
    TEST_F(CpuidEnumTest, EdgeCases)
    {
        // Empty string
        EXPECT_EQ(ETestFlagfromString(""), (uint64_t)-1);
        EXPECT_EQ(ETestFlagfromString("   "), (uint64_t)-1);

        // Empty vector
        std::stringstream   ss;
        std::vector<String> emptyVector;
        ss << emptyVector;
        EXPECT_TRUE(ss.str().empty());

        // Whitespace handling - valid cases
        EXPECT_EQ(ETestFlagfromString("  flag1  "), 1)
            << "Failed to handle surrounding whitespace";
        EXPECT_EQ(ETestFlagfromString("\t\nflag1\r\n"), 1)
            << "Failed to handle special whitespace characters";

        // Whitespace handling - invalid cases
        EXPECT_EQ(ETestFlagfromString("flag 1"), (uint64_t)-1)
            << "Should reject strings with internal whitespace";
        EXPECT_EQ(ETestFlagfromString(" "), (uint64_t)-1)
            << "Should reject whitespace-only strings";
    }

    // Test string manipulation safety
    TEST_F(CpuidEnumTest, StringSafety)
    {
        // Test with very long string
        String longString(1024, 'a');
        EXPECT_EQ(ETestFlagfromString(longString), (uint64_t)-1);

        // Test with special characters
        EXPECT_EQ(ETestFlagfromString("flag1\n"), 1);
        std::string nullString = "flag1";
        nullString += '\0';
        EXPECT_EQ(ETestFlagfromString(nullString), (uint64_t)-1);
    }

    // Test the maxEnumSize limit
    TEST_F(CpuidEnumTest, MaxEnumSizeLimit)
    {
        // Define an enum with maximum allowed flags
        std::stringstream   ss;
        std::vector<String> maxFlags(1024, "flag1");
        ss << maxFlags;
        // Should not crash and should have valid output
        EXPECT_FALSE(ss.str().empty());
    }

    // Test enum range values
    TEST_F(CpuidEnumTest, EnumRangeValues)
    {
        EXPECT_EQ(static_cast<Uint64>(ETestFlag::Min), 0);
        EXPECT_GT(static_cast<Uint64>(ETestFlag::Max),
                  static_cast<Uint64>(ETestFlag::Min));
    }

}} // namespace Au::Test
