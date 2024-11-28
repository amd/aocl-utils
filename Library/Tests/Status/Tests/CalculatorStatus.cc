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

#include "Calculator.hh"
#include <gtest/gtest.h>

using namespace Au::Testing::Status;

// Sample test case
TEST(CalculatorTest, TestAdd)
{
    Calculator a(10);
    Calculator b(20);
    Status     s = a.add(b, a);
    EXPECT_EQ(a.getInt(), 30);
}

TEST(CalculatorTest, TestAddOverflow)
{
    Calculator a(INT64_MAX);
    Calculator b(1);
    Calculator result;
    Status     s = a.add(b, result);
    EXPECT_TRUE(s.ok() == false);
    EXPECT_EQ(s.message(), "Internal Math Error: Addition overflow");
}

TEST(CalculatorTest, TestSub)
{
    Calculator a(20);
    Calculator b(10);
    Calculator result;
    Status     s = a.sub(b, result);
    EXPECT_EQ(result.getInt(), 10);
}

TEST(CalculatorTest, TestSubOverflow)
{
    Calculator a(INT64_MIN);
    Calculator b(1);
    Calculator result;
    Status     s = a.sub(b, result);
    EXPECT_TRUE(s.ok() == false);
    EXPECT_EQ(s.message(), "Internal Math Error: Subtraction overflow");
}

TEST(CalculatorTest, TestDiv)
{
    Calculator a(20);
    Calculator b(10);
    Calculator result;
    Status     s = a.div(b, result);
    EXPECT_EQ(result.getInt(), 2);
}

TEST(CalculatorTest, TestDivByZero)
{
    Calculator a(20);
    Calculator b(0);
    Calculator result;
    Status     s = a.div(b, result);
    EXPECT_TRUE(s.ok() == false);
    EXPECT_EQ(s.message(), "Argument Out of Scope: Division by zero");
}

TEST(CalculatorTest, TestMult)
{
    Calculator a(20);
    Calculator b(10);
    Calculator result;
    Status     s = a.mult(b, result);
    EXPECT_EQ(result.getInt(), 200);
}

TEST(CalculatorTest, TestMultOverflow)
{
    Calculator a(INT64_MAX);
    Calculator b(2);
    Calculator result;
    Status     s = a.mult(b, result);
    EXPECT_TRUE(s.ok() == false);
    EXPECT_EQ(s.message(), "Internal Math Error: Multiplication overflow");
}

int
main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
