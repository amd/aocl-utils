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
#include <thread>

#include "gtest/gtest.h"

namespace {

using namespace Au;
// Verify more number of processors than threads
TEST(ThreadPinning, check_thread_pinning)
{
    ThreadPinning tp;
    std::cout << "Thread Pinning\n" << std::endl;
    std::cout << "Core" << std::endl;
    tp.PinThreads({ 0, 1 }, 1); // spread
    std::cout << "Logical" << std::endl;
    tp.PinThreads({ 0, 1 }, 2); // spread
    std::cout << "Spread" << std::endl;
    tp.PinThreads({ 0, 1 }, 0); // spread
}
// Verify more number of threads than processors
TEST(ThreadPinning, threadsMoreThanProcessors)
{
    ThreadPinning tp;
    std::cout << "Thread Pinning\n" << std::endl;
    // Get the number of processors
    int              numProcs = std::thread::hardware_concurrency();
    std::vector<int> threadList(numProcs + 1);
    // initialize the thread list
    for (int i = 0; i < numProcs + 1; i++)
        threadList[i] = i;
    std::cout << "Core" << std::endl;
    tp.PinThreads(threadList, 1); // spread
    std::cout << "Logical" << std::endl;
    tp.PinThreads(threadList, 2); // spread
    std::cout << "Spread" << std::endl;
    tp.PinThreads(threadList, 0); // spread
}
// Verify hyperthreading enabled and more number of threads than processors
TEST(ThreadPinning, threadsEqualsProcessors)
{
    ThreadPinning tp;
    std::cout << "Thread Pinning\n" << std::endl;
    // Get the number of processors
    int              numProcs = std::thread::hardware_concurrency();
    std::vector<int> threadList(numProcs + 1);
    // initialize the thread list
    for (int i = 0; i < numProcs + 1; i++)
        threadList[i] = i;
    std::cout << "Core" << std::endl;
    tp.PinThreads(threadList, 1); // spread
    std::cout << "Logical" << std::endl;
    tp.PinThreads(threadList, 2); // spread
    std::cout << "Spread" << std::endl;
    tp.PinThreads(threadList, 0); // spread
}

} // namespace
