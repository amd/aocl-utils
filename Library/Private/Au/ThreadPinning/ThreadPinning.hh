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
    /**
     * @brief           Calculate the offset
     *
     * @details         This function calculates the offset based on the group
     *                  and groupMap
     *
     * @param[in]       group       Group number
     *
     * @param[in]       groupMap    Map of groups
     *
     * @return          int
     */
    int calculateOffset(int group, const std::vector<CoreMask>& groupMap)
    {
        int offset = 0;
        if (group > 0) {
            for (int i = 0; i < group; i++) {
                offset += groupMap[i].second;
            }
        }
        return offset;
    }

    /**
     * @brief           Calculate the core number
     *
     * @details         This function calculates the core number based on the
     *                  bitMap
     *
     * @param[in]       pMap    ProcessorMap
     *
     * @return          int
     */
    int calculateCoreNum(CoreMask& pMap)
    {
        return log2(pMap.first & -1 * pMap.first);
    }

    /**
     * @brief           Update the map
     *
     * @details         This function updates the map by clearing the bit at
     *                  coreId
     *
     * @param[in]       pMap    ProcessorMap
     *
     * @param[in]       coreId  Core number
     *
     * @return          void
     */
    void updateMap(CoreMask& pMap, int coreId)
    {
        pMap.first &= ~(1ULL << coreId);
    }

    /**
     * @brief           Get the cache affinity map
     *
     * @details         This function creates a map that maps the given threads
     * to cache groups.
     *
     * @param[in]       procVect    Vector to store the spread affinity
     *
     * @param[out]      cacheMap    Map to store the cache affinity
     *
     * @return          void
     */
    void getCacheAffinityMap(int                              threadCount,
                             std::map<int, std::vector<int>>& cacheMap)
    {

        std::vector<int> procVect(threadCount);
        createVector(
            procVect, 0, threadCount - 1, 0, cpuInfo.cacheMap.size() - 1);
        for (size_t thread = 0; thread < procVect.size(); thread++) {
            int coreNum = procVect[thread];
            cacheMap[coreNum].push_back(thread);
        }
    }

    /**
     * @brief           CoreMapToCoreList
     *
     * @details         Convert the given bitmap to a list of core numbers
     *
     * @param[in]       processorMap    Bitmap of processors
     *
     * @param[out]      coreList        List of core numbers
     *
     * @return          void
     */
    void coreMapToCoreList(const std::vector<CoreMask>& processorMap,
                           std::vector<int>&            coreList)
    {
        for (auto pMap : processorMap) {
            while (pMap.first > 0) {
                coreList.push_back(
                    calculateCoreNum(pMap)
                    + calculateOffset(pMap.second, cpuInfo.groupMap));
                updateMap(pMap, calculateCoreNum(pMap));
            }
        }
    }

    /**
     * @brief           Check if the processorMap is reset
     *
     * @details         This function checks if the processorMap is reset
     *
     * @param[in]       processorMap    ProcessorMap to check
     *
     * @return          bool
     */
    bool isReset(std::vector<std::vector<CoreMask>>& processorMap)
    {
        return std::all_of(
            processorMap.begin(), processorMap.end(), [](auto& pMap) {
                return (
                    std::all_of(pMap.begin(), pMap.end(), [](const auto& p) {
                        return p.first == 0;
                    }));
            });
    }

    /**
     * @brief           Skip the mask
     *
     * @details         This return true if all the bits in the mask is 0
     *
     * @param[in]       pMap          corMask
     *
     * @param[in]       maskIndex     Index of the mask
     *
     * @param[in]       processorMap  ProcessorMap
     *
     * @return          bool
     */

    bool skipMask(CoreMask&                    pMap,
                  size_t&                      maskIndex,
                  const std::vector<CoreMask>& processorMap)
    {
        if (pMap.first == 0 && maskIndex == processorMap.size()) {
            maskIndex = 0;
            return true;
        } else if (pMap.first == 0 && maskIndex < processorMap.size()) {
            pMap = processorMap[maskIndex];
            maskIndex++;
        }
        return false;
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

  public:
    AffinityVector(const CpuTopology& Info = CpuTopology::get())
        : cpuInfo{ Info }
    {
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
        std::map<int, std::vector<int>> cacheMap;
        getCacheAffinityMap(procVect.size(), cacheMap);

        for (auto& cache : cacheMap) {
            auto             processorMap = cpuInfo.cacheMap[cache.first];
            std::vector<int> coreList;
            coreMapToCoreList(processorMap, coreList);

            int              threadCount = cache.second.size();
            std::vector<int> procVectPerCache(threadCount);
            createVector(
                procVectPerCache, 0, threadCount - 1, 0, coreList.size() - 1);

            /* Create the final affinity vector */
            int index = 0;
            for (auto thread : cache.second) {
                int item         = procVectPerCache[index++];
                int coreNum      = coreList[item];
                procVect[thread] = coreNum;
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

        std::vector<size_t>   maskIndex(processorMap.size(), 0);
        std::vector<CoreMask> pMap{};
        for (size_t coreId = 0; coreId < processorMap.size(); coreId++) {
            pMap.push_back(processorMap[coreId][maskIndex[coreId]++]);
        }

        while (threadId < threadCount) {
            if (isReset(processorMap))
                processorMap = cpuInfo.processorMap;

            for (size_t coreId = 0; coreId < processorMap.size(); coreId++) {
                if (skipMask(
                        pMap[coreId], maskIndex[coreId], processorMap[coreId]))
                    continue;

                procVect.push_back(
                    calculateCoreNum(pMap[coreId])
                    + calculateOffset(pMap[coreId].second, cpuInfo.groupMap));
                updateMap(pMap[coreId], calculateCoreNum(pMap[coreId]));

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
    void setAffinity(std::vector<pthread_t> const& threadList,
                     std::vector<int> const&       processorList)
    {
        for (size_t i = 0; i < threadList.size(); i++) {
            // Pin the thread to the processor
#ifdef __linux__
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(processorList[i], &cpuset);
            pthread_setaffinity_np(threadList[i], sizeof(cpu_set_t), &cpuset);
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
