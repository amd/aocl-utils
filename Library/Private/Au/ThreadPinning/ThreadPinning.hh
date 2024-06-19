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
#pragma once
#include <algorithm>
#include <map>
#include <numeric>
#include <vector>

#ifdef __linux__
#include "Au/ThreadPinning/Linux/CpuTopology.hh"
#else
#include "Au/ThreadPinning/Windows/CpuTopology.hh"
#include <windows.h>
#endif

#include <Au/ThreadPinning.hh>

namespace Au {
class AffinityVector
{
  protected:
    const CpuTopology& cpuInfo;
 
  private:
    int calculateOffset(int group, const std::vector<std::pair<KAFFINITY, int>>& groupMap) {
      int offset = 0;
      if (group > 0) {
          for (int i = 0; i < group; i++) {
              offset += groupMap[i].second;
          }
      }
      return offset;
    }

   int calculateCoreNum(std::pair<KAFFINITY, int>& pMap) {
         return log2(pMap.first & -1 * pMap.first);
    }

    void updateMap(std::pair<KAFFINITY, int>& pMap, int coreId) {
        pMap.first &= ~(1ULL << coreId);
    }

  public:
    AffinityVector(const CpuTopology& Info = CpuTopology::get())
        : cpuInfo{ Info }
    {
    }

    /**
     * @brief           Create a vector
     *
     * @details         This function creates a vector that equally distributes
     *                  the threads among the processors.
     *
     * @param[out]      procVect    Vector to store the spread affinity
     *
     * @param[in]       tListStart  Start index of threadList
     *
     * @param[in]       tListEnd    End index of threadList
     *
     * @param[in]       pListStart  Start index of processorList
     *
     * @param[in]       pListEnd    End index of processorList
     *
     * @return          void
     */
    void createVector(std::vector<int>& procVect,
                      int               tListStart,
                      int               tListEnd,
                      int               pListStart,
                      int               pListEnd)
    {
        if (tListStart > tListEnd
            || pListStart > pListEnd) // Base case: no elements left in
                                      // threadList or processorList
            return;
        if (tListStart
            == tListEnd) // Base case: There are more processors than threads
        {
            // Pin each thread as spread out as possible
            procVect[tListStart] = pListStart;
            return;
        }
        if (pListStart
            == pListEnd) // Base case: There are more threads than processors
        {
            // Equally distribute the threads among the processors to the extend
            // possible
            for (int index = tListStart; index <= tListEnd; index++)
                procVect[index] = pListStart;
            return;
        }
        int tListMiddle = (tListEnd - tListStart) / 2
                          + tListStart; // Middle index of threadList
        int pListMiddle = (pListEnd - pListStart) / 2
                          + pListStart; // Middle index of processorList

        // Recursively process the left and right halves of the threadList and
        // processorList
        createVector(procVect,
                     tListStart,
                     tListMiddle,
                     pListStart,
                     pListMiddle); // Left half
        createVector(procVect,
                     tListMiddle + 1,
                     tListEnd,
                     pListMiddle + 1,
                     pListEnd); // Right half
    }

    /**
     * @brief           Get the spread affinity vector
     *
     * @details         This function creates a vector that maps the threads to
     *                  cache groups.
     *
     * @param[out]      procVect    Vector to store the spread affinity
     *
     * @return          void
     */
    void getSpreadAffinityVectory(std::vector<int>& procVect)
    {
        // Create a vector that maps threads to L3 cache.
        std::vector<int> cacheVect(procVect.size());
        createVector(cacheVect, 0, cacheVect.size() - 1, 0, cpuInfo.cacheMap.size() - 1);

        // Create a map with L3 cache number as key
        // and list of threads to be spread on cores sharing the cache as value.
        std::map<int, std::vector<int>> cacheMap;
        for (size_t i = 0; i < cacheVect.size(); i++) {
            cacheMap[cacheVect[i]].push_back(i);
        }

        // Create a vector of core numbers sharing each cache.
        for (auto& cache : cacheMap) {
            std::vector<int> coreList;
            auto             pMap = cpuInfo.cacheMap[cache.first];

            // Convert pMap to coreList
            while (pMap.first > 0) { // While there are cores in the cache mask
                int offset = calculateOffset(pMap.second, cpuInfo.groupMap);
                int coreId = calculateCoreNum(pMap);
                coreList.push_back(coreId + offset);
                updateMap(pMap, coreId);
            }

            // Spread the percache threads to corelist
            int threadCount = cache.second.size();
            std::vector<int> Vect(threadCount);
            createVector(Vect, 0, threadCount - 1, 0, coreList.size() - 1);
            
            // create The processor vector
            int index = 0;
            for (auto thread : cache.second) {
                procVect[thread] = coreList[Vect[index++]];
            }
        }
    }

