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
#include <vector>

#ifdef __linux__
#include "Au/ThreadPinning/Linux/CpuTopology.hh"
#else
#include "Au/ThreadPinning/Windows/CpuTopology.hh"
#include <windows.h>
#endif

#include <Au/ThreadPinning.hh>

#define GROUP_SIZE 64
namespace Au
{
class AffinityVector
{
  protected:
    const CpuTopology& cpuInfo;

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
        createVector(
            cacheVect, 0, cacheVect.size() - 1, 0, cpuInfo.cacheMap.size() - 1);

        // Create a map with L3 cache  number as key
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
                while (pMap.first > 0) {
                    coreList.push_back((log2(pMap.first & -1 * pMap.first)) + (pMap.second * GROUP_SIZE));
                    pMap.first = pMap.first & ~(1ULL << coreList.back());
                }
            int threadCount = cache.second.size();
            // Create the vector that maps the threads that got mapped to the
            // cache to cores sharing the  cache.
            std::vector<int> Vect(threadCount);
            createVector(Vect, 0, threadCount - 1, 0, coreList.size() - 1);

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
        int  threadId    = 0;
        auto pMap        = cpuInfo.processorMap;
        int  threadCount = procVect.size();
        procVect.clear();

        while (threadId < threadCount) {
            // check if all the first elements of pMap are 0
            if (std::all_of(pMap.begin(), pMap.end(), [](auto& p) { return p.first == 0; }))
                pMap = cpuInfo.processorMap;
            for (size_t i = 0; i < pMap.size(); i++) {
                if(pMap[i].first == 0)
                    continue;
                procVect.push_back((log2(pMap[i].first & -1 * pMap[i].first)) + (pMap[i].second * GROUP_SIZE));
                pMap[i].first = pMap[i].first & ~(1ULL << procVect[threadId++]);
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
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadList[i]);
		if (hThread != NULL) {
			GROUP_AFFINITY groupAffinity;
			groupAffinity.Mask = 1ull << processorList[i] % GROUP_SIZE;
			groupAffinity.Group = processorList[i] / GROUP_SIZE;
            SetThreadGroupAffinity(hThread, &groupAffinity, nullptr);
            CloseHandle(hThread);
		}
#endif
        }
    }
};
} // namespace Au
