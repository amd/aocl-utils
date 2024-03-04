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
auto isAmd=true, isIntel=true, isX86_64v2=true, isX86_64v3=true, isX86_64v4=true, hasFlagsT=true,hasFlagsF=true, isUarch=true;
const vector<tuple<String, vector<bool>, EUarch>> testParametersX86Cpu = {
    { "EPYC-Genoa-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen4 },
    { "EPYC-Milan-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen3 },
    { "EPYC-Milan-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen3 },
    { "EPYC-Rome-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v3", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-Rome-v4", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen2 },
    { "EPYC-v1", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v2", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v3", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "EPYC-v4", { isAmd, !isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, isUarch }, EUarch::Zen },
    { "Opteron_G1-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Opteron_G2-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Opteron_G3-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Opteron_G4-v1", { isAmd, !isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Opteron_G5-v1", { isAmd, !isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "phenom-v1", { isAmd, !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Broadwell-v1", { !isAmd, isIntel, isX86_64v2, isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Denverton-v1", { !isAmd, isIntel, isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT , hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Conroe-v1", { !isAmd, isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown },
    { "Skylake-Server-v1", { !isAmd, isIntel, isX86_64v2, isX86_64v3, isX86_64v4, hasFlagsT, hasFlagsF, !isUarch }, EUarch::Unknown }
};
// clang-format on
} // namespace
