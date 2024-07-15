/*
 * Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Au/ThreadPinning.hh"
#include <iostream>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

using namespace Au;

// Function for the thread to execute
void
printThreadId(int id)
{
    std::cout << "Thread started with ID: " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void
checkThreadAffinity(std::thread& thread)
{
#ifdef _WIN32
    // Get the native handle of the thread
    HANDLE handle = (HANDLE)thread.native_handle();

    // Get thread group affinity
    GROUP_AFFINITY groupAffinity;
    BOOL           result = GetThreadGroupAffinity(handle, &groupAffinity);
    if (!result) {
        std::cerr << "Error reading thread affinity for thread  on Windows"
                  << std::endl;
    } else {
        std::cout << "groupAffinity.mask" << groupAffinity.Mask << std::endl;
    }
#else
    // Get the native handle of the thread
    pthread_t handle = thread.native_handle();
    // Create a CPU set and get the current affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int result = pthread_getaffinity_np(handle, sizeof(cpu_set_t), &cpuset);
    if (result != 0) {
        std::cerr << "Error reading thread affinity for thread  on Linux"
                  << std::endl;
    } else {
        std::cout << " affinity mask: " << cpuset.__bits[0] << std::endl;
    }
#endif
}

int
main()
{
    int                      n = 5; // Number of threads
    std::vector<std::thread> threads;

    // Create n threads
    for (int i = 0; i < n; ++i) {
        threads.push_back(std::thread(printThreadId, i));
    }

    ThreadPinning          tp;
    std::vector<pthread_t> threadList;
    for (int i = 0; i < n; i++) {
        threadList.push_back(threads[i].native_handle());
    }
    std::cout << "Pinning threads using custom pin strategy" << std::endl;
    // pin threads using the custom strategy.
    for (int i = 0; i < n; ++i) {
        tp.pinThreads(threadList, { 0, 5, 2, 10, 4 });
    }
    for (int i = 0; i < n; ++i) {
        checkThreadAffinity(threads[i]);
    }
    std::cout << "Pinning threads using spread pin strategy" << std::endl;
    // pin threads using the spread strategy
    for (size_t i = 0; i < n; i++) {
        tp.pinThreads(threadList, 0);
    }
    // Verify thread affinity for each thread
    for (int i = 0; i < n; ++i) {
        checkThreadAffinity(threads[i]);
    }
    std::cout << "Pinning threads using core pin strategy" << std::endl;
    // pin threads using the core strategy
    for (size_t i = 0; i < n; i++) {
        tp.pinThreads(threadList, 1);
    }
    // Verify thread affinity for each thread
    for (int i = 0; i < n; ++i) {
        checkThreadAffinity(threads[i]);
    }

    std::cout << "Pinning threads using logical pin strategy" << std::endl;
    // pin threads using the Logical strategy
    for (size_t i = 0; i < n; i++) {
        tp.pinThreads(threadList, 2);
    }
    // Verify thread affinity for each thread
    for (int i = 0; i < n; ++i) {
        checkThreadAffinity(threads[i]);
    }
    // Join all threads
    for (auto& th : threads) {
        th.join();
    }

    return 0;
}
