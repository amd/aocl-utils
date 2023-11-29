/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Cpuid/X86Cpu.hh"

#include <iostream>

using namespace Au;
using namespace std;

static void
display_core_info()
{
    X86Cpu c{ 0 };

    cout << "Vendor: " << c.getName();

    cout << "Cpu is " << (c.isAMD() ? "AMD" : "Other");
    cout << "Freq: " << c.getFreq();
}

static void
display_cache_info()
{
    /* demonstration purpose, get info of 10th core */
    constexpr auto corenum = 10;
    X86Cpu         c{ corenum };
    auto           cv = c.getCacheView();

    cout << "Core: " << corenum;
    cout << "Cache Levels: " << cv.getNumLevels();
    for (auto& a : cv) {
        cout << "Level: " << a.getLevel() << "\t Type: " << a.getType();
        cout << "Size : " << a.getSize();
        cout << "Sets : " << a.getSets() << "\t Way: " << a.getWay()
             << "\t Lane: " << a.getLane();
    }
}

static void
display_isa_info()
{
}

int
main(void)
{
    display_core_info();

    display_cache_info();

    display_isa_info();

    return 0;
}
