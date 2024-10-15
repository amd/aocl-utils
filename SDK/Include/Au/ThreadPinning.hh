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
#include "Au/Config.h"
#include <memory>
#include <vector>

// Define DWORD
typedef unsigned long DWORD;
#ifdef AU_TARGET_OS_IS_WINDOWS
#include <windows.h>
typedef HANDLE pthread_t;
#endif
namespace Au {
enum pinStrategy
{
    SPREAD,
    CORE,
    LOGICAL
};

class ThreadPinning
{
  public:
    ThreadPinning();
    ~ThreadPinning();
    /**
     * @brief          PinThreads
     *
     * @details        Pin Threads to a specific processor group.
     *
     * Strategy:
     *  0 - Spread
     *        The threads are pinned to the available processors in a cache
     * aware manner.
     *  1 - Core
     *        Pin the threads to the physical cores
     *  2 - Logical
     *        Processor Pin the threads to the logical processors
     *
     * @param[in]      threadList        ThreadIDs to pin
     *
     * @param[in]      pinStrategyIndex  0 - spread , 1 - Core, 2 - Logical
     * Processor
     *
     * @return         None
     */
    void pinThreads(std::vector<pthread_t> const& threadList,
                    int                           pinStrategyIndex);

    /**
     * @brief          pinThreads
     *
     * @details        Pin Threads to a specific processor group.
     *
     * Example: the theadList is [0, 1, 2, 3, 4, 5, 6, 7], and the
     * processPinGroup is [0, 1, 0, 1, 0, 1, 0, 1] The threads will be pinned to
     * the processor group following the below table
     *
     * | Thread List Index | Processor Group Index  |
     * |-------------------|------------------------|
     * | 0                 | 0                      |
     * | 1                 | 1                      |
     * | 2                 | 0                      |
     * | 3                 | 1                      |
     * | 4                 | 0                      |
     * | 5                 | 1                      |
     * | 6                 | 0                      |
     * | 7                 | 1                      |
     *
     *
     * @warning  "threadList" and "processPinGroup" should be of the same size
     *
     * @param[in]      threadList        ThreadIDs to pin
     *
     * @param[in]      processPinGroup   Processor Group to pin the threads
     *
     */
    void pinThreads(std::vector<pthread_t> const& threadList,
                    std::vector<int> const&       processPinGroup);

  private:
    class Impl;
    const Impl*           pImpl() const { return m_pimpl.get(); }
    Impl*                 pImpl() { return m_pimpl.get(); }
    std::unique_ptr<Impl> m_pimpl;
};
} // namespace Au
