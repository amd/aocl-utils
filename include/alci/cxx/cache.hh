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

#pragma once

#include "alci/cxx/alci.hh"
#include "alci/cxx/cpuidrawdata.hh"

#include <cstdint>

namespace alci {

// clang-format off

/**
 * \class   CacheInfo
 * \brief   Class to get cache info.
 *
 * Details include core size, type, level, line size, number of ways.
 */
class CacheInfo
{
  public:
    /**
     * \struct  CacheInfoData
     * \brief   Members to hold cache info data for CPP.
     */
    struct CacheInfoData
    {
        CacheLevel level;     /**< Identifies the cache level - L1/L2/L3. */
        CacheType  type;      /**< Identifies the type of cache D-cache, I-cache. */
        uint64_t   size;      /**< Cache size in KB. */
        uint64_t   numsets;   /**< Cache number of sets. */
        uint64_t   numways;   /**< Cache number of ways. */
        uint64_t   linesize;  /**< Cache line size in bytes. */
        uint32_t   physpartitions;    /**< Cache physical line partitions. */
        uint32_t   numsharingcache;   /**< Number of logical processors sharing cache. */
        bool       selfinit;          /**< 1 = Cache is self initializing;
                                            cache does not need software
                                            initialization. */
        bool       inclusive; /**< 0=Cache is not inclusive (exclusive) of lower cache levels.
                                   1=Cache is inclusive of lower cache levels. */
    };
    
    /**
     * \brief   Updates cache info for current core based on Cache Level and
     *          Cache Type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return -1 on error
     *          0 on success
     */
    alc_cpuid_error_t update(alci::CacheLevel level, alci::CacheType type);
    
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
    alc_cpuid_error_t getInfo(alci::CacheLevel    level,
                          alci::CacheType         type,
                          alci::CacheParam        param,
                          uint64_t*               value);
    
    /**
     * \brief   Get Cache size (Kb) based on Cache Level and Cache Type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns cache size in Kb.
     */
    uint64_t getSize(alci::CacheLevel level, alci::CacheType type);
    
    /**
     * \brief   Get Cache number of sets based on Cache Level and Cache Type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns cache number of sets.
     */
    uint64_t getNumSets(alci::CacheLevel level, alci::CacheType type) const;

    /**
     * \brief   Get number of ways to access Cache based on Cache Level and
     * Cache Type.
     *
     * Ex: 4-way cache will have 4 cache blocks.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns number of ways to access Cache.
     */
    uint64_t getWay(alci::CacheLevel level, alci::CacheType type) const;

    /**
     * \brief   Get cache line size (in bytes) to access Cache based on Cache
     * Level and Cache Type.
     *
     * Ex: 4-way cache will have 4 cache blocks. Each block size is called as
     * cache line size.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns cache line size.
     */
    uint64_t getLane(alci::CacheLevel level, alci::CacheType type) const;
    
    /**
     * \brief Get Cache physical line partitions of the given cache level and type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns cache physical line partitions.
     */
    uint32_t getPartitions(alci::CacheLevel level, alci::CacheType type) const;
    
    /**
     * \brief Get Number of logical processors sharing cache for given
     *        cache level and type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns number of logical processors sharing cache.
     */
    uint32_t getNumSharingCache(alci::CacheLevel level, alci::CacheType type) const;
    
    /**
     * \brief Get fully associative cache details for given cache level and type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns 1 if cache is fully associative, else returns 0.
     */
    bool isFullAssoc(alci::CacheLevel level, alci::CacheType type) const;
    
    /**
     * \brief Get self initializing cache details for given cache level and type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns 1 if cache is self initializing, else returns 0.
     */
    bool isSelfInit(alci::CacheLevel level, alci::CacheType type) const;
    
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
    bool isInclusive(alci::CacheLevel level, alci::CacheType type) const;


  private:
    CacheInfoData m_cache_data; /**< To hold cache info data. */
    CpuIdRawData  m_raw_data;   /**< To hold cpuid raw data.*/
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
    alc_cpuid_error_t getCacheId(alci::CacheLevel  level,
                                alci::CacheType     type,
                                uint32_t            &cacheid) const;
};
// clang-format on

} // namespace alci