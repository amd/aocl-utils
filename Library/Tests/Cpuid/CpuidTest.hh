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
#include <fstream>

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
writeToFile(const String& fileName, const vector<T>& items)
{
    ofstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file" << fileName << endl;
    } else {
        for (auto item : items) {
            file << *item << endl;
        }
    }
}
/**
 * @brief Utility function to read data from a file
 * @param fileName The absolute path of the file
 * @return vector<T> The flags read from the file
 */
template<typename T>
vector<T>
readFromFile(const String& fileName)
{
    ifstream  file(fileName);
    String    data;
    vector<T> items;

    if (!file.is_open()) {
        cout << "Error opening file" << fileName << endl;
    }

    while (getline(file, data)) {
        auto value = data.substr(0, data.find(' '));
        auto item  = static_cast<T>(atoi(value.c_str()));
        items.push_back(item);
    }

    file.close();
    return items;
}
} // namespace
