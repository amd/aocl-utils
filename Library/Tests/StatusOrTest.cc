/*
 * Copyright (C) 2022-2022, Advanced Micro Devices. All rights reserved.
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
 */

#include "Au/StatusOr.hh"

#include "type_traits"

#include "gtest/gtest.h"

namespace Au {

using namespace Au;

TEST(StatusOr, ElementType)
{
    static_assert(std::is_same<StatusOr<int>::value_type, int>(), "");
    static_assert(std::is_same<StatusOr<char>::value_type, char>(), "");
}

// Check operator arrow
TEST(StatusOr, ArrowOperator)
{
    const StatusOr<String> const_val("AOCL Utils");
    EXPECT_STREQ(std::string("AOCL Utils").c_str(), const_val->c_str());
}

TEST(StatusOr, Value)
{
    int v = 10;

    StatusOr<int> sts{ v };
    EXPECT_EQ(sts.value(), v);

    {
        std::unique_ptr<int> ten = std::make_unique<int>(10);
        StatusOr<int>        sts1{ std::move(*ten) };
        EXPECT_EQ(sts1.value(), 10);
    }

    {
        StatusOr<std::unique_ptr<int>> sts_unq_int = std::make_unique<int>(10);
        std::unique_ptr<int>           tenn = std::move(sts_unq_int.value());
        EXPECT_EQ(*tenn, 10);
    }
}

TEST(StatusOr, asdf) {}

TEST(StatusOr, WithQualifierStar)
{
    static_assert(
        std::is_same<const int&,
                     decltype(*std::declval<const StatusOr<int>&>())>(),
        "invalid qualifiers");

    static_assert(
        std::is_same<int&, decltype(*std::declval<StatusOr<int>&>())>(),
        "invalid qualifier");

    static_assert(
        std::is_same<const int&&,
                     decltype(*std::declval<const StatusOr<int>&&>())>(),
        "invalid qualifier");

    static_assert(
        std::is_same<int&&, decltype(*std::declval<StatusOr<int>&&>())>(),
        "invalid qualifier");
}

// Operator arrow with qualifiers
TEST(StatusOr, WithQualifierArrow)
{
    static_assert(
        std::is_same<
            const int*,
            decltype(std::declval<const StatusOr<int>&>().operator->())>(),
        "invalid qualifiers");
    static_assert(
        std::is_same<int*,
                     decltype(std::declval<StatusOr<int>&>().operator->())>(),
        "invalid qualifier");

    static_assert(
        std::is_same<
            const int*,
            decltype(std::declval<const StatusOr<int>&&>().operator->())>(),
        "invalid qualifier");

    static_assert(
        std::is_same<int*,
                     decltype(std::declval<StatusOr<int>&&>().operator->())>(),
        "invalid qualifier");
}

} // namespace Au
