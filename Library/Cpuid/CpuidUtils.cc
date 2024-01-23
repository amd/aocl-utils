#include "Au/Cpuid/CpuidUtils.hh"
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

EFamily
CpuidUtils::getFamily(Uint32 var)
{
    return valueToEnum<EFamily, Uint16>(
        static_cast<Uint16>(extract32(var, 20, 8) + extract32(var, 8, 4)));
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
    return static_cast<Uint16>(extract32(var, 20, 8) + extract32(var, 8, 4));
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
    bool lastLevel = false;
    int  curLevel  = 1;

    while (!lastLevel
           || valueToEnum<CacheLevel, int>(curLevel) > CacheLevel::L5) {
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
