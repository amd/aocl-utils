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

#include "X86RawData.hh"

namespace Au {

/* Precise identification */
#define MAKE_MODEL(base, ext) (((ext) << 4) | (base))

/* Zen, Naples, Whitehaven, Summit Ridge, Snowy Owl */
enum class EUarch : Uint8 // NOLINT
{
    Naples        = MAKE_MODEL(0x1, 0x0),
    Ravenridge    = MAKE_MODEL(0x1, 0x1),
    Bandedkestrel = MAKE_MODEL(0x8, 0x1),
    Dali          = MAKE_MODEL(0x0, 0x2),

    /* Zen+ */
    Pinnacleridge = MAKE_MODEL(0x8, 0x0),
    Picasso       = MAKE_MODEL(0x8, 0x1),

    /* Zen2 */
    Rome    = MAKE_MODEL(0x1, 0x3),
    Renoir  = MAKE_MODEL(0x0, 0x6),
    Matisse = MAKE_MODEL(0x1, 0x7),

    /* Zen3 */
    Zen3_Min = MAKE_MODEL(0x0, 0x0), /* 00 */
    Milan    = MAKE_MODEL(0x1, 0x0), /* 10 */
    Vermeer  = MAKE_MODEL(0x1, 0x2), /* 33 */
    Chagall  = MAKE_MODEL(0x8, 0x0), /* 08 */
    Rembrant = MAKE_MODEL(0x0, 0x4), /* 64 */
    Cezanne  = MAKE_MODEL(0x0, 0x5), /* 80 */
    Zen3_Max = MAKE_MODEL(0x0, 0xF), /* 0f */

    /* Zen4 */
    Zen4_Min    = MAKE_MODEL(0x0, 0x1), /* 0x10 */
    Genoa       = MAKE_MODEL(0x0, 0x1),
    Warhol      = MAKE_MODEL(0x0, 0x1), /* FIXME: ?? */
    Raphael     = MAKE_MODEL(0x0, 0x1),
    DragonRidge = MAKE_MODEL(0x0, 0x2), /* FIXME: ?? */
    Pheonix     = MAKE_MODEL(0x0, 0x3), /* FIXME: ?? */
    Zen4_Max    = MAKE_MODEL(0xf, 0x1), /* 0x1f */

    /* Zen5 - Wikipedia */
    GraniteRidge = 0x0, /* desktop */
    StrixPoint   = 0x0, /* mobile processors  */
    Turin        = 0x0, /* Server */
};

// clang-format off
/**-----------------------------------------------------------------------------------------------------------+
 * |                                      Processor Version Information                                       |
 * -----------------------------------------------------------------------------------------------------------+
 * |                                                    EAX = 1                                               |
 * -----------------------------------------------------------------------------------------------------------+
 * |  31 .. 28  | 27 .. 25 .. 20  |  19 18 17 16  |  15  14  | 13 12    |  11 10 9 8  |  7 6 5 4  |  3 2 1 0  |
 * -----------------------------------------------------------------------------------------------------------+
 * |  Reserved  | Ext Family ID   |  Ext Model ID | Reserved | Cpu Type |  Family ID  |   Model   | Stepping  |
 * -----------------------------------------------------------------------------------------------------------+
 *
 *
 * Family[7:0] = (ExtendedFamily[7:0] + {0000b,BaseFamily[3:0]})
 *       where ExtendedFamily[7:0] = EAX[27:20],
 *             BaseFamily[3:0]     = EAX[11:8]
 */
// clang-format on

X86Cpu::X86Cpu(CpuidUtils* cUtils, CpuNumT num)
    : CpuInfo{ num }
    , mPimpl{ new X86Cpu::Impl{ cUtils } }
{
    pImpl()->update();
}

X86Cpu::X86Cpu(CpuNumT num)
    : CpuInfo{ num }
    , mPimpl{ new X86Cpu::Impl{} }
{
    pImpl()->update();
}

X86Cpu::~X86Cpu() = default;

void
X86Cpu::update()
{
    pImpl()->update();
}

bool
X86Cpu::isAMD() const
{
    return pImpl()->isAMD();
}

bool
X86Cpu::isX86_64v2() const
{
    return pImpl()->isX86_64v2();
}

bool
X86Cpu::isX86_64v3() const
{
    return pImpl()->isX86_64v3();
}

bool
X86Cpu::isX86_64v4() const
{
    return pImpl()->isX86_64v4();
}

bool
X86Cpu::isIntel() const
{
    return pImpl()->isIntel();
}

bool
X86Cpu::hasFlag(ECpuidFlag const& eflag) const
{
    return pImpl()->hasFlag(eflag);
}
} // namespace Au
