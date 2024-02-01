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

#include "CpuidTest.hh"
#include <Python.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <vector>
namespace {
class QemuTest
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<bool>>>
{
  private:
    static bool callQemuEmulator(const char* cpu, const char* testname)
    {
        auto ret = false;

        Py_Initialize();
        if (Py_IsInitialized() == 0) {
            std::cerr << "Failed to initialize the Python interpreter";
        }
        /* Update PYTHONPATH with the path of the python module to be loaded*/
        std::string src_path = PROJECT_SOURCE_DIR;
        src_path += "/Library/Tests/Cpuid/";
        PyObject* sys         = PyImport_ImportModule("sys");
        PyObject* sys_path    = PyObject_GetAttrString(sys, "path");
        PyObject* folder_path = PyUnicode_FromString(src_path.c_str());
        PyList_Append(sys_path, folder_path);

        /*Load The test module */
        PyObject* pModule = PyImport_ImportModule("CpuidTest");

        if (pModule != NULL) {
            PyObject* pFunc = PyObject_GetAttrString(pModule, "run_qemu");

            if (pFunc != NULL && PyCallable_Check(pFunc) != 0) {
                /* Call the run_qemu function with arguments */
                std::string binary_path = PROJECT_BUILD_DIR;
                binary_path += "/Release/core_CpuidTest";

                PyObject* pArgs =
                    PyTuple_Pack(4,
                                 PyUnicode_DecodeFSDefault("x86_64"),
                                 PyUnicode_DecodeFSDefault(cpu),
                                 PyUnicode_DecodeFSDefault(binary_path.c_str()),
                                 PyUnicode_DecodeFSDefault(testname));
                PyObject* pValue = PyObject_CallObject(pFunc, pArgs);

                if (pValue != NULL) {
                    ret = PyLong_AsLong(pValue) != 0;
                }
            }
        }
        /* Finalize the Python interpreter */
        Py_Finalize();

        return ret;
    }

  protected:
    static std::vector<bool> testAll(const std::string&              cpuType,
                                     const std::vector<std::string>& testNames)
    {
        std::vector<bool> results;
        for (const auto& testName : testNames) {
            bool result = callQemuEmulator(cpuType.c_str(), testName.c_str());
            results.push_back(result);
        }
        return results;
    }
    void TearDown() override
    {
        std::string clean_path = PROJECT_SOURCE_DIR;
        clean_path += "/Library/Tests/Cpuid/__pycache__/";
        std::filesystem::remove_all(clean_path);
    }
};

INSTANTIATE_TEST_SUITE_P(QemuTestSuite,
                         QemuTest,
                         ::testing::ValuesIn(testParametersX86Cpu));

TEST_P(QemuTest, CpuTypeTest)
{
    const auto        params  = GetParam();
    const std::string cpuType = std::get<0>(params);
    std::cout << "Emulating " << cpuType << std::endl;
    const std::vector<std::string> testNames = {
        "X86Cpuid.DISABLED_isAMD",
        "X86Cpuid.DISABLED_isIntel",
        "X86Cpuid.DISABLED_isX86_64v2",
        "X86Cpuid.DISABLED_isX86_64v3",
        "X86Cpuid.DISABLED_isX86_64v4"
    };
    const std::vector<bool> expectedResults = std::get<1>(params);

    auto results = testAll(cpuType, testNames);
    EXPECT_EQ(results, expectedResults);
}
} // namespace
