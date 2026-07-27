/*
 * Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
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
#include "Au/Misc.hh" /* extract32/valueToEnum, used by updateCacheInfo */
#include "Capi/au/cpuid/cpuid_core.h"

namespace Au {

/* Thin C++ shims over the pure-C core (cpuid_core.h). Enum casts are identity. */

static inline au_cpuid_regs_t
toCRegs(ResponseT const& r)
{
    au_cpuid_regs_t c = { r.eax, r.ebx, r.ecx, r.edx };
    return c;
}

namespace {
    /* Bridge C core raw hook to C++ virtual __raw_cpuid (for mocks/SimNow). */
    void isHybridTrampoline(uint32_t         eax,
                            uint32_t         ecx,
                            au_cpuid_regs_t* out,
                            void*            ctx)
    {
        CpuidUtils* u = static_cast<CpuidUtils*>(ctx);
        RequestT    req{ eax, 0, ecx, 0 };
        ResponseT   r = u->__raw_cpuid(req);
        out->eax      = r.eax;
        out->ebx      = r.ebx;
        out->ecx      = r.ecx;
        out->edx      = r.edx;
    }
} // namespace

ResponseT
CpuidUtils::__raw_cpuid(RequestT& req)
{
    au_cpuid_regs_t out;
    au_cpuid_raw(req.eax, req.ecx, &out);
    return ResponseT{ out.eax, out.ebx, out.ecx, out.edx };
}

EVendor
CpuidUtils::getMfgInfo(ResponseT const& regs)
{
    au_cpuid_regs_t r = toCRegs(regs);
    return static_cast<EVendor>(au_cpuid_vendor(&r));
}

/* CPUID.1 EAX layout: Family = ExtFam[27:20] + BaseFam[11:8]. */

EFamily
CpuidUtils::getFamily(Uint32 var)
{
    return static_cast<EFamily>(au_cpuid_family(var));
}

Uint16
CpuidUtils::getModel(Uint32 var)
{
    return au_cpuid_model(var);
}

Uint16
CpuidUtils::getStepping(Uint32 var)
{
    return au_cpuid_stepping(var);
}

bool
CpuidUtils::hasFlag(ResponseT const& expected, ResponseT const& actual)
{
    au_cpuid_regs_t e = toCRegs(expected);
    au_cpuid_regs_t a = toCRegs(actual);
    return au_cpuid_has_bits(&e, &a);
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

bool
CpuidUtils::isHybrid()
{
    return au_cpuid_read_is_hybrid(&isHybridTrampoline, this);
}

Uint32
CpuidUtils::getCoreType()
{
    return au_cpuid_read_core_type(&isHybridTrampoline, this);
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
