/*
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Au/Cpuid/X86Cpu.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace Au;

/**
 * Test parameters for X86Cpu Mock Test
 * Vector containing the following parameters:
 * 1. Name of the CPU model to be mocked using simnowdata
 * 2. Vector containing the following parameters:
 *    1. Name of the CPU model to be mocked using simnowdata
 *    2. Vector of boolean values that marks the expected results of
 *       {is_AMD, is_Intel, is_X86_64v2, is_X86_64v3, is_X86_64v4}  APIs
 *     and true for hasFlags(Positive/Negative) and isUarch api tests
 * 3. Expected architecture of the CPU
 */
// clang-format off
auto isAmd=true, isIntel=true, isX86_64v2=true, isX86_64v3=true, isX86_64v4=true, flagPresent=true,flagAbsent=true, isUarch=true, isZenFamily=true;
const std::vector<std::tuple<String, std::vector<bool>, EUarch>> testParametersX86Cpu = {
    // Zen5 server models (Family 0x1A)
    { "EPYC-Turin-Dense-v2", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "EPYC-Turin-Dense-v1", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "EPYC-Turin-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },

    // Zen5 client/mobile/desktop models (Family 0x1A)
    { "Shimada-Peak-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Strix-Point-v1",    { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Strix-Point-v2",    { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Strix-Halo-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Strix-Halo-v2",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Gorgon-Point-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Granite-Ridge-v1",  { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Grado-v1",          { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Fire-Range-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Krackan-Point-v1",  { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Krackan-Point-v2",  { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    { "Sarlak-v1",         { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
    
    // Zen4 server models (Family 0x19)
    { "EPYC-Genoa-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Storm-Peak-V1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },

    // Zen4 client/desktop/server models (Family 0x19)
    { "Dragon-Range-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Raphael-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Phoenix-v1",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Phoenix-v2",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Phoenix2-v1",       { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Phoenix2-v2",       { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Ryzen-MI300C-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Ryzen-MI300A-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Bergamo-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Stones-Dense-v1",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Siena-v1",          { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "EPYC-Milan-v1",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "EPYC-Milan-v2",     { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },

    // Zen3 client/desktop models (Family 0x19)
    { "Genesis-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Chagall-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Vermeer-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Vermeer-v2",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Trento-v1",         { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Rembrandt-v1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Rembrandt-v2",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Cezanne-v1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "EPYC-Rome-v1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "EPYC-Rome-v2",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "EPYC-Rome-v3",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "EPYC-Rome-v4",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },

    // Zen2 client/desktop models (Family 0x17)
    { "Castle-Peak-V1",    { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Project-X-V1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Cardinal-V1",       { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Grey-Hawk-V1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Renoir-V1",         { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Lucienne-V1",       { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Matisse-V1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Van-Gogh-V1",       { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Mero-V1",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Mendocino-V1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },

    { "EPYC-v1",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "EPYC-v2",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "EPYC-v3",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "EPYC-v4",           { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },

    // Zen+ client models (Family 0x17)
    { "Picasso-V1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Colfax-V1",         { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Pinnacle-Ridge-V1", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },

    // Zen client/desktop models (Family 0x17)
    { "Summit-Ridge-V1",      { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Whitehaven-V1",        { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Snowy-Owl-V1",         { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Great-Horned-Owl-V1",  { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Raven-V1",             { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Banded-Kestrel-V1",    { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Raven-V2",             { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    
    // Feature flag fallback tests (unknown model numbers using flag-based detection)
    { "Mock-Zen17-Model25-CLWB",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen2 },
    { "Mock-Zen17-Model25-NoCLWB", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Mock-Zen17-Model25-NoFlags",{ isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen },
    { "Mock-Zen19-ModelB5-AVX512", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen4 },
    { "Mock-Zen19-ModelB5-VAES",   { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Mock-Zen19-ModelB5-NoFlags",{ isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen3 },
    { "Opteron_G1-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
    { "Opteron_G2-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
    { "Opteron_G3-v1",     { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
    { "Opteron_G4-v1",     { isAmd,  !isIntel, isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
    { "Opteron_G5-v1",     { isAmd,  !isIntel, isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
    { "phenom-v1",         { isAmd,  !isIntel, !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily}, EUarch::Unknown },
 //   { "Broadwell-v1",      { !isAmd, isIntel,  isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily }, EUarch::Unknown },
    { "Denverton-v1",      { !isAmd, isIntel,  isX86_64v2,  !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily }, EUarch::Unknown },
    { "Conroe-v1",         { !isAmd, isIntel,  !isX86_64v2, !isX86_64v3, !isX86_64v4, flagPresent, flagAbsent, isUarch, !isZenFamily }, EUarch::Unknown },
   // { "Skylake-Server-v1", { !isAmd, isIntel,  isX86_64v2,  isX86_64v3,  isX86_64v4,  flagPresent, flagAbsent, isUarch, !isZenFamily }, EUarch::Unknown }
    { "MockFutureArch-v1", { isAmd,  !isIntel, isX86_64v2,  isX86_64v3,  !isX86_64v4, flagPresent, flagAbsent, isUarch, isZenFamily}, EUarch::Zen5 },
};
// clang-format on
/**
 * Test parameters for CpuidUtils Mock Test
 * Vector contains the following parameters:
 * 1. Name of the CPU model to be mocked using simnowdata
 * 2. VendorInfo structure containing the expected results.
 */
// clang-format off
const std::vector<std::tuple<String, VendorInfo>> testParametersCpuidUtils = {
    { "EPYC-Turin-Dense-v2", { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x10, 0x0 } } },
    { "EPYC-Turin-Dense-v1", { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x11, 0x0 } } },
    { "EPYC-Turin-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x02, 0x0 } } },

    // Zen5 client/mobile/desktop models (Family 0x1A)
    { "Shimada-Peak-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x08, 0x0 } } },
    { "Strix-Point-v1",    { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x24, 0x0 } } },
    { "Strix-Point-v2",    { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x30, 0x0 } } },
    { "Strix-Halo-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x38, 0x0 } } },
    { "Strix-Halo-v2",     { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x70, 0x0 } } },
    { "Gorgon-Point-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x40, 0x0 } } },
    { "Granite-Ridge-v1",  { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x44, 0x0 } } },
    { "Grado-v1",          { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x44, 0x0 } } },
    { "Fire-Range-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x48, 0x0 } } },
    { "Krackan-Point-v1",  { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x60, 0x0 } } },
    { "Krackan-Point-v2",  { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x68, 0x0 } } },
    { "Sarlak-v1",         { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x77, 0x0 } } },
    
    // Zen4 client/desktop/server models (Family 0x19)
    { "EPYC-Genoa-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x11, 0x1 } } },
    { "Storm-Peak-V1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x18, 0x0 } } },
    { "Dragon-Range-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x61, 0x0 } } },
    { "Raphael-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x61, 0x0 } } },
    { "Phoenix-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x74, 0x0 } } },
    { "Phoenix-v2",        { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x75, 0x0 } } },
    { "Phoenix2-v1",       { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x78, 0x0 } } },
    { "Phoenix2-v2",       { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x7c, 0x0 } } },
    { "Ryzen-MI300C-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x80, 0x0 } } },
    { "Ryzen-MI300A-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0x90, 0x0 } } },
    { "Bergamo-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0xa0, 0x0 } } },
    { "Stones-Dense-v1",   { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0xa0, 0x0 } } },
    { "Siena-v1",          { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0xa0, 0x0 } } },
    
    // Zen3 server models (Family 0x19)
    { "EPYC-Milan-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x01, 0x1 } } },
    { "EPYC-Milan-v2",     { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x01, 0x2 } } },

    // Zen3 client/desktop models (Family 0x19)
    { "Genesis-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x00, 0x0 } } },
    { "Chagall-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x08, 0x0 } } },
    { "Vermeer-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x21, 0x0 } } },
    { "Vermeer-v2",        { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x21, 0x2 } } },
    { "Trento-v1",         { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x31, 0x1 } } },
    { "Rembrandt-v1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x40, 0x0 } } },
    { "Rembrandt-v2",      { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x44, 0x1 } } },
    { "Cezanne-v1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0x50, 0x0 } } },

    // Zen2 server models (Family 0x17)
    { "EPYC-Rome-v1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0 } } },
    { "EPYC-Rome-v2",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0 } } },
    { "EPYC-Rome-v3",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0 } } },
    { "EPYC-Rome-v4",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0 } } },

    // Zen2 client/desktop models (Family 0x17)
    { "Castle-Peak-V1",    { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x31, 0x0 } } },
    { "Project-X-V1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x84, 0x0 } } },
    { "Cardinal-V1",       { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x47, 0x0 } } },
    { "Grey-Hawk-V1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x60, 0x0 } } },
    { "Renoir-V1",         { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x60, 0x0 } } },
    { "Lucienne-V1",       { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x68, 0x0 } } },
    { "Matisse-V1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x71, 0x0 } } },
    { "Van-Gogh-V1",       { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x90, 0x0 } } },
    { "Mero-V1",           { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0x98, 0x0 } } },
    { "Mendocino-V1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen2,    0xa0, 0x0 } } },

    { "EPYC-v1",           { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x2 } } },
    { "EPYC-v2",           { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x2 } } },
    { "EPYC-v3",           { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x2 } } },
    { "EPYC-v4",           { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x2 } } },

    // Zen+ client models (Family 0x17)
    { "Picasso-V1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x18, 0x0 } } },
    { "Colfax-V1",         { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x08, 0x0 } } },
    { "Pinnacle-Ridge-V1", { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x08, 0x0 } } },

    // Zen client/desktop models (Family 0x17)
    { "Summit-Ridge-V1",      { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x0 } } },
    { "Whitehaven-V1",        { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x0 } } },
    { "Snowy-Owl-V1",         { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x01, 0x0 } } },
    { "Great-Horned-Owl-V1",  { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x11, 0x0 } } },
    { "Raven-V1",             { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x11, 0x0 } } },
    { "Banded-Kestrel-V1",    { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x18, 0x0 } } },
    { "Raven-V2",             { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x20, 0x0 } } },

    // Feature flag fallback tests (unknown model numbers using flag-based detection)
    { "Mock-Zen17-Model25-CLWB",    { VendorInfo{ EVendor::Amd,   EFamily::Zen2,     0x25, 0x0 } } },
    { "Mock-Zen17-Model25-NoCLWB",  { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x25, 0x0 } } },
    { "Mock-Zen17-Model25-NoFlags", { VendorInfo{ EVendor::Amd,   EFamily::Zen,     0x25, 0x0 } } },
    { "Mock-Zen19-ModelB5-AVX512",  { VendorInfo{ EVendor::Amd,   EFamily::Zen4,    0xB5, 0x0 } } },
    { "Mock-Zen19-ModelB5-VAES",    { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0xB5, 0x0 } } },
    { "Mock-Zen19-ModelB5-NoFlags", { VendorInfo{ EVendor::Amd,   EFamily::Zen3,    0xB5, 0x0 } } },
    { "Opteron_G1-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x06, 0x1 } } },
    { "Opteron_G2-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x06, 0x1 } } },
    { "Opteron_G3-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x3 } } },
    { "Opteron_G4-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x01, 0x2 } } },
    { "Opteron_G5-v1",     { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x0 } } },
    { "phenom-v1",         { VendorInfo{ EVendor::Amd,   EFamily::Unknown, 0x02, 0x3 } } },
    { "Broadwell-v1",      { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0X3D, 0x2 } } },
    { "Denverton-v1",      { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x5F, 0x1 } } },
    { "Conroe-v1",         { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x0F, 0x3 } } },
    { "Skylake-Server-v1", { VendorInfo{ EVendor::Intel, EFamily::Unknown, 0x55, 0x4 } } },
    { "MockFutureArch-v1", { VendorInfo{ EVendor::Amd,   EFamily::Zen5,    0x11, 0x0 } } },
    // clang-format on
};

/**
 *  The MockCpuidUtils class is used to mock the CpuidUtils class
 */
class MockCpuidUtils : public CpuidUtils
{
  public:
    MockCpuidUtils()
        : CpuidUtils()
    {
    }
    MOCK_METHOD(ResponseT, __raw_cpuid, (RequestT & req), (override)){};
};

/**
 * The Base class for both X86Cpu and CpuidUtils Mock tests
 * It contains the common functions and data for both the tests
 */
class MockCpuidBase : public testing::Test
{
  private:
    /**
     * @brief parseCSV function is used to parse the CSV file and populate the
     * map with the request and response data
     * The format of the file is assumed to be:
     * {1,0,0,0}:{329300,2048,4294586883,126614525}
     * where the first{} represents the request and the second{} represents the
     * response
     * @param filename is the name of the file to be parsed
     *
     * @return map<RequestT, ResponseT> is the map containing the request and
     * response data
     */
    std::map<RequestT, ResponseT> parseCSV(const String& filename)
    {
        std::map<RequestT, ResponseT> data;

        std::cout << "Processing" << filename << std::endl;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return data; // Return an empty vector if the file cannot be opened
        }

        String line;
        while (getline(file, line)) {
            std::istringstream lineStream(line);
            String             requestStr, respStr;

            // Assuming the CSV structure is
            // {1,0,0,0}:{329300,2048,4294586883,126614525}
            getline(lineStream, requestStr, ':');
            getline(lineStream, respStr, ':');
            // Extract values between curly braces and populate structures
            ResponseT request{}; // The request needs to be updated, hence not
                                 // using RequestT here.
            ResponseT response{};

            sscanf(requestStr.c_str(),
                   "{%x,%x,%x,%x}",
                   &request.eax,
                   &request.ebx,
                   &request.ecx,
                   &request.edx);
            sscanf(respStr.c_str(),
                   "{%x,%x,%x,%x}",
                   &response.eax,
                   &response.ebx,
                   &response.ecx,
                   &response.edx);

            data[request] = response;
        }

        file.close();
        return data;
    }

  protected:
    /**
     * @brief Configure  Mocks the __raw_cpuid function by specifying the
     * expected request and response data by parsing the CSV file corresponding
     * to the CPU.
     *
     */
    std::map<RequestT, ResponseT> Configure()
    {
        String projectDir   = PROJECT_SOURCE_DIR;
        String testDir      = "/Library/Tests/Cpuid/Mock/simnowdata/";
        String dataFilename = projectDir + testDir + filename + "/" + filename;

        std::map<RequestT, ResponseT> csvData = parseCSV(dataFilename);
        for (const auto& entry : csvData) {
            ON_CALL(mockCpuidUtils, __raw_cpuid(entry.first))
                .WillByDefault(testing::Return(entry.second));
        }
        return csvData;
    }

  public:
    MockCpuidBase()
        : mockCpuidUtils()
        , filename{}
    {
    }
    MockCpuidUtils mockCpuidUtils;
    String         filename;
};
} // namespace
