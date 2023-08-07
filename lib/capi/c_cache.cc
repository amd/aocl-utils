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

constexpr int ALCI_KB = 1024;

EXTERN_C_BEGIN

/**
 * \brief       Provides pointer to cache info of the given cache level & type.
 *
 * \param[out]  cacheinfo Pointer to the cache information in alc_cache_infop
 * structure. \param[in]   level     Cache level. \param[in]   type      Cache
 * type.
 *
 * \retval      ALC_E_SUCCESS      If valid cache information is found.
 * \retval      ALC_E_FAILURE      If valid cache information is not found.
 */
alc_cpuid_error_t
alcpu_cache_get_info(alc_cache_infop   cacheinfo,
                     alc_cache_level_t level,
                     alc_cache_type_t  type)
{
    /* todo: update cacheinfo

    auto cachedata = alci::CacheInfo();

    if (!cachedata.update((alci::Level)level, (alci::Type)type))
        return ALC_E_FAILURE;

    cacheinfo->cachesize     = cachedata.m_size;
    cacheinfo->cachenumsets  = cachedata.m_set;
    cacheinfo->cachelevel    = (uint32_t)cachedata.m_level;
    cacheinfo->cachetype     = (uint32_t)cachedata.m_type;
    cacheinfo->cachenumways  = cachedata.m_way;
    cacheinfo->cachelinesize = cachedata.m_line;
    */
    return ALC_E_SUCCESS;
}


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
            clvl = CacheLevel::e_Level_3;
    }
    return clvl;
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
 * returns 0.
 */
uint64_t
alcpu_cache_get_size(alc_cache_level_t level, alc_cache_type_t type)
{
    alci::Cpu c{ 0 };

    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    return cinfo.getSize(level_to_cxx(level), type_to_cxx(type)) / ALCI_KB;
}

/**
 * \brief Get cache size of the given cache level and type for specific core.
 *
 * Provides cache size in KB.
 *
 * \param[in]   core  Core number for which cache size to be fetched
 * \param[in]   level Cache level.
 * \param[in]   type  Cache type.
 * \param[out]  size  Cache size returned (in KB).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_size(alc_core_num_t    core_num,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    size_t*           size)
{
    /* FIXME: use core_num to get cache heirarchy */
    Cpu c{core_num};
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *size = cinfo.getSize(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

/**
 * \brief Get cache ways of the given cache level and type for specific core.
 *
 * \param[in]   core  Core number for which cache size to be fetched
 * \param[in]   level Cache level.
 * \param[in]   type  Cache type.
 * \param[out]  way   Cache way returned (in bytes).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_way(alc_core_num_t    core_num,
                   alc_cache_level_t level,
                   alc_cache_type_t  type,
                   size_t*           way)
{
    /* FIXME: use core_num to get cache heirarchy */
    Cpu c{core_num};
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *way = cinfo.getWay(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

/**
 * \brief Get cache lanes of the given cache level and type for specific core.
 *
 * \param[in]   core  Core number for which cache size to be fetched
 * \param[in]   level Cache level.
 * \param[in]   type  Cache type.
 * \param[out]  lane  Cache lane returned (in bytes).
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
    auto cinfo = c.getCacheInfo(level_to_cxx(level));
    *lane = cinfo.getLane(level_to_cxx(level), type_to_cxx(type));
    return ALC_E_SUCCESS;
}

EXTERN_C_END
