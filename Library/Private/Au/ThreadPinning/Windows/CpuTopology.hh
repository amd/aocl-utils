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
#include "Au/Config.h"
#include <bitset>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include <tuple>
#include <vector>
#include <windows.h>
#include "Au/Config.h"

namespace Au {
typedef std::pair<KAFFINITY, int> CoreMask;
using SLPIEX = SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX;
// if compiler is msvc
/**
 * @brief       Advance a pointer by a number of bytes.
 *
 * @details     Utility function to iterate through the processor information
 *              buffer.
 *
 * @param[in]   p   Pointer to the current position in the buffer.
 * @param[in]   cb  Number of bytes to advance the pointer.
 *
 * @return      Pointer to the new position in the buffer.
 */
template<typename T>
T*
AdvanceBytes(T* p, SIZE_T cb)
{
    return reinterpret_cast<T*>(reinterpret_cast<BYTE*>(p) + cb);
}

/**
 * @brief       Class to iterate through the logical processor information
 *              buffer.
 *
 * @details     This class is used to iterate through the logical processor
 *              information buffer.
 *
 * @param[in]   Relationship    The type of information to retrieve.
 *
 */
class LogicalProcessorInformation
{
  public:
    LogicalProcessorInformation(LOGICAL_PROCESSOR_RELATIONSHIP Relationship)
        : m_bufferBase(nullptr)
        , m_bufferCurrent(nullptr)
        , m_bufferRemaining(0)
    {
        DWORD bufferSize = 0;
        if (GetLogicalProcessorInformationEx(
                Relationship, nullptr, &bufferSize))
            return;
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return;
        m_bufferBase =
            reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(
                LocalAlloc(LMEM_FIXED, bufferSize));
        if (!m_bufferBase)
            return;
        if (!GetLogicalProcessorInformationEx(
                Relationship, m_bufferBase, &bufferSize))
            return;
        m_bufferCurrent   = m_bufferBase;
        m_bufferRemaining = bufferSize;
    }

    ~LogicalProcessorInformation() { LocalFree(m_bufferBase); }

    /**
     * @brief       Move to the next logical processor information entry.
     *
     * @details     This function moves the iterator to the next logical
     *              processor information entry.
     *
     * @return      void
     */
    void MoveNext()
    {
        if (m_bufferCurrent) {
            m_bufferRemaining -= m_bufferCurrent->Size;
            if (m_bufferRemaining) {
                m_bufferCurrent =
                    AdvanceBytes(m_bufferCurrent, m_bufferCurrent->Size);
            } else {
                m_bufferCurrent = nullptr;
            }
        }
    }

    /**
     * @brief       Get the current logical processor information entry.
     *
     * @details     This function returns the current logical processor
     *              information entry.
     *
     * @return      Pointer to the current logical processor information entry.
     */
    SLPIEX* Current() { return m_bufferCurrent; }

  private:
    SLPIEX* m_bufferBase;
    SLPIEX* m_bufferCurrent;
    DWORD   m_bufferRemaining;
};

class CpuTopology
{
  public:
    uint32_t                           active_processors;
    std::vector<std::vector<CoreMask>> processorMap;
    std::vector<std::vector<CoreMask>> cacheMap;
    std::vector<CoreMask>              groupMap;

    static const CpuTopology& get()
    {
        static const CpuTopology info;
        return info;
    }

    CpuTopology()
        : active_processors(0)
        , processorMap{}
        , cacheMap{}
        , groupMap{}
    {
        active_processors = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
        LogicalProcessorInformation processorInfo(RelationProcessorCore);
        LogicalProcessorInformation cacheInfo(RelationCache);
        LogicalProcessorInformation groupInfo(RelationGroup);

#ifdef AU_COMPILER_IS_MSVC
        for (; auto pInfo = processorInfo.Current(); processorInfo.MoveNext()) {
            // Collect the physical core -> logical core mapping
            processorMap.push_back(
                { std::make_pair(pInfo->u.Processor.GroupMask->Mask,
                                 pInfo->u.Processor.GroupMask->Group) });
        }

        for (; auto cInfo = cacheInfo.Current(); cacheInfo.MoveNext()) {
            // Collect the L3 Cache --> Logical core mapping
            if (cInfo->u.Cache.Level == 3
                && (cInfo->u.Cache.Type == CacheData
                    || cInfo->u.Cache.Type == CacheUnified)) {
                cacheMap.push_back(
                    { std::make_pair(cInfo->u.Cache.u.GroupMask.Mask,
                                     cInfo->u.Cache.u.GroupMask.Group) });
            }
        }
        for (; auto gInfo = groupInfo.Current(); groupInfo.MoveNext()) {

            // Collect the Group --> Logical core mapping
            groupMap.push_back(
                std::make_pair(gInfo->u.Group.GroupInfo->ActiveProcessorMask,
                               gInfo->u.Group.GroupInfo->ActiveProcessorCount));
        }
#else
        for (; auto pInfo = processorInfo.Current(); processorInfo.MoveNext()) {
            // Collect the physical core -> logical core mapping
            processorMap.push_back(
                { std::make_pair(pInfo->Processor.GroupMask->Mask,
                                 pInfo->Processor.GroupMask->Group) });
        }

        for (; auto cInfo = cacheInfo.Current(); cacheInfo.MoveNext()) {
            // Collect the L3 Cache --> Logical core mapping
            if (cInfo->Cache.Level == 3
                && (cInfo->Cache.Type == CacheData
                    || cInfo->Cache.Type == CacheUnified)) {
                cacheMap.push_back(
                    { std::make_pair(cInfo->Cache.GroupMask.Mask,
                                     cInfo->Cache.GroupMask.Group) });
            }
        }
        for (; auto gInfo = groupInfo.Current(); groupInfo.MoveNext()) {
            // Collect the Group --> Logical core mapping
            groupMap.push_back(
                std::make_pair(gInfo->Group.GroupInfo->ActiveProcessorMask,
                               gInfo->Group.GroupInfo->ActiveProcessorCount));
        }
#endif
    }
};
} // namespace Au
