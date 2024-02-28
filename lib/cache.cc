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

#include "alci/cxx/cache.hh"

namespace alci {

#include "alci/enum.h"

constexpr int ALCI_KB = 1024;

/*
   CACHE APIS
*/

/**
 * \brief   Get cache id for current core based on Cache Level and Cache
 *          Type.
 *
 * \param[in]  level    Cache level.
 * \param[in]  type     Cache type.
 * \param[out] cacheid  Cache id.
 *
 * \retval  ALC_E_SUCCESS      Getting cacheid is successful.
 * \retval  ALC_E_FAILURE      Getting cacheid is failed.
 */
alc_cpuid_error_t
CacheInfo::getCacheId(alci::CacheLevel  level,
                    alci::CacheType     type,
                    uint32_t            &cacheid) const
{
    /* Check topology extensions bit. Should not get cache details
       if this bit is not set and return. */ 
    if (!(m_raw_data.m_cpuid[ALC_CPUID_EAX_8_01][ecx] & \
        ALC_CPUID_BIT_TOPOLOGY_EXTENSIONS)) {
        return ALC_E_FAILURE;
    }
    
    switch (level) {
        case CacheLevel::e_Level_1:
            if (type == CacheType::eData) {
                cacheid = ALC_CPUID_EAX_8_1D_ECX_00;
            } else if (type == CacheType::eInstruction) {
                cacheid = ALC_CPUID_EAX_8_1D_ECX_01;
            } else {
                return ALC_E_FAILURE;
            }
            break;

        case CacheLevel::e_Level_2:
            if (type == CacheType::eUnified) {
                cacheid = ALC_CPUID_EAX_8_1D_ECX_02;
            } else {
                return ALC_E_FAILURE;
            }
            break;

        case CacheLevel::e_Level_3:
            if (type == CacheType::eUnified) {
                cacheid = ALC_CPUID_EAX_8_1D_ECX_03;
            } else {
                return ALC_E_FAILURE;
            }

            break;
        case CacheLevel::e_Level_4:
        case CacheLevel::e_Level_5:
        case CacheLevel::e_Level_Unknown:
        default:
            return ALC_E_FAILURE;
    }
    return ALC_E_SUCCESS;
}

// clang-format off
/**
 * \brief   Updates cache info for current core based on Cache Level and Cache
 *          Type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \retval  ALC_E_SUCCESS      Update of cache information is successful.
 * \retval  ALC_E_FAILURE      Update of cache information is failed.
 */
alc_cpuid_error_t 
CacheInfo::update(alci::CacheLevel  level,
                  alci::CacheType   type)
{
    uint32_t cacheid = 0;
    alc_cpuid_error_t   status;

    status = getCacheId(level, type, cacheid);
    if (status == ALC_E_SUCCESS)
    {
        m_cache_data.level    = static_cast<alci::CacheLevel>(
                                extract32(m_raw_data.m_cpuid[cacheid][eax], 5, 3));
        m_cache_data.type     = static_cast<alci::CacheType>(
                                extract32(m_raw_data.m_cpuid[cacheid][eax], 0, 5));
        m_cache_data.numsets  = (m_raw_data.m_cpuid[cacheid][ecx]) + 1;
        m_cache_data.numways  = (extract32(m_raw_data.m_cpuid[cacheid][ebx], 22, 10) + 1);
        m_cache_data.physpartitions = (extract32(m_raw_data.m_cpuid[cacheid][ebx], 12, 10) + 1);
        m_cache_data.linesize = (extract32(m_raw_data.m_cpuid[cacheid][ebx], 0, 12) + 1);
        // Stores cache size in KB.
        m_cache_data.size     = ((m_cache_data.numways * m_cache_data.physpartitions * 
                                m_cache_data.linesize * m_cache_data.numsets)/ALCI_KB);
        m_cache_data.numsharingcache  = (extract32(m_raw_data.m_cpuid[cacheid][eax], 14, 12) + 1);
        m_cache_data.selfinit   = (m_raw_data.m_cpuid[cacheid][eax] & ALC_CPUID_BIT_SELF_INITIALIZING);
        m_cache_data.inclusive = (m_raw_data.m_cpuid[cacheid][edx] & ALC_CPUID_BIT_CACHE_INCLUSIVE);
    }
    return status;
}
// clang-format on

/**
 * \brief       Provides pointer to cache info of the given cache level & type.
 *
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 * \param[in]   param     Cache parameter to get value.
 * \param[out]  value     Pointer to the cache param value.
 *
 * \retval      ALC_E_SUCCESS   If valid cache value is found.
 * \retval      ALC_E_FAILURE   If valid cache value is not found.
 */
alc_cpuid_error_t
CacheInfo::getInfo(alci::CacheLevel   level,
              alci::CacheType         type,
              alci::CacheParam        param,
              uint64_t*               value)
{
    alc_cpuid_error_t status = update(level, type);
    
    if (status == ALC_E_SUCCESS) {
        switch (param) {
            case CacheParam::eSize:
                *value = m_cache_data.size;
                break;
            case CacheParam::eNumSets:
                *value = m_cache_data.numsets;
                break;
            case CacheParam::eNumWays:
                *value = m_cache_data.numways;
                break;
            case CacheParam::eLineSize:
                *value = m_cache_data.linesize;
                break;
            case CacheParam::ePhysPartitions:
                *value = m_cache_data.physpartitions;
                break;
            case CacheParam::eNumSharingCache:
                *value = m_cache_data.numsharingcache;
                break;
            case CacheParam::eFullyAssoc:
                // Cache is fully associative if numsets=1.
                *value = (m_cache_data.numsets == 1);
                break;
            case CacheParam::eSelfInit:
                *value = m_cache_data.selfinit;
                break;
            case CacheParam::eInclusive:
                *value = m_cache_data.inclusive;
                break;
            default:
                *value = 0;
        }
    }
    return status;
}

/**
 * \brief   Get Cache size (KB) based on Cache Level and Cache Type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns cache size in KB.
 */
uint64_t
CacheInfo::getSize(alci::CacheLevel level, alci::CacheType type)
{
    if (update(level, type)) {
        return m_cache_data.size;
    } else {
        return 0;
    }
}

// clang-format off
/**
 * \brief   Get Cache number of sets based on Cache Level and Cache Type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns cache number of sets.
 */
uint64_t
CacheInfo::getNumSets(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return ((m_raw_data.m_cpuid[cacheid][ecx]) + 1);
    } else {
        return 0;
    }
}

