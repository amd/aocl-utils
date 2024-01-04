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

#include "Au/Status.hh"
#include "Au/Types.hh"

#include "gtest/gtest.h"

namespace {

using namespace Au;

TEST(Status, check_eok)
{
    Status sts = StatusOk();

    EXPECT_EQ(sts.code(), NoError().code());
}

TEST(Status, check_eexists)
{
    String s{ "here" };
    Status sts = StatusAlreadyExists(s);

    EXPECT_EQ(sts.code(), AlreadyExistsError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

TEST(Status, check_einvalid)
{
    String s{ "my_var is invalid" };
    Status sts = StatusInvalidArgument(s);

    EXPECT_EQ(sts.code(), InvalidArgumentError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

TEST(Status, check_enotfound)
{
    String s{ "exhausted" };
    Status sts = StatusNotFound(s);

    EXPECT_EQ(sts.code(), NotFoundError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

TEST(Status, check_enotavail)
{
    String s{ "list exhausted" };
    Status sts = StatusNotAvailable(s);

    EXPECT_EQ(sts.code(), NotAvailableError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

TEST(Status, check_enotimpl)
{
    String s{ "myimpl" };
    Status sts = StatusNotImplemented(s);

    EXPECT_EQ(sts.code(), NotImplementedError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

TEST(Status, check_eunknown)
{
    String s{ "Known" };
    Status sts = StatusUnknown(s);

    EXPECT_EQ(sts.code(), UnknownError().code());
    EXPECT_TRUE(sts.message().ends_with(s));
}

} // namespace
