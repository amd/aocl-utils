/*
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

#include "Au/Buffer.hh"

#include "gtest/gtest.h"

namespace {

using namespace Au;
using namespace Au::Memory;

TEST(BufferView, Create)
{
    std::byte  buf[100];
    BufferView bp{ buf, sizeof(buf) };

    EXPECT_EQ(bp.ptr(), &buf[0]);
}

TEST(BufferView, Size)
{
    std::byte  buf[100];
    BufferView bp{ buf, sizeof(buf) };

    EXPECT_EQ(bp.size(), sizeof(buf));
}

TEST(BufferView, Erase)
{
    std::byte  buf[100];
    BufferView bp{ buf, sizeof(buf) };

    EXPECT_EQ(bp.size(), sizeof(buf));
}

TEST(Buffer, Create)
{
    Buffer b{ 100 };
    EXPECT_TRUE(b.ptr() == nullptr);
}

/* We dont support allocation yet */
TEST(Buffer, Allocate)
{
    Buffer b{ 100 };
    EXPECT_TRUE(b.ptr() == nullptr);

    EXPECT_TRUE(b.allocate(10, 8) != nullptr);
    EXPECT_TRUE(b.ptr() != nullptr);
    EXPECT_TRUE(b.size() == 10);
    EXPECT_TRUE(b.alignment() == 8);
}

TEST(BufferLocal, Create)
{
    constexpr Uint32 SIZE = 512;
    BufferLocal      bos{ SIZE };

    EXPECT_EQ(bos.size(), SIZE);

    bos.ptr()[10] = (std::byte)0xff;
    EXPECT_TRUE(bos.ptr()[10] == static_cast<std::byte>(0xff));
}

TEST(BufferLocal, Erase)
{
    constexpr Uint32 SIZE = 512;
    BufferLocal      bos{ SIZE };

    EXPECT_TRUE(bos.ptr()[10] == static_cast<std::byte>(0xff));
    bos.erase();
    EXPECT_TRUE(bos.ptr()[10] == static_cast<std::byte>(0x0));
}

} // namespace
