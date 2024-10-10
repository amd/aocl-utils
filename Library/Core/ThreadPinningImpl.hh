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
#pragma once

#include "Au/ThreadPinning.hh"
#include "Au/ThreadPinning/ThreadPinning.hh"
#include <thread>

namespace Au {
class ThreadPinning::Impl : public AffinityVector
{
  public:
    /**
     * @brief          PinThreads
     *
     * @details        Pin Threads to a specific processor group.
     *
     * @param[in]      threadList        ThreadIds to pin
     *
     * @param[in]      pinStrategyIndex  0 - spread , 1 - Core, 2 - Logical
     * Processor
     *
     * @return         None
     */
    void pinThreads(std::vector<pthread_t> threadList, int pinStrategyIndex);
    /**
     * @brief          pinThreads
     *
     * @details        Pin Threads to a specific processor group.
     *
     * @param[in]      threadList        ThreadIds to pin
     *
     * @param[in]      processPinGroup   Processor Group to pin the threads
     *
     */
    void pinThreads(std::vector<pthread_t>  threadList,
                    std::vector<int> const& processPinGroup);
};
} // namespace Au
