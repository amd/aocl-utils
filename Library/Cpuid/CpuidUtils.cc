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
CpuidUtils::__update_vendor_info(ResponseT const& regs)
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

void
CpuidUtils::__update_mfg_info(ResponseT const& resp)
{
}

bool
CpuidUtils::__has_flag(ResponseT const& expected, ResponseT const& actual)
{
    return expected & actual;
}

/*
using CacheLevel = CacheInfo::CacheLevel;
using CacheType  = CacheInfo::CacheType;
CacheLevel
InttoLevel(Uint32 lvl)
{
    switch (lvl) {
            // clang-format off
        case 1:return CacheLevel::L1;break;
        case 2:return CacheLevel::L2;break;
        case 3:return CacheLevel::L3;break;
        default:AUD_ASSERT(true, "Invalid Cache Level");break;
            // clang-format on
    }

    return CacheLevel::Unknown;
}

CacheType
InttoType(Uint32 tp)
{
    switch (tp) {
            // clang-format off
        case 1:return CacheType::DCache;break;
        case 2:return CacheType::ICache;break;
        case 3:return CacheType::Unified;break;
        default:AUD_ASSERT(true, "Invalid Cache Type");break;
            // clang-format on
    }
    return CacheType::Unknown;
}

void
update_cache_info(CacheInfo& ci, ResponseT const& resp)
{
    ci.setType(InttoType(Au::extract32(resp.eax, 0, 5)));

    auto lvl = InttoLevel(Au::extract32(resp.eax, 5, 3));
    ci.setLevel(lvl);

    auto sets = resp.ecx + 1;
    ci.setSets(sets);

    auto lane = extract32(resp.ebx, 0, 12) + 1;
    ci.setLane(lane);

    auto way = extract32(resp.ebx, 22, 10) + 1;
    ci.setWay(way);

    auto partitions = extract32(resp.ebx, 12, 10) + 1;
    ci.setSize(way * partitions * lane * sets);
}

void
update_cache_view(CacheView& cv)
{
    bool last_level = false;
    int  cur_level  = 1;

    while (!last_level || InttoLevel(cur_level) > CacheLevel::L5) {
        CacheInfo ci{ CacheLevel::L1,
                      CacheType::DCache }; /* dummy, will be overriden */
/*      RequestT  req{ 0x8000'001D, 0, (Uint32)cur_level, 0 };
      ResponseT resp = __raw_cpuid(req);

      if ((resp.eax & 0x1f) == 0x0) /* beyond last cache levels */
/*        break;

update_cache_info(ci, resp);

    cur_level++;
}
}*/
} // namespace Au
