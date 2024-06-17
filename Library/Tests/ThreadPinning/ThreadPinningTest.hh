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
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <climits> // for CHAR_BIT
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
  protected:
    void SetUp() override
    {
        
        num_threads = std::thread::hardware_concurrency()
                  + rand() % std::thread::hardware_concurrency();
        createThreads();
        for(int i = 0; i < num_threads; ++i)
        {
            pthread_t handle = threads[i].native_handle();
            thread_ids.push_back(handle);
        }
    }

    void TearDown() override
    {
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    int                      num_threads;
    std::vector<std::thread> threads;
    std::vector<pthread_t>   thread_ids;
    pinStrategy              strategy;
    ThreadPinning    tp;

  private:
    static void printThreadId(int id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Helper function to create threads and perform a simple task
    void createThreads()
    {
        for (int i = 0; i < num_threads; ++i) {
            threads.push_back(std::thread(printThreadId, i));
        }
    }

#ifdef _WIN32
    int getCoreAffinity(HANDLE threadHandle)
    {
        GROUP_AFFINITY groupAffinity;
        BOOL result = GetThreadGroupAffinity(threadHandle, &groupAffinity);
        if (!result) {
           std::cerr << "Error reading thread affinity for thread " << std::endl;
        } else {
            for (size_t core = 0; core < sizeof(groupAffinity.Mask) * CHAR_BIT; ++core) {
                if (groupAffinity.Mask & (1 << core)) {
                    return core + groupAffinity.Group * GROUP_SIZE;
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
  public:
    bool VerifyAffinity()
    {
               

        AffinityVector   av;
        // Call the getAffinityVector API to get the affinity vector
        std::vector<int> affinityVector(num_threads);
        affinityVector.reserve(num_threads);
        av.getAffinityVector(affinityVector, strategy);

        // Verify the core affinity of each thread matches the expected core
        for (int i = 0; i < num_threads; ++i) {
            int expectedCore = affinityVector[i];
            int actualCore   = getCoreAffinity(thread_ids[i]);
            if (expectedCore != actualCore)
                return false;
        }
    return true;
    }
};
} // namespace
