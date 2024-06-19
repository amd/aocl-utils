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
#include "MockTest.hh"

namespace {

using namespace Au;
class MockCpuTopology : public CpuTopology
{
  public:
    MockCpuTopology() = default;
    // Implement the virtual functions
    void setActiveProcessors(int num) { active_processors = num; }
    void setPMap(std::vector<std::pair<KAFFINITY, int>> pMap)
    {
        processorMap = pMap;
    }
    void setCMap(std::vector<std::pair<KAFFINITY, int>> cMap)
    {
        cacheMap = cMap;
    }
    void setGMap(std::vector<std::pair<KAFFINITY, int>> gMap)
    {
        groupMap = gMap;
    }
};

INSTANTIATE_TEST_SUITE_P(ThreadPinningTests,
                         ThreadPinningTest,
                         ::testing::ValuesIn(testParametersCpuTopology));

TEST_P(ThreadPinningTest, affinityVectorTest)
{
    std::vector<std::pair<KAFFINITY, int>> pMap, cMap, gMap;
    std::vector<int> coreResult, LogicalResult, SpreadResult;
    int              activeProcessors, numberofThreads;
    String           name;

    std::tie(name,
             activeProcessors,
             numberofThreads,
             pMap,
             cMap,
             gMap,
             coreResult,
             LogicalResult,
             SpreadResult) = GetParam();
    MockCpuTopology mockCT;
    mockCT.setActiveProcessors(activeProcessors);
    mockCT.setPMap(pMap);
    mockCT.setCMap(cMap);
    mockCT.setGMap(gMap);

    auto             av = AffinityVector(mockCT);
    std::vector<int> processPinGroup(numberofThreads);
    processPinGroup.reserve(numberofThreads);

    std::cout << "Thread Pinning for " << name << std::endl;
    
    std::cout << "Core :" ;
    av.getAffinityVector(processPinGroup, 1); // core
    EXPECT_EQ(processPinGroup, coreResult);
    std::cout << " Pass" << std::endl;

    std::cout << "Logical :";
    av.getAffinityVector(processPinGroup, 2); // Logical
    EXPECT_EQ(processPinGroup, LogicalResult);
    std::cout << " Pass" << std::endl;
    
    std::cout << "Spread :" ;
    av.getAffinityVector(processPinGroup, 0); // Spread
    std::cout << " Pass" << std::endl;
    EXPECT_EQ(processPinGroup, SpreadResult);
}

} // namespace
