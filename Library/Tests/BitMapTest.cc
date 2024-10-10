/*
 * Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
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

#include "Au/BitMap.hh"

#include "gtest/gtest.h"

namespace {

using namespace Au;

TEST(BitMap, Construct)
{
    BitMap b(10);
    EXPECT_EQ(b.size(), 10);

    /* This should fail for 0 size in debug mode */
    if (AU_ENABLE_ASSERTIONS) {
        ASSERT_ANY_THROW({
            BitMap b1(0);
            EXPECT_EQ(b1.size(), 0);
            EXPECT_EQ(b1.count(), 0);
            b1.set(0);
        });
    }

    BitMap b2(1);
    EXPECT_EQ(b2.size(), 1);
    b2.set(0);
    EXPECT_EQ(b2.count(), 1);
}

TEST(BitMap, Zero)
{
    BitMap b(10);

    EXPECT_TRUE(b.isZero());

    b.set(1);
    EXPECT_FALSE(b.isZero());
}

TEST(BitMap, isFullApi)
{
    { /* Test for 1 bit */
        BitMap one(1);
        one.set();
        EXPECT_TRUE(one.isFull());
    }

    { /* Some randome length within a 'word' */
        BitMap eleven(11);
        eleven.set();
        EXPECT_TRUE(eleven.isFull());
    }

    { /* At word boundary */
        BitMap b1(64);
        b1.set();
        EXPECT_TRUE(b1.isFull());
    }

    { /* just crossing word boundary */
        BitMap b2(65);
        b2.set();
        EXPECT_TRUE(b2.isFull());
    }
}

TEST(BitMap, SetApi)
{
    /* Test set() API */
    BitMap b1(11);

    b1.set();
    EXPECT_EQ(b1.count(), 11);

    /* TODO: Add word-boundary tests
     * - width 63
     * - width 64 (one bit in next word)
     */
}

TEST(BitMap, SetPosApi)
{
    /* Test set(pos) API */
    BitMap b(12);

    b.set(0);
    EXPECT_EQ(b.count(), 1);
    b.set(1);
    EXPECT_EQ(b.count(), 2);
    b.set(9);
    EXPECT_EQ(b.count(), 3);
    /* out of bound, count should remain as before */
    if (AU_ENABLE_ASSERTIONS) {
        ASSERT_ANY_THROW(b.set(13));
        EXPECT_EQ(b.count(), 3);
    }

    BitMap b1(63);
    /* Around word boundary */
    b1.set(62);
    EXPECT_EQ(b1.count(), 1);

    /* out of bound, count() should remain as before */
    if (AU_ENABLE_ASSERTIONS) {
        ASSERT_ANY_THROW({
            b1.set(64);
            EXPECT_EQ(b1.count(), 1);
        });
    }
}

/* Test setting bits from another bit map, equal in size */
TEST(BitMap, SetBitMapApi)
{
    BitMap b(14);
    b.set(); /* set-all bits */
    EXPECT_EQ(b.count(), 14);
    EXPECT_TRUE(b.isFull());

    BitMap b1(14);
    b1.set(b);
    EXPECT_EQ(b1.count(), 14);
    EXPECT_TRUE(b1.isFull());
}

/*  word-boundary crossing tests
 * - consider 2 bitmaps a, b
 * - b.size() > a.size(), 'b' width 63, 64
 */
TEST(BitMap, SetBitMapApiGreater)
{
    {
        BitMap b(14);
        b.set(); /* set-all bits */
        EXPECT_TRUE(b.isFull());

        BitMap b1(12);
        b1.set(b);
        EXPECT_EQ(b1.count(), 12);
        EXPECT_TRUE(b1.isFull());
    } // namespace

    { /* One cross, other is exact or  */
        BitMap b(64), b1(63);
        b.set();

        b1.set(b);
        EXPECT_EQ(b1.count(), 63);
        EXPECT_TRUE(b1.isFull());
    }

    {
        BitMap b(66), b1(64);
        b.set();

        b1.set(b);
        EXPECT_EQ(b1.count(), 64);
        EXPECT_TRUE(b1.isFull());
    }
}

