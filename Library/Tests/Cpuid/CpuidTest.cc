/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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
#include "gtest/gtest.h"
#include <cstdlib>
#include <fstream>

namespace {
using namespace Au;

/**
 * Tests that can run on host, DISABLED by default since tests doesn't add any
 * value as the result is unknown on a physical machine
 * The disabled tests are used by the QemuTest to verify the results of the
 * Cpuid emulation in Qemu.
 */
TEST(X86Cpuid, DISABLED_isAMD)
{
    X86Cpu cpu{ 0 };
    EXPECT_TRUE(cpu.isAMD() == true);
}

TEST(X86Cpuid, DISABLED_isX86_64v2)
{

    X86Cpu cpu{ 0 };
    EXPECT_TRUE(cpu.isX86_64v2() == true);
}

TEST(X86Cpuid, DISABLED_isX86_64v3)
{

    X86Cpu cpu{ 0 };
    EXPECT_TRUE(cpu.isX86_64v3() == true);
}

TEST(X86Cpuid, DISABLED_isX86_64v4)
{

    X86Cpu cpu{ 0 };
    EXPECT_TRUE(cpu.isX86_64v4() == true);
}

TEST(X86Cpuid, DISABLED_isIntel)
{

    X86Cpu cpu{ 0 };
    EXPECT_TRUE(cpu.isIntel() == true);
}

TEST(X86Cpuid, DISABLED_hasFlagPositive)
{
    X86Cpu       cpu{ 0 };
    auto         flags = readFromFile<String>("FlagsT.txt");
    string       token;
    stringstream ss;

    ss << flags;
    while (std::getline(ss, token, ':')) {
        auto flag       = stoi(token);
        auto cpuid_flag = valueToEnum<ECpuidFlag, Uint64>(flag);
        EXPECT_TRUE(cpu.hasFlag(cpuid_flag));
    }
}

TEST(X86Cpuid, DISABLED_hasFlagNegative)
{
    X86Cpu       cpu{ 0 };
    auto         flags = readFromFile<String>("FlagsF.txt");
    string       token;
    stringstream ss;

    ss << flags;
    while (std::getline(ss, token, ':')) {
        auto flag       = stoi(token);
        auto cpuid_flag = valueToEnum<ECpuidFlag, Uint64>(flag);
        EXPECT_FALSE(cpu.hasFlag(cpuid_flag));
    }
}

TEST(X86Cpuid, DISABLED_isUarch)
{
    EUarch uarch;
    X86Cpu cpu{ 0 };

    // verify the uarch passed from the qemu testcase.
    uarch = readFromFile<EUarch>("Uarch.txt").front();
    EXPECT_TRUE(cpu.isUarch(uarch));

    // Verify isUarch returns False for any microarchitecure higher than the
    // current one
    uarch = static_cast<EUarch>(static_cast<int>(uarch) + 1);
    EXPECT_FALSE(cpu.isUarch(uarch));

    // Verify isUarch returns True for any microarchitecure lower than the
    // current one.
    uarch = static_cast<EUarch>(static_cast<int>(uarch) - 2);
    EXPECT_TRUE(cpu.isUarch(uarch));

    // Verify isUarch returns false for any microarchitecure lower than the
    // current one in strict mode
    EXPECT_FALSE(cpu.isUarch(uarch, true));
    writeToFile<EUarch>("UarchResult.txt", { cpu.getUarch() });
}

TEST(X86Cpuid, DISABLED_getVendorInfo)
{
    X86Cpu cpu{ 0 };
    auto   result = cpu.getVendorInfo();

    vector<Uint32> vendorInfo = { enumToValue<EVendor, Uint32>(result.m_mfg),
                                  enumToValue<EFamily, Uint32>(result.m_family),
                                  result.m_model,
                                  result.m_stepping,
                                  enumToValue<EUarch, Uint32>(result.m_uarch) };

    auto iter = vendorInfo.begin();
    writeToFile<vector<Uint32>::iterator>(
        "VendorInfoCpp.txt", { iter, iter + 1, iter + 2, iter + 3, iter + 4 });
}
TEST(X86Cpuid, CheckCupNumber)
{
    // 0 - 2 Verify that the core is set to the correct CPU
    cpu_set_t cpuSet;

    X86Cpu cpu{ 0 };
    auto   pid = getpid();

    sched_getaffinity(pid, sizeof(cpuSet), &cpuSet);
    EXPECT_TRUE(cpuSet.__bits[0] == 1);

    X86Cpu cpu1{ 1 };
    pid = getpid();

    sched_getaffinity(pid, sizeof(cpuSet), &cpuSet);
    EXPECT_TRUE(cpuSet.__bits[0] == 2);

    X86Cpu cpu2{ 2 };
    pid = getpid();

    sched_getaffinity(pid, sizeof(cpuSet), &cpuSet);
    EXPECT_TRUE(cpuSet.__bits[0] == 4);

    // Verify that an exception is thrown when the core number is greater
    // than the number of phycal cores.
    auto nthreads = std::thread::hardware_concurrency();
    EXPECT_ANY_THROW(X86Cpu cpu3{ nthreads + 1 });
}
} // namespace