    /**
     * @brief           Get the core affinity vector
     *
     * @details         This function creates a vector that maps the threads to
     *                  physical cores.
     *
     * @param[out]      procVect    Vector to store the core affinity
     *
     * @return          void
     */
    void getCoreAffinityVector(std::vector<int>& procVect)
    {
        int threadCount = procVect.size();
        procVect.clear();

        auto processorMap = cpuInfo.processorMap;
        int  threadId     = 0;

        while (threadId < threadCount) {
            // If all the first elements of processorMap are 0, reset
            // processorMap
            if (std::all_of(processorMap.begin(),processorMap.end(),[](auto& p) { return p.first == 0; }))
                processorMap = cpuInfo.processorMap;

            for (size_t coreId = 0; coreId < processorMap.size(); coreId++) {
                // Skip if all the cores in the group are used
                if (processorMap[coreId].first == 0)
                    continue;

                int offset = calculateOffset(processorMap[coreId].second, cpuInfo.groupMap);
                int coreNum = calculateCoreNum(processorMap[coreId]);
                procVect.push_back(coreNum + offset);
                updateMap(processorMap[coreId], coreNum);
                
                threadId++;
                if (threadId == threadCount)
                    break;
            }
        }
    }

    /**
     * @brief           Get the logical core affinity vector
     *
     * @details         This function creates a vector that maps the threads to
     *                  logical cores.
     *
     * @param[out]      procVect    Vector to store the logical core affinity
     *
     * @return          void
     */
    void getLogicalAffinityVector(std::vector<int>& procVect)
    {
        int threadCount = procVect.size();

        procVect.clear();
        for (int threadId = 0; threadId < threadCount; threadId++) {
            procVect.push_back(threadId % cpuInfo.active_processors);
        }
    }

    /**
     * @brief          getAffinityVector
     *
     * @details        Get the affinity vector based on the pinning strategy
     *
     * @param[in]      pinStrategyIndex  0 - spread , 1 - Core, 2 - Logical
     *                 Processor
     *
     * @param[out]     processPinGroup   Vector to store the affinity vector
     *
     * @return         None
     */
    void getAffinityVector(std::vector<int>& processPinGroup,
                           int               pinStrategyIndex)
    {
        switch (pinStrategyIndex) {
            case pinStrategy::SPREAD:
                getSpreadAffinityVectory(processPinGroup);
                break;
            case pinStrategy::CORE:
                getCoreAffinityVector(processPinGroup);
                break;
            case pinStrategy::LOGICAL:
                getLogicalAffinityVector(processPinGroup);
                break;
            default:
                break;
        }
    }

    /** @brief         setAffinity
     *
     * @details       Pin Threads to a specific processor group.
     *
     * @param[in]     threadList        ThreadIds to pin
     *
     * @param[in]     processorList    List of processors to pin the threads
     */
    void setAffinity(std::vector<DWORD> const& threadList,
                     std::vector<int> const&   processorList)
    {
        for (size_t i = 0; i < threadList.size(); i++) {
            // Pin the thread to the processor
#ifdef __linux__
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(processorList[i], &cpuset);
            sched_setaffinity(threadList[i], sizeof(cpu_set_t), &cpuset);
#else
            GROUP_AFFINITY groupAffinity;
            ZeroMemory(&groupAffinity, sizeof(GROUP_AFFINITY));
            // calculate the group and mask from the processor number and
            // cpuInfo.groupMap
            int core  = 0;
            int group = 0;
            for (auto gMap : cpuInfo.groupMap) {
                if (processorList[i] > core + gMap.second) {
                    core = core + gMap.second;
                    group++;
                } else {
                    break;
                }
            }

            groupAffinity.Mask  = 1ull << (processorList[i] - core);
            groupAffinity.Group = group;
            HANDLE hThread      = (HANDLE)threadList[i];
            auto   result =
                SetThreadGroupAffinity(hThread, &groupAffinity, nullptr);
            if (!result) {
                std::cout << "SetThreadGroupAffinity Failed\n";
            }
#endif
        }
    }
};
} // namespace Au