/* Add word-boundary crossing tests
 * - consider 2 bitmaps a, b
 * - b.size() < a.size(), 'b' width 63, 64
 */
TEST(BitMap, SetBitMapApiLesser)
{
    BitMap b(12);
    b.set(); /* set-all bits */
    EXPECT_TRUE(b.isFull());

    BitMap b1(14);
    b1.set(b);
    EXPECT_EQ(b1.count(), 12);
    EXPECT_FALSE(b1.isFull());
}

TEST(BitMap, CountApi)
{
    BitMap b(13);

    b.set(1);
    EXPECT_EQ(b.count(), 1);

    b.set(9);
    EXPECT_EQ(b.count(), 2);

    /* Crossing boundary */
    BitMap b1(65);
    EXPECT_EQ(b1.size(), 65);
    b1.set();
    EXPECT_EQ(b1.count(), 65);
    b1.clear();
    EXPECT_EQ(b1.count(), 0);
    b1.set(64);
    b1.set(1);
    EXPECT_EQ(b1.count(), 2);
}

TEST(BitMap, ClearApi)
{
    BitMap b(14);
    b.set(0);
    b.set(13);
    EXPECT_EQ(b.count(), 2);

    b.clear();
    EXPECT_EQ(b.count(), 0);
}

TEST(BitMap, ClearPosApi)
{
    BitMap b(14);
    b.set(0);
    b.set(13);
    EXPECT_EQ(b.count(), 2);

    b.clear(0);
    EXPECT_EQ(b.count(), 1);
    b.clear(13);
    EXPECT_EQ(b.count(), 0);
    EXPECT_TRUE(b.isZero());
}

TEST(BitMap, ClearBitMapApi)
{
    BitMap b(14), b1(14);
    b.set(); /* set-all bits */
    EXPECT_TRUE(b.isFull());

    b1.set(b); /* set all 14 bits */
    EXPECT_EQ(b1.count(), 14);
    EXPECT_TRUE(b1.isFull());

    b1.clear(b); /* clear all 14 bits */
    EXPECT_TRUE(b1.isZero());
    EXPECT_EQ(b1.count(), 0);
}

TEST(BitMap, ClearBitMapApiGreater)
{
    {
        BitMap b(14), b1(16);
        b.set(); /* set-all bits */
        EXPECT_TRUE(b.isFull());

        b1.set();    /* set all bits */
        b1.clear(b); /* clear 14 bits */
        EXPECT_TRUE(b1.isClear(0));
        EXPECT_TRUE(b1.isSet(14));
        EXPECT_TRUE(b1.isSet(15));
        EXPECT_EQ(b1.count(), 2);
    }

    {
        BitMap b(16), b1(14);
        b.set(); /* set-all bits */
        EXPECT_TRUE(b.isFull());

        b1.set();    /* set all bits */
        b1.clear(b); /* clear 14 bits */
        EXPECT_TRUE(b1.isZero());
        EXPECT_EQ(b1.count(), 0);
    }

    {
        BitMap c(63), c1(66);
        c.set(); /* set-all bits */
        EXPECT_TRUE(c.isFull());

        c1.set();    /* set all bits */
        c1.clear(c); /* clear 63 bits */
        EXPECT_TRUE(c1.isSet(63));
        EXPECT_TRUE(c1.isSet(64));
        EXPECT_TRUE(c1.isSet(65));
        EXPECT_EQ(c1.count(), 3);
    }

    {
        BitMap d(66), d1(63);
        d.set(); /* set-all bits */
        EXPECT_TRUE(d.isFull());

        d1.set();    /* set all bits */
        d1.clear(d); /* clear 14 bits */
        EXPECT_TRUE(d1.isZero());
    }
}

} // namespace
