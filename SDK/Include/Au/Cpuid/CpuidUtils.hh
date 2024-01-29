
#pragma once

#include "Au/Assert.hh"
#include "Au/Au.hh"
#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Misc.hh"
#include "Au/Types.hh"

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
    EVendor getMfgInfo(ResponseT const& regs);
    /**
     * \brief       Get Family ID from given 32-bit input value.
     *
     * Family[7:0] = (ExtendedFamily[7:0] + {0000b,BaseFamily[3:0]})
     * where ExtendedFamily[7:0] = var[27:20], BaseFamily[3:0] = var[11, 8]
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Family ID value.
     */
    EFamily getFamily(Uint32 var);
    /**
     * \brief       Get Model number from given input value.
     *
     *  Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
     *  where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Model number value.
     */
    Uint16 getModel(Uint32 var);
    /**
     * \brief       Get Stepping ID from given input value.
     *
     * Get Stepping ID from given input value.
     * Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
     * where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
     * For now, model number is stepping. TODO: need to revisit for each family.
     *
     * \param[in]   var  32-bit value.
     *
     * \return      Returns Stepping ID value.
     */
    Uint16 getStepping(Uint32 var);
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
    bool hasFlag(ResponseT const& expected, ResponseT const& actual);
    void updateCacheView(CacheView& cView);
    void updateCacheInfo(CacheInfo& cInfo, ResponseT const& resp);
};
} // namespace Au
