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
#include <bitset>
#include <cmath>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <sys/sysinfo.h>
#include <tuple>
#include <vector>

namespace Au {
typedef unsigned long KAFFINITY;
// Define DWORD
typedef unsigned long DWORD;

class LogicalProcessorInformation
{

  public:
    std::string    filename;
    int            cpuId;
    DIR*           dirp;
    struct dirent* dp; // Pointer to the directory entry

    explicit LogicalProcessorInformation(const std::string& filename)
        : filename(filename)
        , cpuId(0)
        , dirp(opendir("/sys/devices/system/cpu/"))
        , dp(NULL)
    {
    }
    // Copy constructor
    LogicalProcessorInformation(LogicalProcessorInformation& other)
        : filename(other.filename)
        , cpuId(other.cpuId)
        , dirp(other.dirp)
        , dp(other.dp)
    {
        other.dp   = NULL;
        other.dirp = NULL;
    }
    // = operator
    LogicalProcessorInformation& operator=(LogicalProcessorInformation& other)
    {
        if (this != &other) {
            filename   = other.filename;
            cpuId      = other.cpuId;
            dirp       = other.dirp;
            dp         = other.dp;
            other.dp   = NULL;
            other.dirp = NULL;
        }
        return *this;
    }
    ~LogicalProcessorInformation() { closedir(dirp); }
    /**
     * @brief       Move to the next directory entry.
     *
     * @details     This function moves to the next directory
     *              corresponding to cpu<id>, updates cpuid memeber variable
     *
     * @param[in]   void
     *
     * @return      void
     */
    void MoveNext()
    {
        // Move to the next directory entry
        while ((dp = readdir(dirp)) != NULL) {
            std::string dirName(dp->d_name);
            if (dirName.find("cpu") == 0 && isdigit(dirName[3])) {
                cpuId = std::stoi(dirName.substr(3));
                break;
            }
        }
    }

    /**
     * @brief       Parse the line and store the information.
     *
     * @details     This function parses the line and stores the information in
     * the Map. The line is a comma separated string of hexadecimal values.
     *
     * @param[out]   std::vector<std::pair<KAFFINITY, int>>& Map
     *               A vector of pairs that stores the mask and the position of
     * the mask. obtained from parsing the line.
     *
     * @param[in]   std::string line
     *
     * @return      void
     */
    void parseLine(std::vector<std::pair<KAFFINITY, int>>& Map,
                   std::string                             line)
    {
        std::reverse(line.begin(), line.end());
        std::stringstream ss(line);
        int               position = 0;
        std::string       chunk;
        int               nonZeroMask = 0;

        while (std::getline(ss, chunk, ',')) {
            if (std::stoull(chunk, nullptr, 16)) {
                std::reverse(chunk.begin(), chunk.end());

                Map.resize(nonZeroMask + 1);
                Map[nonZeroMask].first  = std::stoull(chunk, nullptr, 16);
                Map[nonZeroMask].second = position;

                nonZeroMask++;
            }
            position++;
        }
    }

    /**
     * @brief       Process the file and store the information.
     *
     * @details     This function processes the file and stores the information
     * in the Map. The file is a comma separated string of hexadecimal values.
     *
     * @param[out]   std::vector<std::pair<KAFFINITY, int>>& Map
     *               A vector of pairs that stores the mask and the position of
     * the mask. obtained from parsing the line.
     *
     * @param[in]   std::ifstream& file
     *
     * @return      void
     */
    void processFile(std::vector<std::pair<KAFFINITY, int>>& Map,
                     std::ifstream&                          file)
    {
        std::string line;
        if (getline(file, line))
            parseLine(Map, line);
    }

    /**
     * @brief       Get the current directory entry.
     *
     * @details     This function gets the current directory entry.
     *
     * @param[out]   std::vector<std::pair<KAFFINITY, int>>& Map
     *               A vector of pairs that stores the mask and the position of
     * the mask. obtained from parsing the line.
     *
     * @return      void
     */
    void Current(std::vector<std::pair<KAFFINITY, int>>& Map)
    {
        if (dp) {
            std::string   dirName(dp->d_name);
            std::ifstream file("/sys/devices/system/cpu/" + dirName + filename);
            // check if file is open, for offline cpus the file will not be
            // present.
            //  Hence will fail to open.
            if (file.is_open()) {
                processFile(Map, file);
            }
        }
    }
};

class CpuTopology
{
  private:
    /**
     * @brief            Eliminate duplicates from the Map.
     *
     * @details          This function eliminates duplicates from the Map.
     *
     * @param[in/out]    std::vector<std::vector<std::pair<KAFFINITY, int>>>&
     * Map A vector of unique pairs from the Map.
     * @return      void
     */
    void eliminateDuplicates(
        std::vector<std::vector<std::pair<KAFFINITY, int>>>& Map)
    {
        std::sort(Map.begin(), Map.end());
        auto newMap = std::unique(Map.begin(), Map.end());
        Map.erase(newMap, Map.end());
    }

    /**
     * @brief            Compare two vectors.
     *
     * @details          Used to reorder the vector after eliminating
     * duplicates.
     *
     * @param[in]        const std::vector<std::pair<KAFFINITY, int>>& a
     *                   A vector of pairs.
     * @param[in]        const std::vector<std::pair<KAFFINITY, int>>& b
     *                   A vector of pairs.
     *
     * @return           bool
     */
    static bool compareVectors(const std::vector<std::pair<KAFFINITY, int>>& a,
                               const std::vector<std::pair<KAFFINITY, int>>& b)
    {
        if (a[0].second == b[0].second)
            return a[0].first < b[0].first;
        else
            return a[0].second < b[0].second;
    }

  public:
    uint32_t                                            active_processors;
    std::vector<std::vector<std::pair<KAFFINITY, int>>> processorMap;
    std::vector<std::vector<std::pair<KAFFINITY, int>>> cacheMap;
    std::vector<std::pair<KAFFINITY, int>>              groupMap;

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
        active_processors = get_nprocs();
        LogicalProcessorInformation processorInfo("/topology/thread_siblings");
        LogicalProcessorInformation cacheInfo("/cache/index3/shared_cpu_map");

        // Collect the physical core -> logical core mapping
        processorMap.resize(active_processors);
        processorInfo.MoveNext();
        while (processorInfo.dp) {
            processorInfo.Current(processorMap[processorInfo.cpuId]);
            processorInfo.MoveNext();
        }
        eliminateDuplicates(processorMap);
        std::sort(processorMap.begin(), processorMap.end(), compareVectors);

        // Collect the L3 Cache --> Logical core mapping
        cacheMap.resize(active_processors);
        cacheInfo.MoveNext();
        while (cacheInfo.dp) {
            cacheInfo.Current(cacheMap[cacheInfo.cpuId]);
            cacheInfo.MoveNext();
        }

        eliminateDuplicates(cacheMap);
        std::sort(cacheMap.begin(), cacheMap.end(), compareVectors);

        // Collect the Group --> Logical core mapping
        groupMap.resize(active_processors);
        for (size_t cpuId = 0; cpuId < processorMap.size(); cpuId++) {
            for (auto processorMask : processorMap[cpuId]) {
                int groupIndex = processorMask.second;
                groupMap[groupIndex].first |= processorMask.first;
                std::bitset<32> bits(processorMask.first);
                groupMap[groupIndex].second += bits.count();
            }
        }
        while (!groupMap.back().first) {
            groupMap.pop_back();
        }
    }
};
} // namespace Au
