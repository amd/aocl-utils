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

#include "Au/Au.hh"

#include "Au/Error.hh"

#include "gtest/gtest.h"

using namespace Au;

namespace {

TEST(Error, check_noerror)
{
    auto& er = Au::NoError();

    EXPECT_EQ(er.code(), 0);

    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
}

TEST(Error, check_eexists)
{
    auto& er = Au::AlreadyExistsError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_einternal)
{
    auto& er = Au::InternalError();

    // EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_einvalidarg)
{
    auto& er = Au::InvalidArgumentError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    // EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_enotfound)
{
    auto& er = Au::NotFoundError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    // EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_enotimpl)
{
    auto& er = Au::NotImplementedError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    // EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_enotavail)
{
    auto& er = Au::NotAvailableError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    // EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

TEST(Error, check_unknown)
{
    auto& er = Au::UnknownError();

    EXPECT_FALSE(Au::InternalError().code() == er.code());
    EXPECT_FALSE(Au::AlreadyExistsError().code() == er.code());
    EXPECT_FALSE(Au::InvalidArgumentError().code() == er.code());
    EXPECT_FALSE(Au::NotFoundError().code() == er.code());
    EXPECT_FALSE(Au::NotImplementedError().code() == er.code());
    EXPECT_FALSE(Au::NotAvailableError().code() == er.code());
    // EXPECT_FALSE(Au::UnknownError().code() == er.code());
    EXPECT_FALSE(Au::NoError().code() == er.code());
}

} // namespace
