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

#include "Au/Cpuid/CpuidUtils.hh"
#include "Au/Misc.hh"
#include <iostream>
namespace Au {

ResponseT
CpuidUtils::__raw_cpuid(RequestT& req)
{
    ResponseT resp;
    if (req.eax == 0x00000007) {
        asm volatile(
            "cpuid"
            : "=a"(resp.eax), "=b"(resp.ebx), "=c"(resp.ecx), "=d"(resp.edx)
            : "0"(req.eax), "2"(req.ecx));

    } else {
        asm volatile(
            "cpuid"
            : "=a"(resp.eax), "=b"(resp.ebx), "=c"(resp.ecx), "=d"(resp.edx)
            : "0"(req.eax));
    }

    return resp;
}

EVendor
CpuidUtils::getMfgInfo(ResponseT const& regs)
{
    if (regs.ebx == 0x68747541 && regs.ecx == 0x444d4163
        && regs.edx == 0x69746e65) {
        return EVendor::Amd;
    }
    if (regs.ebx == 0x756e6547 && regs.ecx == 0x6c65746e
        && regs.edx == 0x49656e69) {
        return EVendor::Intel;
    }
    return EVendor::Other;
}

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

EFamily
CpuidUtils::getFamily(Uint32 var)
{
    auto family =
        static_cast<Uint16>(extract32(var, 20, 8) + extract32(var, 8, 4));
    if (family < *(EFamily::Zen) || family > *(EFamily::Max))
        return EFamily::Unknown;
    return valueToEnum<EFamily, Uint16>(family);
}

Uint16
CpuidUtils::getModel(Uint32 var)
{
    return static_cast<Uint16>(extract32(var, 16, 4) << 4
                               | extract32(var, 4, 4));
}

Uint16
CpuidUtils::getStepping(Uint32 var)
{
    return static_cast<Uint16>(extract32(var, 0, 4));
}

bool
CpuidUtils::hasFlag(ResponseT const& expected, ResponseT const& actual)
{
    return expected & actual;
}

void
CpuidUtils::updateCacheInfo(CacheInfo& cInfo, ResponseT const& resp)
{
    auto lvl = valueToEnum<CacheLevel, Uint32>(Au::extract32(resp.eax, 5, 3));
    cInfo.setLevel(lvl);

    auto type = valueToEnum<CacheType, Uint32>(Au::extract32(resp.eax, 0, 5));
    cInfo.setType(type);

    auto sets = resp.ecx + 1;
    cInfo.setSets(sets);

    auto lane = extract32(resp.ebx, 0, 12) + 1;
    cInfo.setLane(lane);

    auto way = extract32(resp.ebx, 22, 10) + 1;
    cInfo.setWay(way);

    auto partitions = extract32(resp.ebx, 12, 10) + 1;
    cInfo.setSize(static_cast<Uint64>(way) * (partitions)
                  * static_cast<Uint64>(lane) * (sets));
}

void
CpuidUtils::updateCacheView(CacheView& cView)
{
    int curLevel = 1;

    while (valueToEnum<CacheLevel, int>(curLevel) > CacheLevel::L5) {
        CacheInfo cInfo{ CacheLevel::L1,
                         CacheType::DCache }; /* dummy, will be overriden */
        RequestT  req{ 0x8000'001D, 0, (Uint32)curLevel, 0 };
        ResponseT resp = __raw_cpuid(req);

        if ((resp.eax & 0x1f) == 0x0) /* beyond last cache levels */
            break;

        updateCacheInfo(cInfo, resp);

        curLevel++;
    }
}
} // namespace Au
