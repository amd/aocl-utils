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

#include "capi/integer_capi.h"
#include <gtest/gtest.h>

using namespace Au::Testing::Status;

TEST(IntegerCapiTest, TestAdd)
{
    char buffer[100];

    integer a      = au_testing_integer_create(10);
    integer b      = au_testing_integer_create(20);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_add(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_EQ(au_testing_integer_get_int(result), 30);
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestAddOverflow)
{
    char buffer[100];

    integer a      = au_testing_integer_create(INT64_MAX);
    integer b      = au_testing_integer_create(1);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_add(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Addition overflow");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestSub)
{
    char buffer[100];

    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_sub(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_integer_get_int(result), 10);
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestSubOverflow)
{
    char buffer[100];

    integer a      = au_testing_integer_create(INT64_MIN);
    integer b      = au_testing_integer_create(1);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_sub(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Subtraction overflow");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestDiv)
{
    char buffer[100];

    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_div(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_integer_get_int(result), 2);
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestDivByZero)
{
    char buffer[100];

    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(0);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_div(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Argument Out of Scope: Division by zero");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestMult)
{
    char buffer[100];

    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_mult(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_integer_get_int(result), 200);
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestMultOverflow)
{
    char buffer[100];

    integer a      = au_testing_integer_create(INT64_MAX);
    integer b      = au_testing_integer_create(2);
    integer result = au_testing_integer_create(0);
    status  s      = au_testing_integer_mult(a, b, result);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Internal Math Error: Multiplication overflow");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestEqual)
{
    char buffer[100];

    integer a = au_testing_integer_create(20);
    integer b = au_testing_integer_create(20);
    status  s = au_testing_integer_equal(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
}

TEST(IntegerCapiTest, TestNotEqual)
{
    char buffer[100];

    integer a = au_testing_integer_create(20);
    integer b = au_testing_integer_create(10);
    status  s = au_testing_integer_equal(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Unknown Error: Not equal");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
}

TEST(IntegerCapiTest, TestEqualInt)
{
    char buffer[100];

    integer a = au_testing_integer_create(20);
    status  s = au_testing_integer_equal_int(a, 20);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
}

TEST(IntegerCapiTest, TestNotEqualInt)
{
    char buffer[100];

    integer a = au_testing_integer_create(20);
    status  s = au_testing_integer_equal_int(a, 10);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_TRUE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "Unknown Error: Not equal");
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
}

TEST(IntegerCapiTest, TestAssign)
{
    char buffer[100];

    integer a = au_testing_integer_create(20);
    integer b = au_testing_integer_create(10);
    status  s = au_testing_integer_assign(a, b);
    au_testing_get_status_string(s, buffer, 100);
    EXPECT_FALSE(au_testing_status_is_error(s));
    EXPECT_STREQ(buffer, "No Error");
    EXPECT_EQ(au_testing_integer_get_int(a), 10);
    au_testing_destroy_status(s);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
}

TEST(IntegerCapiTest, TestDivOp)
{
    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_div_op(a, b);
    EXPECT_EQ(au_testing_integer_get_int(result), 2);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestAddOp)
{
    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_add_op(a, b);
    EXPECT_EQ(au_testing_integer_get_int(result), 30);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestSubOp)
{
    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_sub_op(a, b);
    EXPECT_EQ(au_testing_integer_get_int(result), 10);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

TEST(IntegerCapiTest, TestMultOp)
{
    integer a      = au_testing_integer_create(20);
    integer b      = au_testing_integer_create(10);
    integer result = au_testing_integer_mult_op(a, b);
    EXPECT_EQ(au_testing_integer_get_int(result), 200);
    au_testing_integer_destroy(a);
    au_testing_integer_destroy(b);
    au_testing_integer_destroy(result);
}

int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