/**
 * \brief   Get number of ways to access Cache based on Cache Level and Cache
 * Type.
 *
 * Ex: 4-way cache will have 4 cache blocks.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns number of ways to access Cache (in bytes).
 */
uint64_t
CacheInfo::getWay(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return (extract32(m_raw_data.m_cpuid[cacheid][ebx], 22, 10) + 1);
    } else {
        return 0;
    }
}

/**
 * \brief   Get cache line size (in bytes) to access Cache based on Cache Level
 * and Cache Type.
 *
 * Ex: 4-way cache will have 4 cache blocks. Each block size is called as cache
 * line size.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns cache line size (in bytes).
 */
uint64_t
CacheInfo::getLane(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return (extract32(m_raw_data.m_cpuid[cacheid][ebx], 0, 12) + 1);
    } else {
        return 0;
    }
}

/**
 * \brief Get Cache physical line partitions of the given cache level and type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns cache physical line partitions.
 */
uint32_t
CacheInfo::getPartitions(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return (extract32(m_raw_data.m_cpuid[cacheid][ebx], 12, 10) + 1);
    } else {
        return 0;
    }
}

/**
 * \brief Get Number of logical processors sharing cache for given
 *        cache level and type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns number of logical processors sharing cache.
 */
uint32_t
CacheInfo::getNumSharingCache(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return (extract32(m_raw_data.m_cpuid[cacheid][eax], 14, 12) + 1);
    } else {
        return 0;
    }
}

/**
 * \brief Get fully associative cache details for given cache level and type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns 1 if cache is fully associative, else returns 0.
 */
bool
CacheInfo::isFullAssoc(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return ((m_raw_data.m_cpuid[cacheid][eax] & ALC_CPUID_BIT_FULLYASSOCIATIVE) == ALC_CPUID_BIT_FULLYASSOCIATIVE);
    } else {
        return false;
    }
}

/**
 * \brief Get self initializing cache details for given cache level and type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns 1 if cache is self initializing, else returns 0.
 */
bool
CacheInfo::isSelfInit(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;

    if (getCacheId(level, type, cacheid)) {
        return ((m_raw_data.m_cpuid[cacheid][eax] & ALC_CPUID_BIT_SELF_INITIALIZING) == ALC_CPUID_BIT_SELF_INITIALIZING);
    } else {
        return false;
    }
}

/**
 * \brief Get cache inclusive or exclusive for given cache level and type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns 1 if cache is inclusive of lower cache levels,
 *                    else returns 0 if cache is not inclusive (exclusive)
 *                    of lower cache levels.
 */
bool
CacheInfo::isInclusive(alci::CacheLevel level, alci::CacheType type) const
{
    uint32_t cacheid = 0;
    
    if (getCacheId(level, type, cacheid)) {
        return ((m_raw_data.m_cpuid[cacheid][edx] & ALC_CPUID_BIT_CACHE_INCLUSIVE) == ALC_CPUID_BIT_CACHE_INCLUSIVE);
    } else {
        return 0;
    }
}

// clang-format on

} // namespace alci
