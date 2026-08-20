/*
 * Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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

#include <memory>
#include <type_traits>
#include <utility>

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

// A minimal move-only type: models any StatusOr payload that owns a resource
// (e.g. a pimpl'd handle held via unique_ptr, like X86Cpu). Copyable is
// deliberately disabled so the tests below exercise the move path only.
namespace {
    struct MoveOnly
    {
        std::unique_ptr<int> p;
        explicit MoveOnly(int v)
            : p{ std::make_unique<int>(v) }
        {
        }
        MoveOnly(MoveOnly&&)                 = default;
        MoveOnly& operator=(MoveOnly&&)      = default;
        MoveOnly(const MoveOnly&)            = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
    };
} // namespace

// Regression: StatusOr wrapping a move-only T must itself be movable.
// User-declared copy ops on StatusOr previously suppressed the implicit move
// ctor/assignment, leaving StatusOr<MoveOnly> neither copyable nor movable, so
// the idiomatic factory patterns below failed to compile. The prior tests only
// moved the *contents* of a StatusOr, never the StatusOr object itself, which
// is why the gap went unnoticed until a move-only return type (X86Cpu) used it.
static_assert(std::is_move_constructible<StatusOr<MoveOnly>>(),
              "StatusOr<move-only> must be move-constructible");
static_assert(std::is_move_assignable<StatusOr<MoveOnly>>(),
              "StatusOr<move-only> must be move-assignable");
static_assert(!std::is_copy_constructible<StatusOr<MoveOnly>>(),
              "StatusOr<move-only> must not be copy-constructible");

TEST(StatusOr, MoveOnlyPayloadIsMovable)
{
    // Move-construct the wrapper (the "return sor;" forwarding pattern).
    StatusOr<MoveOnly> a{ MoveOnly{ 42 } };
    StatusOr<MoveOnly> b{ std::move(a) };
    ASSERT_TRUE(b.ok());
    EXPECT_EQ(*b.value().p, 42);

    // Move-assign the wrapper.
    StatusOr<MoveOnly> c{ MoveOnly{ 7 } };
    c = std::move(b);
    ASSERT_TRUE(c.ok());
    EXPECT_EQ(*c.value().p, 42);
}

// Mirrors how X86Cpu::buildFromCore is consumed: a factory returning
// StatusOr<move-only> by value, whose result is returned again through a named
// local (needs the wrapper's move ctor, not copy elision).
static StatusOr<MoveOnly>
makeMoveOnly(int v)
{
    StatusOr<MoveOnly> s{ MoveOnly{ v } };
    return s;
}

TEST(StatusOr, MoveOnlyFactoryForwarding)
{
    auto s = makeMoveOnly(99);
    ASSERT_TRUE(s.ok());
    EXPECT_EQ(*s.value().p, 99);
}

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
    static_assert(std::is_same<const int*,
                               decltype(std::declval<const StatusOr<int>&>()
                                            .operator->())>(),
                  "invalid qualifiers");
    static_assert(
        std::is_same<int*,
                     decltype(std::declval<StatusOr<int>&>().operator->())>(),
        "invalid qualifier");

    static_assert(std::is_same<const int*,
                               decltype(std::declval<const StatusOr<int>&&>()
                                            .operator->())>(),
                  "invalid qualifier");

    static_assert(
        std::is_same<int*,
                     decltype(std::declval<StatusOr<int>&&>().operator->())>(),
        "invalid qualifier");
}

} // namespace Au
