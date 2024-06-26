/*
# Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this softwareand associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright noticeand this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
*/

#include "Au/ThreadPinning.hh"
#include "Au/Assert.hh"
#include "Capi/au/macros.h"
#include "Capi/au/threadpinning.h"
#include <vector>

AUD_EXTERN_C_BEGIN

using namespace Au;

AUD_API_EXPORT
void
au_pin_threads_core(pthread_t* threadList, size_t threadListSize)
{
    AUD_ASSERT(threadList != nullptr, "Thread list is null");
    AUD_ASSERT(threadListSize > 0, "Thread list size is 0");

    ThreadPinning          tp;
    std::vector<pthread_t> threadListVec;
    for (size_t i = 0; i < threadListSize; i++) {
        threadListVec.push_back(threadList[i]);
    }
    tp.pinThreads(threadListVec, 1); // core
}

AUD_API_EXPORT
void
au_pin_threads_logical(pthread_t* threadList, size_t threadListSize)
{
    AUD_ASSERT(threadList != nullptr, "Thread list is null");
    AUD_ASSERT(threadListSize > 0, "Thread list size is 0");

    ThreadPinning          tp;
    std::vector<pthread_t> threadListVec;
    for (size_t i = 0; i < threadListSize; i++) {
        threadListVec.push_back(threadList[i]);
    }
    tp.pinThreads(threadListVec, 2); // logical
}

AUD_API_EXPORT
void
au_pin_threads_spread(pthread_t* threadList, size_t threadListSize)
{
    AUD_ASSERT(threadList != nullptr, "Thread list is null");
    AUD_ASSERT(threadListSize > 0, "Thread list size is 0");

    ThreadPinning          tp;
    std::vector<pthread_t> threadListVec;
    for (size_t i = 0; i < threadListSize; i++) {
        threadListVec.push_back(threadList[i]);
    }
    tp.pinThreads(threadListVec, 0); // spread
}

AUD_API_EXPORT
void
au_pin_threads_custom(pthread_t* threadList,
                      size_t     threadListSize,
                      int*       affinityVector,
                      size_t     affinityVectorSize)
{
    AUD_ASSERT(threadList != nullptr, "Thread list is null");
    AUD_ASSERT(threadListSize > 0, "Thread list size is 0");
    AUD_ASSERT(affinityVector != nullptr, "Affinity vector is null");
    AUD_ASSERT(affinityVectorSize > 0, "Affinity vector size is 0");
    AUD_ASSERT(affinityVectorSize == threadListSize,
               "Affinity vector size is not equal to thread list size");

    ThreadPinning          tp;
    std::vector<pthread_t> threadListVec;
    for (size_t i = 0; i < threadListSize; i++) {
        threadListVec.push_back(threadList[i]);
    }

    std::vector<int> affinityVectorVec;
    for (size_t i = 0; i < affinityVectorSize; i++) {
        affinityVectorVec.push_back(affinityVector[i]);
    }
    tp.pinThreads(threadListVec, affinityVectorVec);
}
AUD_EXTERN_C_END
