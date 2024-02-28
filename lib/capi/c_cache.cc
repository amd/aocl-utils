/*
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
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

#include "alci/alci.h"

#include "alci/cxx/alci.hh"
#include "alci/cxx/cache.hh"
#include "alci/cxx/cpu.hh"

using namespace alci;

EXTERN_C_BEGIN

/**
 * \brief       Provides cache type of cxx using c based enum.
 *
 * \param[in]   type  Cache type.
 *
 * \return      Returns cache type of cxx.
 */
static alci::CacheType
type_to_cxx(alc_cache_type_t type)
{
    CacheType  ctp;

    switch (type) {
        case ALC_E_TYPE_ICACHE:
            ctp = CacheType::eInstruction;
            break;
        case ALC_E_TYPE_DCACHE:
            ctp = CacheType::eData;
            break;
        case ALC_E_TYPE_UNIFIED:
            ctp = CacheType::eUnified;
            break;
        default:
            ctp = CacheType::eUnified;
            break;
    }

    return ctp;
}

/**
 * \brief       Provides cache level of cxx using c based enum.
 *
 * \param[in]   level   Cache level.
 *
 * \return      Returns cache level of cxx.
 */
static alci::CacheLevel
level_to_cxx(alc_cache_level_t level)
{
    CacheLevel clvl;

    switch (level) {
        case ALC_E_L1:
            clvl = CacheLevel::e_Level_1;
            break;
        case ALC_E_L2:
            clvl = CacheLevel::e_Level_2;
            break;
        case ALC_E_L3:
            clvl = CacheLevel::e_Level_3;
            break;
        case ALC_E_L4:
            clvl = CacheLevel::e_Level_4;
            break;
        case ALC_E_L5:
            clvl = CacheLevel::e_Level_5;
            break;
        default:
            clvl = CacheLevel::e_Level_Unknown;
    }
    return clvl;
}

/**
 * \brief       Provides cache param of cxx using c based enum.
 *
 * \param[in]   param   Cache param.
 *
 * \return      Returns cache param of cxx.
 */
static alci::CacheParam
param_to_cxx(alc_cache_param_t param)
{
    CacheParam cprm;

    switch (param) {
        case ALC_E_PARAM_SIZE:
            cprm = CacheParam::eSize;
            break;
        case ALC_E_PARAM_NUMSETS:
            cprm = CacheParam::eNumSets;
            break;
        case ALC_E_PARAM_NUMWAYS:
            cprm = CacheParam::eNumWays;
            break;
        case ALC_E_PARAM_LINESIZE:
            cprm = CacheParam::eLineSize;
            break;
        case ALC_E_PARAM_PHYSPARTITIONS:
            cprm = CacheParam::ePhysPartitions;
            break;
        case ALC_E_PARAM_NUMSHARINGCACHE:
            cprm = CacheParam::eNumSharingCache;
            break;
        case ALC_E_PARAM_FULLYASSOC:
            cprm = CacheParam::eFullyAssoc;
            break;
        case ALC_E_PARAM_SELFINIT:
            cprm = CacheParam::eSelfInit;
            break;
        case ALC_E_PARAM_INCLUSIVE:
            cprm = CacheParam::eInclusive;
            break;
        default:
            cprm = CacheParam::eUnknown;
    }
    return cprm;
}

/**
 * \brief       Provides cache value of the given cache level, type
 *              and paramter.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[in]   param     Cache parameter to get value.
 * \param[out]  value     Pointer to the cache param value.
 *
 * \retval      ALC_E_SUCCESS   If valid cache value is found.
 * \retval      ALC_E_FAILURE   If valid cache value is not found.
 */
alc_cpuid_error_t
alci_cache_get_info(alc_core_num_t      core_num,
                    alc_cache_level_t   level,
                    alc_cache_type_t    type,
                    alc_cache_param_t   param,
                    uint64_t*           value)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;

    if (value) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        status = cinfo.getInfo(level_to_cxx(level), type_to_cxx(type),
                              param_to_cxx(param), value);
    }
    return status;
}

/**
 * \brief       Get cache size of the given cache level and type.
 *
 * Provides cache size in KB.
 *
 * \param[in]   level Cache level.
 * \param[in]   type  Cache type.
 *
 * \return      If size > 0, returns valid cache size found (in KB). Otherwise,
 *              returns 0.
 */
