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
#include <thread>
#include <vector>

#ifdef __linux__
#include "Au/ThreadPinning/Linux/CpuTopology.hh"
#else
#include "Au/ThreadPinning/Windows/CpuTopology.hh"
#include <windows.h>
#endif

#include "Au/Assert.hh"
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
     * It is a map of cache number to a vector of thread indexes
     * Example:
     * cacheMap[0] = [0, 1, 2, 3]
     * cacheMap[1] = [4, 5, 6, 7]
     * Implies, threads at indices 0,1,2,3 are pinned to cache 0 and threads at
     * indices 4,5,6,7 are pinned to cache 1
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

    // clang-format off
    /**
     * @brief           Update the final affinity vector
     *
     * @details         This function updates the affinity vector by
     * distributing the threads in the procVectPerCache among the cores in the
     * coreList
     *
     * @param[out]      procVect            Vector to store the affinity
     *
     * @param[in]       procVectPerCache    Affinity vector for a cache[ holds
     * the index of cores in the coreList]
     * Example
     * coreList = [8, 9, 10, 11, 12, 13, 14, 15]
     * procVectPerCache = [0, 2, 4, 6]
     * Implies, threadindices at indices 0,1,2,3 of threads vector are pinned to
     * cores 8, 10, 12, 14
     * Example
     * threads = [0, 1, 5, 6]
     * Implies
     *
     * thread at 0th index in threads is pinned to core at 0th index of coreList,
     * thread at 1st index in threads is pinned to core at 2nd index of coreList,
     * thread at 5th index in threads is pinned to core at 4th index of coreList,
     * thread at 6th index in threads is pinned to core at 6th index of coreList
     *
     * threads holds the indices of the threads in the input threadlist to be
     * pinned to the cores procVectPerCache holds the indices of the cores in
     * the coreList to which the threads are to be pinned there is a one to one
     * mapping between the threads and procVectPerCache
     * @param[in]       coreList            List of core numbers sharing the
     * cache
     *
     * @param[in]       threads             List of threadindices to be pinned
     * to the cores sharing the cache.
     */
    // clang-format on
    void updateprocVect(std::vector<int>&       procVect,
                        const std::vector<int>& procVectPerCache,
                        const std::vector<int>& coreList,
                        const std::vector<int>& threads)
    {
        int index = 0;
        for (auto thread : threads) {
            int item         = procVectPerCache[index++];
            int coreNum      = coreList[item];
            procVect[thread] = coreNum;
        }
    }

    /**
     * @brief           Update the final affinity vector
     *
     * @details         This function updates the affinity vector based on the
     *                  processorMap and the threadCount
     *                  It updates the affinity vector by picking one core from
     *                  a processor at a time and then moving to the next
     * processor.
     *
     * @param[out]      procVect        Vector to store the affinity
     *
     * @param[in]       processorMap    The processor topology
     *
     * @param[in]       pMap            A single core mask corresponding to each
     * processor (Processors can have multiple coremasks of 64 bits each)
     *
     * @param[in]       maskIndex       Index of the coremask in the processor's
     * mask list
     *
     * @param[in]       threadCount     Number of threads to pin
     *
     * @param[in,out]   threadId        Index of the thread to pin
     */
    void updateprocVect(std::vector<int>&                   procVect,
                        std::vector<std::vector<CoreMask>>& processorMap,
                        std::vector<CoreMask>&              pMap,
                        std::vector<size_t>&                maskIndex,
                        int                                 threadCount,
                        int&                                threadId)
    {

        for (size_t coreId = 0; coreId < processorMap.size(); coreId++) {
            if (skipMask(pMap[coreId], maskIndex[coreId], processorMap[coreId]))
                continue;

            procVect.push_back(
                calculateCoreNum(pMap[coreId])
                + calculateOffset(pMap[coreId].second, cpuInfo.groupMap));
            updateMap(pMap[coreId], calculateCoreNum(pMap[coreId]));

            threadId++;
            if (threadId == threadCount)
                return;
        }
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

            updateprocVect(procVect, procVectPerCache, coreList, cache.second);
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
        auto processorMap = cpuInfo.processorMap;
        int  threadCount  = procVect.size();
        int  threadId     = 0;

        std::vector<size_t>   maskIndex(processorMap.size(), 1);
        std::vector<CoreMask> pMap;
        for (const auto& core : processorMap) {
            pMap.push_back(core[0]);
        }

        procVect.clear();
        while (threadId < threadCount) {
            if (isReset(processorMap)) {
                // if all the cores are used, reset the processorMap
                processorMap = cpuInfo.processorMap;
            }
            updateprocVect(
                procVect, processorMap, pMap, maskIndex, threadCount, threadId);
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
            AUD_ASSERT(processorList[i] < std::thread::hardware_concurrency(),
                       "Invalid processor Id");
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
                if (processorList[i] > core + gMap.second - 1) {
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
