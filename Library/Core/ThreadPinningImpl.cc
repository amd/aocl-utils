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
#include "ThreadPinningImpl.hh"
#include "Au/Assert.hh"
namespace Au {

void
ThreadPinning::Impl::pinThreads(std::vector<pthread_t> threadList,
                                int                    pinStrategyIndex)
{
    AUD_ASSERT(threadList.size() > 0, "Thread list is empty");
    if (threadList.size() == 0) {
        return;
    }
    AUD_ASSERT(pinStrategyIndex >= 0 && pinStrategyIndex < 3,
               "Invalid pin strategy index");
    std::vector<int> processPinGroup(threadList.size());
    if (threadList.size() == 0) {
        return;
    }
    processPinGroup.reserve(threadList.size());
    // Get the processor group to pin the threads
    getAffinityVector(processPinGroup, pinStrategyIndex);
    // Pin the threads to the processor group in the processPinGroup
    if (processPinGroup.size() == 0) {
        return;
    }
    pinThreads(threadList, processPinGroup);
}

void
ThreadPinning::Impl::pinThreads(std::vector<pthread_t>  threadList,
                                std::vector<int> const& processPinGroup)
{
    AUD_ASSERT(threadList.size() > 0, "Thread list is empty");
    AUD_ASSERT(processPinGroup.size() > 0, "Processor group is empty");
    AUD_ASSERT(threadList.size() == processPinGroup.size(),
               "Thread list and processor group size mismatch");
    if (threadList.size() == 0 || threadList.size() != processPinGroup.size()) {
        return;
    }
    // Pin the threads to the processor group in the processPinGroup
    setAffinity(threadList, processPinGroup);
}

} // namespace Au
