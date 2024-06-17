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
    
    tp.pinThreads(thread_ids, strategy);
    // with number of threads more than hardware concurrency
    EXPECT_TRUE(VerifyAffinity());
    // with number of threads equal to hardware concurrency
    num_threads = std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds1(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds1, strategy);
    EXPECT_TRUE(VerifyAffinity());
    
    // with number of threads less than hardware concurrency
    num_threads = std::thread::hardware_concurrency()
                  - rand() % std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds2(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds2, strategy);
    EXPECT_TRUE(VerifyAffinity());
    
}

TEST_F(PinThreadsTest, verifyCore)
{
    // Test core strategy
    strategy = pinStrategy::CORE;
    // with number of threads more than hardware concurrency
    tp.pinThreads(thread_ids, strategy);
    EXPECT_TRUE(VerifyAffinity());
    
    // with number of threads equal to hardware concurrency
    num_threads = std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds1(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds1, strategy);
    EXPECT_TRUE(VerifyAffinity());

    // with number of threads less than hardware concurrency
    num_threads = std::thread::hardware_concurrency()
                  - rand() % std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds2(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds2, strategy);
    EXPECT_TRUE(VerifyAffinity());
}

TEST_F(PinThreadsTest, verifyLogical)
{
    // Test logical strategy
    strategy = pinStrategy::LOGICAL;
    // with number of threads more than hardware concurrency    
    tp.pinThreads(thread_ids, strategy);
    EXPECT_TRUE(VerifyAffinity());

    // with number of threads equal to hardware concurrency
    num_threads = std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds1(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds1, strategy);
    EXPECT_TRUE(VerifyAffinity());
    
    // with number of threads less than hardware concurrency
    num_threads = std::thread::hardware_concurrency()
                  - rand() % std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds2(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds2, strategy);
    EXPECT_TRUE(VerifyAffinity());
}

TEST_F(PinThreadsTest, verifyCustom)
{
    // Test custom strategy
    std::vector<int> AffinityVector(num_threads);
    // initilize the affinity vector with random numbers less than hardware concurrency
    for (int i = 0; i < num_threads; i++)
    {
        AffinityVector.push_back(rand() % std::thread::hardware_concurrency());
    }
    tp.pinThreads(thread_ids, AffinityVector);
    EXPECT_TRUE(VerifyAffinity());

    // with number of threads equal to hardware concurrency
    num_threads = std::thread::hardware_concurrency();
    std::vector<pthread_t> threadIds1(thread_ids.begin(), thread_ids.begin() + num_threads);
    AffinityVector.resize(num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        AffinityVector.push_back(rand() % std::thread::hardware_concurrency());
    }
    tp.pinThreads(threadIds1, AffinityVector);
    EXPECT_TRUE(VerifyAffinity());
    
    // with number of threads less than hardware concurrency
    num_threads = std::thread::hardware_concurrency()
                  - rand() % std::thread::hardware_concurrency();
    AffinityVector.resize(num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        AffinityVector.push_back(rand() % std::thread::hardware_concurrency());
    }
    std::vector<pthread_t> threadIds2(thread_ids.begin(), thread_ids.begin() + num_threads);
    tp.pinThreads(threadIds2, AffinityVector);
    EXPECT_TRUE(VerifyAffinity());
}
} // namespace
