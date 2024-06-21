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
#include "Au/ThreadPinning.hh"
#include "Au/ThreadPinning/ThreadPinning.hh"
#include <climits> // for CHAR_BIT
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif
namespace {
using namespace Au;

// Test fixture class
class PinThreadsTest : public ::testing::Test
{
  public:
    PinThreadsTest()
        : num_threads(0)
        , thread_ids()
        , strategy(pinStrategy::SPREAD)
        , threads()
        , tp(){};

  protected:
    int                      num_threads;
    std::vector<pthread_t>   thread_ids;
    pinStrategy              strategy;
    std::vector<std::thread> threads;
    ThreadPinning            tp;

    /**
     * @brief           SetUp
     *
     * @details         Set up the test fixture by creating threads for the test
     *
     * @param           void
     *
     * @return          void
     */
    void SetUp() override
    {

        num_threads = std::thread::hardware_concurrency()
                      + rand() % (std::thread::hardware_concurrency() - 1);
        createThreads();
        for (int i = 0; i < num_threads; ++i) {
            pthread_t handle = threads[i].native_handle();
            thread_ids.push_back(handle);
        }
    }

    /**
     * @brief           TearDown
     *
     * @details         Tear down the test fixture by joining the threads
     *
     * @param           void
     *
     * @return          void
     */
    void TearDown() override
    {
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

  private:
    /**
     * @brief           printThreadId
     *
     * @details         Thread function
     *
     * @return          void
     */
    static void printThreadId()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    /**
     * @brief           createThreads
     *
     * @details         Create threads.
     *
     * @param           void
     *
     * @return          void
     */
    void createThreads()
    {
        for (int i = 0; i < num_threads; ++i) {
            threads.push_back(std::thread(printThreadId));
        }
    }

    /**
     * @brief           getCoreAffinity
     *
     * @details         Get the core affinity of the thread
     *
     * @param           threadHandle
     *
     * @return          int
     */
#ifdef _WIN32
    int getCoreAffinity(HANDLE threadHandle)
    {
        GROUP_AFFINITY groupAffinity;
        BOOL result = GetThreadGroupAffinity(threadHandle, &groupAffinity);
        if (!result) {
            std::cerr << "Error reading thread affinity for thread "
                      << std::endl;
        } else {
            for (size_t core = 0; core < sizeof(groupAffinity.Mask) * CHAR_BIT;
                 ++core) {
                if (groupAffinity.Mask & (1 << core)) {
                    int offset = 0;
                    for (int i = 0; i < groupAffinity.Group; ++i) {
                        offset = offset + CpuTopology::get().groupMap[i].second;
                    }
                    return core + offset;
                }
            }
        }
        return -1;
    }
#else
    int getCoreAffinity(pthread_t threadHandle)
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        pthread_getaffinity_np(threadHandle, sizeof(cpu_set_t), &cpuset);

        for (int core = 0; core < CPU_SETSIZE; ++core) {
            if (CPU_ISSET(core, &cpuset)) {
                return core;
            }
        }
        return -1;
    }
#endif

    /**
     * @brief           VerifyAffinity
     *
     * @details         Verify the core affinity of each thread matches the
     * expected core
     *
     * @param           affinityVector
     *
     * @return          bool
     */
    bool VerifyAffinity(std::vector<int> affinityVector)
    {

        for (int i = 0; i < num_threads; ++i) {
            int expectedCore = affinityVector[i];
            int actualCore   = getCoreAffinity(thread_ids[i]);
            if (expectedCore != actualCore)
                return false;
        }
        return true;
    }

    /**
     * @brief           VerifyAffinity
     *
     * @details         Verify the core affinity of each thread matches the
     * expected core
     *
     * @return          bool
     */
    bool VerifyAffinity()
    {

        AffinityVector   av;
        std::vector<int> affinityVector(num_threads);
        av.getAffinityVector(affinityVector, strategy);

        for (int i = 0; i < num_threads; ++i) {
            int expectedCore = affinityVector[i];
            int actualCore   = getCoreAffinity(thread_ids[i]);
            if (expectedCore != actualCore)
                return false;
        }
        return true;
    }

  public:
    /**
     * @brief           verifyStrategy
     *
     * @details         Verify the pinning strategy
     *
     * @param           void
     *
     * @return          void
     */
    void verifyStrategy()
    {
        // with number of threads more than hardware concurrency
        tp.pinThreads(thread_ids, strategy);
        EXPECT_TRUE(VerifyAffinity());

        // with number of threads equal to hardware concurrency
        num_threads = std::thread::hardware_concurrency();
        std::vector<pthread_t> threadIds1(thread_ids.begin(),
                                          thread_ids.begin() + num_threads);
        tp.pinThreads(threadIds1, strategy);
        EXPECT_TRUE(VerifyAffinity());

        // with number of threads less than hardware concurrency
        num_threads = std::thread::hardware_concurrency()
                      - (rand() % std::thread::hardware_concurrency() - 1);
        std::vector<pthread_t> threadIds2(thread_ids.begin(),
                                          thread_ids.begin() + num_threads);
        tp.pinThreads(threadIds2, strategy);
        EXPECT_TRUE(VerifyAffinity());
    }

    /**
     * @brief           verifyStrategy
     *
     * @details         Verify the pinning strategy
     *
     * @param           affinityVector
     *
     * @return          void
     */
    void verifyStrategy(std::vector<int>& affinityVector)
    {
        // initilize the affinity vector with random numbers less than hardware
        // concurrency
        for (int i = 0; i < num_threads; i++) {
            affinityVector.push_back(
                rand() % (std::thread::hardware_concurrency() - 1));
        }
        // with number of threads more than hardware concurrency
        tp.pinThreads(thread_ids, affinityVector);
        EXPECT_TRUE(VerifyAffinity(affinityVector));

        // with number of threads equal to hardware concurrency
        num_threads = std::thread::hardware_concurrency();
        affinityVector.clear();
        for (int i = 0; i < num_threads; i++) {
            affinityVector.push_back(
                rand() % (std::thread::hardware_concurrency() - 1));
        }
        std::vector<pthread_t> threadIds1(thread_ids.begin(),
                                          thread_ids.begin() + num_threads);
        tp.pinThreads(threadIds1, affinityVector);
        EXPECT_TRUE(VerifyAffinity(affinityVector));

        // with number of threads less than hardware concurrency
        num_threads = std::thread::hardware_concurrency()
                      - (rand() % std::thread::hardware_concurrency() - 1);
        affinityVector.clear();
        for (int i = 0; i < num_threads; i++) {
            affinityVector.push_back(
                rand() % (std::thread::hardware_concurrency() - 1));
        }
        std::vector<pthread_t> threadIds2(thread_ids.begin(),
                                          thread_ids.begin() + num_threads);
        tp.pinThreads(threadIds2, affinityVector);
        EXPECT_TRUE(VerifyAffinity(affinityVector));
    }
};
} // namespace
