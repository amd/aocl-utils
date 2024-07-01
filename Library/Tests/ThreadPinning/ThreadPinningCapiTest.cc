
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

#include "Capi/au/threadpinning.h"
#include "ThreadPinningTest.hh"
namespace {

TEST_F(PinThreadsTest, capiVerifySpread)
{
    // Test spread strategy
    strategy              = pinStrategy::SPREAD;
    pthread_t* threadList = &thread_ids[0];
    au_pin_threads_spread(threadList, thread_ids.size());
    EXPECT_TRUE(VerifyAffinity());
}

TEST_F(PinThreadsTest, capiVerifyCore)
{
    // Test core strategy
    strategy              = pinStrategy::CORE;
    pthread_t* threadList = &thread_ids[0];
    au_pin_threads_core(threadList, thread_ids.size());
    EXPECT_TRUE(VerifyAffinity());
}

TEST_F(PinThreadsTest, capiVerifyLogical)
{
    // Test logical strategy
    strategy              = pinStrategy::LOGICAL;
    pthread_t* threadList = &thread_ids[0];
    au_pin_threads_logical(threadList, thread_ids.size());
    EXPECT_TRUE(VerifyAffinity());
}

TEST_F(PinThreadsTest, capiVerifyCustom)
{
    // Test custom strategy
    pthread_t*       threadList = &thread_ids[0];
    std::vector<int> affinityVector{};

    // initilize the affinity vector with random numbers less than hardware
    // concurrency
    for (int i = 0; i < num_threads; i++) {
        affinityVector.push_back(rand()
                                 % (std::thread::hardware_concurrency() - 1));
    }
    au_pin_threads_custom(threadList,
                          thread_ids.size(),
                          &affinityVector[0],
                          affinityVector.size());
    EXPECT_TRUE(VerifyAffinity(affinityVector));
}

#if AU_BUILD_TYPE_DEBUG == 1
// Negative test case
TEST_F(PinThreadsNegativeTest, capiVerifyInvalidcorenumber)
{
    // Test custom strategy
    pthread_t*       threadList = &thread_ids[0];
    std::vector<int> affinityVector(thread_ids.size(),
                                    std::thread::hardware_concurrency() + 1);

    EXPECT_ANY_THROW(au_pin_threads_custom(threadList,
                                           thread_ids.size(),
                                           &affinityVector[0],
                                           affinityVector.size()));
}

TEST_F(PinThreadsNegativeTest, capiVerifyNullThreadList)
{
    // Test custom strategy
    pthread_t*       threadList = nullptr;
    std::vector<int> affinityVector{};

    // initilize the affinity vector with random numbers less than hardware
    // concurrency
    for (int i = 0; i < num_threads; i++) {
        affinityVector.push_back(rand()
                                 % (std::thread::hardware_concurrency() - 1));
    }
    EXPECT_ANY_THROW(au_pin_threads_custom(threadList,
                                           thread_ids.size(),
                                           &affinityVector[0],
                                           affinityVector.size()));
}
// Null affinity vector
TEST_F(PinThreadsNegativeTest, capiVerifyNullAffintyVector)
{
    // Test custom strategy
    pthread_t* threadList = &thread_ids[0];
    EXPECT_ANY_THROW(
        au_pin_threads_custom(threadList, thread_ids.size(), nullptr, 0));
}
// unequally sized thread list and affinity vector
TEST_F(PinThreadsNegativeTest, capiVerifyInvalidAffinityVector)
{
    // Test custom strategy
    pthread_t*       threadList = &thread_ids[0];
    std::vector<int> affinityVector{};

    // initilize the affinity vector with random numbers less than hardware
    // concurrency
    for (int i = 0; i < num_threads; i++) {
        affinityVector.push_back(rand()
                                 % (std::thread::hardware_concurrency() - 1));
    }
    EXPECT_ANY_THROW(au_pin_threads_custom(threadList,
                                           thread_ids.size(),
                                           &affinityVector[0],
                                           affinityVector.size() - 1));
}
#endif
} // namespace
