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
#include <fstream>

namespace {

using namespace Au;
class MockCpuTopology : public CpuTopology
{
  public:
    MockCpuTopology() = default;

    void setActiveProcessors(int num) { active_processors = num; }
    void setPMap(std::vector<std::vector<std::pair<KAFFINITY, int>>> pMap)
    {
        processorMap = pMap;
    }
    void setCMap(std::vector<std::vector<std::pair<KAFFINITY, int>>> cMap)
    {
        cacheMap = cMap;
    }
    void setGMap(std::vector<std::pair<KAFFINITY, int>> gMap)
    {
        groupMap = gMap;
    }
};

INSTANTIATE_TEST_SUITE_P(
    ThreadPinningTests,
    ThreadPinningTest,
    ::testing::ValuesIn(std::move(testParametersCpuTopology)));

TEST_P(ThreadPinningTest, affinityVectorTest)
{
    std::vector<std::vector<std::pair<KAFFINITY, int>>> pMap, cMap;
    std::vector<std::pair<KAFFINITY, int>>              gMap;
    std::vector<int> coreResult, LogicalResult, SpreadResult;
    int              activeProcessors, numberofThreads;
    String           name;
    std::ofstream    logFile;

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
    // write to log file
    logFile.open("ThreadPinningMockTest.log", std::ios::app);
    logFile << "Thread Pinning for " << name << std::endl;
    logFile << "Topology : " << std::endl;
    logFile << "Active Processors : " << activeProcessors << std::endl;
    logFile << "Number of Threads : " << numberofThreads << std::endl;
    logFile << "Processor Map : " << std::endl;
    for (long unsigned int i = 0; i < pMap.size(); i++) {
        logFile << "Processor " << i << " : ";
        for (long unsigned j = 0; j < pMap[i].size(); j++) {
            logFile << pMap[i][j].first << "<--------->" << pMap[i][j].second
                    << " ";
        }
        logFile << std::endl;
    }
    for (long unsigned int i = 0; i < cMap.size(); i++) {
        logFile << "Cache " << i << " : ";
        for (long unsigned int j = 0; j < cMap[i].size(); j++) {
            logFile << cMap[i][j].first << "<--------->" << cMap[i][j].second
                    << " ";
        }
        logFile << std::endl;
    }
    for (long unsigned int i = 0; i < gMap.size(); i++) {
        logFile << "Group " << i << " : ";
        logFile << gMap[i].first << "'<---------->" << gMap[i].second
                << std::endl;
    }

    logFile << "Core :" << std::endl;
    std::cout << "Core :" << std::endl;
    av.getAffinityVector(processPinGroup, 1); // core
    for (int i = 0; i < numberofThreads; i++) {
        logFile << processPinGroup[i] << "<--------->" << coreResult[i]
                << std::endl;
    }
    logFile << std::endl;
    EXPECT_EQ(processPinGroup, coreResult);
    std::cout << " Pass" << std::endl;
    logFile << " Pass" << std::endl;
    logFile << "Logical :" << std::endl;
    std::cout << "Logical :";
    av.getAffinityVector(processPinGroup, 2); // Logical
    EXPECT_EQ(processPinGroup, LogicalResult);
    for (int i = 0; i < numberofThreads; i++) {
        logFile << processPinGroup[i] << "<--------->" << LogicalResult[i]
                << std::endl;
    }
    logFile << std::endl;
    std::cout << " Pass" << std::endl;
    logFile << " Pass" << std::endl;

    std::cout << "Spread :";
    logFile << "Spread :" << std::endl;
    av.getAffinityVector(processPinGroup, 0); // Spread
    for (int i = 0; i < numberofThreads; i++) {
        logFile << processPinGroup[i] << "<--------->" << SpreadResult[i]
                << std ::endl;
    }
    logFile << std::endl;
    std::cout << " Pass" << std::endl;
    logFile << " Pass" << std::endl;
    EXPECT_EQ(processPinGroup, SpreadResult);
}

} // namespace
