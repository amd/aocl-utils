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

#include "ThreadPinningTest.hh"
namespace {

TEST_F(PinThreadsTest, verifySpread)
{
    // Test spread strategy
    strategy = pinStrategy::SPREAD;
    verifyStrategy();
}

TEST_F(PinThreadsTest, verifyCore)
{
    // Test core strategy
    strategy = pinStrategy::CORE;
    verifyStrategy();
}

TEST_F(PinThreadsTest, verifyLogical)
{
    // Test logical strategy
    strategy = pinStrategy::LOGICAL;
    verifyStrategy();
}
TEST_F(PinThreadsTest, verifyCustom)
{
    // Test custom strategy
    std::vector<int> AffinityVector{};
    verifyStrategy(AffinityVector);
}
#if AU_BUILD_TYPE_DEBUG == 1
TEST_F(PinThreadsNegativeTest, verifyInvalidStrategy)
{
    // Test invalid strategy
    int strategy = 5;
    EXPECT_ANY_THROW(tp.pinThreads(thread_ids, strategy));
}

TEST_F(PinThreadsNegativeTest, verifyInvalidAffinity)
{
    // Test invalid affinity
    std::vector<int> AffinityVector(thread_ids.size(),
                                    std::thread::hardware_concurrency() + 1);
    EXPECT_ANY_THROW(tp.pinThreads(thread_ids, AffinityVector));
}

TEST_F(PinThreadsNegativeTest, verifyInvalidAffinitySize)
{
    // Test invalid affinity size
    std::vector<int> AffinityVector = { 0, 1, 2, 3, 4 };
    EXPECT_ANY_THROW(tp.pinThreads(thread_ids, AffinityVector));
}

TEST_F(PinThreadsNegativeTest, verifyNullAffinity)
{
    // Test null affinity
    std::vector<int> AffinityVector;
    EXPECT_ANY_THROW(tp.pinThreads(thread_ids, AffinityVector));
}

TEST_F(PinThreadsNegativeTest, verifyNullThreadList)
{
    // Test null thread list
    std::vector<pthread_t> thread_ids;
    EXPECT_ANY_THROW(tp.pinThreads(thread_ids, strategy));
}
#endif
} // namespace
