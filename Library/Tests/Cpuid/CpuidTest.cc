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
#include "Au/Cpuid/Enum.hh"

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
    X86Cpu            cpu{ 0 };
    auto              flags = readFromFile<String>("FlagsT.txt");
    String            token;
    std::stringstream ss;

    ss << flags;
    while (std::getline(ss, token, ':')) {
        auto flag       = stoi(token);
        auto cpuid_flag = valueToEnum<ECpuidFlag, Uint64>(flag);
        EXPECT_TRUE(cpu.hasFlag(cpuid_flag));
    }
}

TEST(X86Cpuid, DISABLED_hasFlagNegative)
{
    X86Cpu            cpu{ 0 };
    auto              flags = readFromFile<String>("FlagsF.txt");
    String            token;
    std::stringstream ss;

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

    std::vector<Uint32> vendorInfo = {
        enumToValue<EVendor, Uint32>(result.m_mfg),
        enumToValue<EFamily, Uint32>(result.m_family),
        result.m_model,
        result.m_stepping,
        enumToValue<EUarch, Uint32>(result.m_uarch)
    };

    auto iter = vendorInfo.begin();
    writeToFile<std::vector<Uint32>::iterator>(
        "VendorInfoCpp.txt", { iter, iter + 1, iter + 2, iter + 3, iter + 4 });
}
TEST(X86Cpuid, CheckCpuNumber)
{
    // 0 - 2 Verify that the core is set to the correct CPU

    X86Cpu cpu{ 0 };
    checkAffinity(0);

    X86Cpu cpu1{ 1 };
    checkAffinity(1);

    X86Cpu cpu2{ 2 };
    checkAffinity(2);

    // Verify that an exception is thrown when the core number is greater
    // than the number of phycal cores.
    auto nthreads = std::thread::hardware_concurrency();
    EXPECT_ANY_THROW(X86Cpu cpu3{ nthreads + 1 });
}

TEST(X86Cpu, BCTEST)
{
    alci::Cpu core{ 0 };

    std::cout << "----- Platform details -----\n";
    std::cout << "Core info:\n----------\n";
    // The function name changed older api is core.isAmd
    // std::cout << "Is AMD           : " << (core.isAMD() ? "YES" : "NO") <<
    // "\n";
    // The function name and signature changed. older api is getVendor
    // std::cout << "Vendor           : " << core.getVendorInfo() << "\n";
    // The return value of the function changed.
    std::cout << "Architecture     : " << static_cast<Uint32>(core.getUarch())
              << "\n";

    std::cout << "isUarchZen       : "
              << (core.isUarch(Uarch::eZen) ? "YES" : "NO") << "\n";
    std::cout << "isUarchZen2      : "
              << (core.isUarch(Uarch::eZen2) ? "YES" : "NO") << "\n";
    std::cout << "isUarchZen3      : "
              << (core.isUarch(Uarch::eZen3) ? "YES" : "NO") << "\n";
    std::cout << "isUarchZen4      : "
              << (core.isUarch(Uarch::eZen4) ? "YES" : "NO") << "\n";

    std::cout << "Features supported:\n-------------------\n";
    std::cout << "AVX support   : "
              << (core.isAvailable(ALC_E_FLAG_AVX) ? "YES" : "NO") << "\n";
    std::cout << "AVX2 support  : "
              << (core.isAvailable(ALC_E_FLAG_AVX2) ? "YES" : "NO") << "\n";
    std::cout << "AVX512 support: "
              << (core.isAvailable(ALC_E_FLAG_AVX512F) ? "YES" : "NO") << "\n";

    if (core.isAvailable(ALC_E_FLAG_AVX512F)) {
        std::cout << "  AVX512DQ         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512DQ) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512PF         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512PF) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512ER         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512ER) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512CD         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512CD) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512BW         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512BW) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512VL         : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512VL) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_IFMA      : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_IFMA) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_VNNI      : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_VNNI) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_BITALG    : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_BITALG) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_VBMI      : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_VBMI) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_VBMI2     : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_VBMI2) ? "YES" : "NO")
                  << "\n";
        std::cout << "  AVX512_VPOPCNTDQ : "
                  << (core.isAvailable(ALC_E_FLAG_AVX512_VPOPCNTDQ) ? "YES"
                                                                    : "NO")
                  << "\n";
    }
}
} // namespace