uint64_t
alcpu_cache_get_size(alc_cache_level_t level, alc_cache_type_t type)
{
    alci::Cpu c{ 0 };

    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    return cinfo.getSize(level_to_cxx(level), type_to_cxx(type));
}

/**
 * \brief Get cache size of the given cache level and type for specific core.
 *
 * Provides cache size in KB.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  size      Cache size returned (in KB).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_size(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    uint64_t*         size)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (size) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *size = cinfo.getSize(level_to_cxx(level), type_to_cxx(type));
        if (*size != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get cache sets of the given cache level and type for specific core.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  numsets   Cache number of sets returned (in bytes).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_num_sets(alc_core_num_t    core_num,
                        alc_cache_level_t level,
                        alc_cache_type_t  type,
                        uint64_t*         numsets)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (numsets) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *numsets = cinfo.getNumSets(level_to_cxx(level), type_to_cxx(type));
        if (*numsets != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get cache ways of the given cache level and type for specific core.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  way       Cache way returned (in bytes).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_way(alc_core_num_t    core_num,
                   alc_cache_level_t level,
                   alc_cache_type_t  type,
                   size_t*           way)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (way) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *way = static_cast<size_t>(cinfo.getWay(level_to_cxx(level),
                                            type_to_cxx(type)));
        if (*way != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get cache lanes of the given cache level and type for specific core.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  lane      Cache lane returned (in bytes).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_lane(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    size_t*           lane)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (lane) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *lane = static_cast<size_t>(cinfo.getLane(level_to_cxx(level),
                                            type_to_cxx(type)));
        if (*lane != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get Cache physical line partitions of the given cache level and type
 *        for specific core.
 *
 * \param[in]   core_num    Core number of Cpu.
 * \param[in]   level       Cache level.
 * \param[in]   type        Cache type.
 * \param[out]  partitions  Cache physical line partitions.
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_partitions(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    uint32_t*         partitions)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (partitions) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *partitions = cinfo.getPartitions(level_to_cxx(level), type_to_cxx(type));
        if (*partitions != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get Number of logical processors sharing cache for given
 *        cache level and type for specific core.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  numlp     Number of logical processors sharing cache.
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_num_sharing_cache(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    uint32_t*         numlp)
{
    Cpu c{core_num};
    alc_cpuid_error_t status = ALC_E_FAILURE;
    
    if (numlp) {
        auto cinfo = c.getCacheInfo(level_to_cxx(level));
        *numlp = cinfo.getNumSharingCache(level_to_cxx(level), type_to_cxx(type));
        if (*numlp != 0)
            status = ALC_E_SUCCESS;
    }
    return status;
}

/**
 * \brief Get fully associative cache details for given cache level and type
 *        for specific core.
 *
 * fullassoc flag:  If cache is fully associative, then sets 1.
 *                  Else 0 will be set.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  fullassoc Fully associative flag.
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_is_fully_assoc(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    bool*             fullassoc)
{
    Cpu c{core_num};
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *fullassoc = cinfo.isFullAssoc(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

/**
 * \brief Get self initializing cache details for given cache level and type
 *        for specific core.
 *
 * selfinit flag: Sets 1 if Cache is self initializing; and cache does not need
 *                software initialization. Sets 0 otherwise.
 *
 * \param[in]   core_num  Core number of Cpu.
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[out]  selfinit  Self Initialization flag.
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_is_self_init(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    bool*             selfinit)
{
    Cpu c{core_num};
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *selfinit = cinfo.isSelfInit(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

/**
 * \brief Get cache inclusive or exclusive for given cache level and type
 *        for specific core.
 *
 * inclusive flag: Sets 1 if cache is inclusive of lower cache levels;
 *                 Sets 0 if cache is not inclusive (exclusive) of 
 *                 lower cache levels.
 *
 * \param[in]   core_num    Core number of Cpu.
 * \param[in]   level       Cache level.
 * \param[in]   type        Cache type.
 * \param[out]  inclusive   Cache Inclusive flag.
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_is_inclusive(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    bool*             inclusive)
{
    Cpu c{core_num};
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *inclusive = cinfo.isInclusive(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

EXTERN_C_END
