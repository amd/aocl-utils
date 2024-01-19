
#pragma once

#include "Au/Assert.hh"
#include "Au/Au.hh"
#include "Au/Types.hh"

#include "Au/Cpuid/CacheInfo.hh"
#include <iostream>
#include <type_traits>
using namespace std;

namespace Au {
/* ID return values */
struct CpuidRegs
{
    Uint32 eax;
    Uint32 ebx;
    Uint32 ecx;
    Uint32 edx;

    /* following are required for making this key in a std::map */
    bool operator==(CpuidRegs const& Reg) const
    {
        return eax == Reg.eax && ebx == Reg.ebx && ecx == Reg.ecx
               && edx == Reg.edx;
    }

    bool operator<(CpuidRegs const& Reg) const
    {
        return eax < Reg.eax || ebx < Reg.ebx || ecx < Reg.ecx || edx < Reg.edx;
    }

    bool operator&(CpuidRegs const& Reg) const
    {
        return ((eax & Reg.eax) != 0U) || ((ebx & Reg.ebx) != 0U)
               || ((ecx & Reg.ecx) != 0U) || ((edx & Reg.edx) != 0U);
    }
};
using RequestT  = const CpuidRegs;
using ResponseT = CpuidRegs;

using CacheLevel = CacheInfo::CacheLevel;
using CacheType  = CacheInfo::CacheType;
/**
 * @enum  Vendor
 * @brief CPU vendors.
 */
enum class EVendor : Uint32
{
    Amd = 1, /**< AMD. */
    Intel,   /**< Intel. */
    Other    /**< Others. */
};

/* Processor family info */
enum class EFamily : Uint16
{
    Zen      = 0x17,
    Zen_Plus = 0x17,
    Zen2     = 0x17,
    Zen3     = 0x19,
    Zen4     = 0x19,
    Max      = 0x19, /* Always set to latest family ID */
};

/**
 * @struct  VendorInfo
 * @brief   CPU core info.
 */
class VendorInfo
{
    /* TODO: Make this private and provide accessors */
  public:
    EVendor mMfg;      /**< CPU manufacturing vendor. */
    EFamily mFamily;   /**< CPU family ID. */
    Uint16  mModel;    /**< CPU model number. */
    Uint16  mStepping; /**< CPU stepping. */
};

class CpuidUtils
{
  public:
    virtual ~CpuidUtils() {}
    /**
     * \brief   Function to query CPUID instruction based on EAX input
     * parameter.
     *
     * Function which has assembly code to query CPUID instruction
     * based on EAX input value. EAX should have valid value to
     * get CPUID information and other EBX, ECX, EDX should have 0s.
     * In some cases, ECX also will have valid values.
     *
     * \param[in] req Request structure containing EAX, EBX, ECX, EDX values.
     * \param[out] resp regs pointer which has EAX, EBX, ECX, EDX values.
     */
    virtual ResponseT __raw_cpuid(RequestT& req);
    /**
     * \brief   Get CPU Vendor info from CPUID instruction.
     *
     * It identifies AMD and Intel Cpus. Tags anyother vendor as Other.
     *
     * \param[in]  ResponseT    The CPUID Register Data.
     * @return The EVendor [AMD, Intel or Other]
     */
    EVendor __update_vendor_info(ResponseT const& regs);
    /**
     * \brief   Get CPU manufacturing info from CPUID instruction.
     *
     * It gives CPU Family, Model & Stepping.
     *
     * \param[in]   cpuid_regs  CpuId registers result values.
     * \param[out]  mfg_info    Updates CPU manufacturing info.
     */
    void __update_mfg_info(ResponseT const& resp);
    /**
     * @details Issues the cpuid instruction using EAX/ECX gets response and
     * checks a flag in appropriate register
     *
     * @param[in] expected Expected bit(s) to check
     *
     * @param[in] actual   Actula value of E{A,B,C,D}X after 'cpuid' issued
     *
     * @return true if cpu has flag, false otherwise
     */
    bool __has_flag(ResponseT const& expected, ResponseT const& actual);
};
} // namespace Au
