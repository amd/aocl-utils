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

const vector<tuple<string, vector<bool>>> testParametersX86Cpu = {
    { "EPYC-Genoa-v1", { true, false, true, true, false } },
    { "EPYC-Milan-v1", { true, false, true, true, false } },
    { "EPYC-Milan-v2", { true, false, true, true, false } },
    { "EPYC-Rome-v1", { true, false, true, true, false } },
    { "EPYC-Rome-v2", { true, false, true, true, false } },
    { "EPYC-Rome-v3", { true, false, true, true, false } },
    { "EPYC-Rome-v4", { true, false, true, true, false } },
    { "EPYC-v1", { true, false, true, true, false } },
    { "EPYC-v2", { true, false, true, true, false } },
    { "EPYC-v3", { true, false, true, true, false } },
    { "EPYC-v4", { true, false, true, true, false } },
    { "Opteron_G1-v1", { true, false, false, false, false } },
    { "Opteron_G2-v1", { true, false, false, false, false } },
    { "Opteron_G3-v1", { true, false, false, false, false } },
    { "Opteron_G4-v1", { true, false, true, false, false } },
    { "Opteron_G5-v1", { true, false, true, false, false } },
    { "phenom-v1", { true, false, false, false, false } },
    { "Broadwell-v1", { false, true, true, true, false } },
    { "Denverton-v1", { false, true, true, false, false } },
    { "Conroe-v1", { false, true, false, false, false } },
    { "Skylake-Server-v1", { false, true, true, true, true } }
};
} // namespace
