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

#include <bitset>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include <tuple>
#include <vector>
typedef unsigned long KAFFINITY;
// Define DWORD
typedef unsigned long DWORD;

namespace Au {

struct CpuTopology
{
    uint32_t                               active_processors;
    std::vector<std::pair<KAFFINITY, int>> processorMap;
    std::vector<std::pair<KAFFINITY, int>> cacheMap;

    static const CpuTopology& get()
    {
        static const CpuTopology info;
        return info;
    }

    CpuTopology()
        : active_processors(0)
        , processorMap{}
        , cacheMap{}
    {
    }
};
} // namespace Au
