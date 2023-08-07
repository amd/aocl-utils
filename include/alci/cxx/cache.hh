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
     * \brief   Get Cache size (Kb) based on Cache Level and Cache Type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return  Returns cache size in Kb.
     */
    uint64_t getSize(alci::CacheLevel level, alci::CacheType type);

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
    uint64_t getWay(alci::CacheLevel level, alci::CacheType type);

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
    uint64_t getLane(alci::CacheLevel level, alci::CacheType type);

    /**
     * \brief   Updates cache info for current core based on Cache Level and
     * Cache Type.
     *
     * \param[in] level Cache level.
     * \param[in] type  Cache type.
     *
     * \return -1 on error
     *          0 on success
     */
    alc_cpuid_error_t update(alci::CacheLevel level, alci::CacheType type);

    /* TODO: add support for the following if needed */
    //  uint64_t  m_cachephyspartitions; /**< Cache physical line partitions */
    //  uint8_t   m_fullyassociative;    /**< 1 = fully associative cache */
    //  uint8_t   m_selfinitialization;  /**< 1 = Cache is self initializing;
    //                                              cache does not need software
    //                                              initialization */

  private:
    CacheLevel m_level; /**< Identifies the cache level - L1/L2/L3. */
    CacheType  m_type;  /**< Identifies the type of cache D-cache, I-cache. */
    uint64_t   m_size;  /**< Cache size in bytes. */
    uint64_t   m_set;   /**< Cache number of sets. */
    uint64_t   m_line;  /**< Cache line size in bytes. */
    uint64_t   m_way;   /**< Cache number of ways. */
    CpuIdRawData m_raw_data; /**< To hold cpuid raw data.*/
};

} // namespace alci