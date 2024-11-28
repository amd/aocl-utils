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

#include "capi/calculator_capi.h"
#include <gtest/gtest.h>

using namespace Au::Testing::Status;

TEST(CalculatorCapiTest, TestAdd)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(10);
    calculator b      = au_testing_calculator_create(20);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_add(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_EQ(au_testing_calculator_get_int(result), 30);
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestAddOverflow)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(INT64_MAX);
    calculator b      = au_testing_calculator_create(1);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_add(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Addition overflow");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestSub)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_sub(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_calculator_get_int(result), 10);
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestSubOverflow)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(INT64_MIN);
    calculator b      = au_testing_calculator_create(1);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_sub(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Subtraction overflow");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestDiv)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_div(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_calculator_get_int(result), 2);
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestDivByZero)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(0);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_div(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Argument Out of Scope: Division by zero");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestMult)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_mult(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_calculator_get_int(result), 200);
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestMultOverflow)
{
    char buffer[100];

    calculator a      = au_testing_calculator_create(INT64_MAX);
    calculator b      = au_testing_calculator_create(2);
    calculator result = au_testing_calculator_create(0);
    status     s      = au_testing_calculator_mult(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Multiplication overflow");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestEqual)
{
    char buffer[100];

    calculator a = au_testing_calculator_create(20);
    calculator b = au_testing_calculator_create(20);
    status     s = au_testing_calculator_equal(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
}

TEST(CalculatorCapiTest, TestNotEqual)
{
    char buffer[100];

    calculator a = au_testing_calculator_create(20);
    calculator b = au_testing_calculator_create(10);
    status     s = au_testing_calculator_equal(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Unknown Error: Not equal");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
}

TEST(CalculatorCapiTest, TestEqualInt)
{
    char buffer[100];

    calculator a = au_testing_calculator_create(20);
    status     s = au_testing_calculator_equal_int(a, 20);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
}

TEST(CalculatorCapiTest, TestNotEqualInt)
{
    char buffer[100];

    calculator a = au_testing_calculator_create(20);
    status     s = au_testing_calculator_equal_int(a, 10);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Unknown Error: Not equal");
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
}

TEST(CalculatorCapiTest, TestAssign)
{
    char buffer[100];

    calculator a = au_testing_calculator_create(20);
    calculator b = au_testing_calculator_create(10);
    status     s = au_testing_calculator_assign(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_calculator_get_int(a), 10);
    au_testing_destroy_status(s);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
}

TEST(CalculatorCapiTest, TestDivOp)
{
    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_div_op(a, b);
    EXPECT_EQ(au_testing_calculator_get_int(result), 2);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestAddOp)
{
    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_add_op(a, b);
    EXPECT_EQ(au_testing_calculator_get_int(result), 30);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestSubOp)
{
    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_sub_op(a, b);
    EXPECT_EQ(au_testing_calculator_get_int(result), 10);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

TEST(CalculatorCapiTest, TestMultOp)
{
    calculator a      = au_testing_calculator_create(20);
    calculator b      = au_testing_calculator_create(10);
    calculator result = au_testing_calculator_mult_op(a, b);
    EXPECT_EQ(au_testing_calculator_get_int(result), 200);
    au_testing_calculator_destroy(a);
    au_testing_calculator_destroy(b);
    au_testing_calculator_destroy(result);
}

int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
