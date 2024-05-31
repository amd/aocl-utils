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

/**
 * @file  CpuidTest.hh
 * @brief Utility functions common to Cpuid tests
 */

#include "Au/Cpuid/X86Cpu.hh"
#include "Au/Misc.hh"
#include "gtest/gtest.h"
#include <fstream>
#ifdef WIN32
#include <windows.h>
#endif
namespace {
using namespace Au;

/**
 * @brief Utility function to write the data to a file
 *
 * @param[in] fileName The name of the file to write to
 * @param[in] items The data to write to the file
 *
 * @return void
 */
template<typename T>
static void
writeToFile(const String& fileName, const std::vector<T>& items)
{
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Error opening file" << fileName << std::endl;
    } else {
        for (auto item : items) {
            file << *item << std::endl;
        }
    }
}
/**
 * @brief Utility function to read data from a file
 * @param fileName The absolute path of the file
 * @return vector<T> The flags read from the file
 */
template<typename T>
inline std::vector<T>
readFromFile(const String& fileName)
{
    std::ifstream  file(fileName);
    String         data;
    std::vector<T> items{};

    if (!file.is_open()) {
        std::cout << "Error opening file" << fileName << std::endl;
    } else {
        while (getline(file, data)) {
            auto value = data.substr(0, data.find(' '));
            auto item  = static_cast<T>(atoi(value.c_str()));
            items.push_back(item);
        }
    }

    file.close();
    return items;
}

template<>
inline std::vector<String>
readFromFile<String>(const String& fileName)
{
    std::ifstream       file(fileName);
    String              data;
    std::vector<String> items;

    if (!file.is_open()) {
        std::cout << "Error opening file" << fileName << std::endl;
    } else {
        while (getline(file, data)) {
            auto value = data.substr(0, data.find(' '));
            items.push_back(value);
        }
    }
    file.close();
    return items;
}

static inline void
#ifdef __linux__
checkAffinity(cpu_set_t Mask)
{
    cpu_set_t cpuSet;
    auto      tid = gettid();
    sched_getaffinity(tid, sizeof(cpuSet), &cpuSet);
    EXPECT_TRUE(cpuSet.__bits[0] == Mask.__bits[0]);
#else
checkAffinity(DWORD_PTR Mask)
{
    DWORD threadId    = GetCurrentThreadId();
    auto  currentMask = SetThreadAffinityMask(&threadId, Mask);
    EXPECT_TRUE(currentMask == Mask);
#endif
}
} // namespace
