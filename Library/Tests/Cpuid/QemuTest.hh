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

#if defined(AU_TARGET_OS_IS_LINUX)
#include <Python.h>
#endif
#include "Au/Cpuid/X86Cpu.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace Au;
/**
 * Test parameters for X86Cpu Qemu test
 * 1. Name of the CPU model to be emulated by Qemu
 * 2. Vector containing the following parameters:
 *    1. Name of the CPU model to be emulated by Qemu
 *    2. Vector of boolean values that marks the expected results of
 *      {is_AMD, is_Intel, is_X86_64v2, is_X86_64v3, is_X86_64v4} APIs
 * 3. The Uarch(microarchitecutre) of the CPU.
 */
// clang-format off
auto isAmd=true, isIntel=true, isX86_64v2=true, isX86_64v3=true, isX86_64v4=true, flagPresent=true, flagAbsent=true, isUarch=true;
const std::vector<std::tuple<String, std::vector<bool>, EUarch>> testParametersX86Cpu = {
    { "EPYC-Genoa-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen4 },
    { "EPYC-Milan-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen3 },
    { "EPYC-Milan-v2",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen3 },
    { "EPYC-Rome-v1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen2 },
    { "EPYC-Rome-v2",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen2 },
    { "EPYC-Rome-v3",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen2 },
    { "EPYC-Rome-v4",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen2 },
    { "EPYC-v1",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen },
    { "EPYC-v2",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen },
    { "EPYC-v3",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen },
    { "EPYC-v4",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch },  EUarch::Zen },
    { "Opteron_G1-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Opteron_G2-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Opteron_G3-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Opteron_G4-v1",     { isAmd,  !isIntel, isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Opteron_G5-v1",     { isAmd,  !isIntel, isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "phenom-v1",         { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    //{ "Broadwell-v1",      { !isAmd, isIntel,  isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Denverton-v1",      { !isAmd, isIntel,  isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    { "Conroe-v1",         { !isAmd, isIntel,  !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, !isUarch }, EUarch::Unknown },
    //{ "Skylake-Server-v1", { !isAmd, isIntel,  isX86_64v2,  isX86_64v3,  !isX86_64v4,  flagPresent, flagAbsent, !isUarch }, EUarch::Unknown }
};
// clang-format on

/**
 * Test parameters to verify getVendorInfo Api
 * Vector contains the following parameters:
 * 1. Name of the CPU model to be emulated using Qemu
 * 2. VendorInfo structure containing the expected results.
 */
// clang-format off
const std::vector<std::tuple<String, VendorInfo>> testParametersVendorInfo = {
    { "EPYC-Genoa-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x11, 0x0, EUarch::Zen4 } } },
    { "EPYC-Milan-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x01, 0x1, EUarch::Zen3 } } },
    { "EPYC-Milan-v2",     { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x01, 0x1, EUarch::Zen3 } } },
    { "EPYC-Rome-v1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0, EUarch::Zen2 } } },
    { "EPYC-Rome-v2",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0, EUarch::Zen2 } } },
    { "EPYC-Rome-v3",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0, EUarch::Zen2 } } },
    { "EPYC-Rome-v4",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0, EUarch::Zen2 } } },
    { "EPYC-v1",           { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x01, 0x2, EUarch::Zen } } },
    { "EPYC-v2",           { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x01, 0x2, EUarch::Zen } } },
    { "EPYC-v3",           { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x01, 0x2, EUarch::Zen } } },
    { "EPYC-v4",           { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x01, 0x2, EUarch::Zen } } },
    { "Opteron_G1-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x06, 0x1, EUarch::Unknown } } },
    { "Opteron_G2-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x06, 0x1, EUarch::Unknown } } },
    { "Opteron_G3-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x3, EUarch::Unknown } } },
    { "Opteron_G4-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x01, 0x2, EUarch::Unknown } } },
    { "Opteron_G5-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x0, EUarch::Unknown } } },
    { "phenom-v1",         { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x3, EUarch::Unknown } } },
    { "Broadwell-v1",      { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0X3D, 0x2, EUarch::Unknown } } },
    { "Denverton-v1",      { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x5F, 0x1, EUarch::Unknown } } },
    { "Conroe-v1",         { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x0F, 0x3, EUarch::Unknown } } },
    { "Skylake-Server-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x55, 0x4, EUarch::Unknown } } }
};

class QemuTestBase :public testing::Test
{
    protected:
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

#if defined(AU_TARGET_OS_IS_LINUX)
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
                    releaseType = "Debug";
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
#endif
        return ret;
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
        std::remove("Uarch.txt");
        std::remove("UarchResult.txt");
        std::remove("VendorInfoC.txt");
        std::remove("VendorInfoCpp.txt");
    }
};
} // namespace
