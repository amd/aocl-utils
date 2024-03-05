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

#include <Python.h>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <vector>

#include "QemuTest.hh"

namespace {
using namespace Au;

class QemuTest
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::vector<bool>>>
{
  private:
    /**
     * @brief Write the flags to a file
     *
     * @param[in] filename The name of the file to write to
     * @param[in] flags The flags to write to the file
     *
     * @return void
     */
    static void writeToFile(const std::string&             filename,
                            const std::vector<ECpuidFlag>& flags)
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error opening file" << std::endl;
        } else {
            for (auto flag : flags) {
                file << *flag << std::endl;
            }
        }
    }

    /**
     * @brief Call the Qemu emulator to run the tests
     * Implemented in a python module named CpuidTest.
     * @param[in] cpu The type of the cpu to emulate
     * @param[in] testName The name of the test to runi
     *
     * @return bool The result of the test run on the emulated cpu
     */
    static bool callQemuEmulator(const char* cpu, const char* testName)
    {
        auto ret = false;

        Py_Initialize();
        if (Py_IsInitialized() == 0) {
            std::cerr << "Failed to initialize the Python interpreter";
        }
        /* Update PYTHONPATH with the path of the python module to be loaded*/
        std::string srcPath = PROJECT_SOURCE_DIR;
        srcPath += "/Library/Tests/Cpuid/";
        PyObject* sys        = PyImport_ImportModule("sys");
        PyObject* sysPath    = PyObject_GetAttrString(sys, "path");
        PyObject* folderPath = PyUnicode_FromString(srcPath.c_str());
        PyList_Append(sysPath, folderPath);

        /*Load The test module */
        PyObject* pModule = PyImport_ImportModule("CpuidTest");

        if (pModule != NULL) {
            PyObject* pFunc = PyObject_GetAttrString(pModule, "run_qemu");

            if (pFunc != NULL && PyCallable_Check(pFunc) != 0) {
                /* Call the run_qemu function with arguments */
                std::string binaryPath = PROJECT_BUILD_DIR;
                std::string releaseType;
                if (AU_BUILD_TYPE_RELEASE)
                    releaseType = "Release";
                if (AU_BUILD_TYPE_DEBUG)
                    releaseType = "Developer";
                if (AU_BUILD_TYPE_DEVELOPER)
                    releaseType = "Library/Tests";

                binaryPath += "/" + releaseType + "/core_CpuidTest";

                PyObject* pArgs =
                    PyTuple_Pack(4,
                                 PyUnicode_DecodeFSDefault("x86_64"),
                                 PyUnicode_DecodeFSDefault(cpu),
                                 PyUnicode_DecodeFSDefault(binaryPath.c_str()),
                                 PyUnicode_DecodeFSDefault(testName));
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
    /**
     * @brief Run all the tests in testNames on the emulated cpu.
     * @param cpuType The type of the cpu to emulate
     * @param testNames The names of the tests to run
     *
     * @return std::vector<bool> The results of the tests
     */
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

    /**
     * @brief Check if the emulated cpu has all the flags in the flags vector
     * @param cpuType The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    static bool hasFlagT(const std::string& cpuType)
    {
        String simnowdataPath = PROJECT_SOURCE_DIR;
        simnowdataPath += "/Library/Tests/Cpuid/Mock/simnowdata/";
        std::filesystem::copy(simnowdataPath + cpuType + "/FlagsT.txt",
                              "FlagsT.txt");
        return callQemuEmulator(cpuType.c_str(), "hasFlagT");
    }

    /**
     * @brief Check if the emulated cpu does not have any of the flags in the
     * flags vector
     * @param cpuType The type of the cpu to emulate
     *
     * @return bool The result of the test
     */
    static bool hasFlagF(const std::string& cpuType)
    {
        String simnowdataPath = PROJECT_SOURCE_DIR;
        simnowdataPath += "/Library/Tests/Cpuid/Mock/simnowdata/";
        std::filesystem::copy(simnowdataPath + cpuType + "/FlagsF.txt",
                              "FlagsF.txt");

        return callQemuEmulator(cpuType.c_str(), "hasFlagF");
    }

    /**
     * @brief Clean up the test environment
     *
     * @return void
     */
    void TearDown() override
    {
        std::string cleanPath = PROJECT_SOURCE_DIR;
        cleanPath += "/Library/Tests/Cpuid/__pycache__/";
        std::filesystem::remove_all(cleanPath);
        std::remove("FlagsT.txt");
        std::remove("FlagsF.txt");
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
    results.push_back(hasFlagT(cpuType));
    results.push_back(hasFlagF(cpuType));
    EXPECT_EQ(results, expectedResults);
}
} // namespace
