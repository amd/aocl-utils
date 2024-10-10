/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

#include "Au/Defs.hh"

#include "Au/Au.hh"
#include "Au/Types.hh"

#include <cstdint>
#include <vector>

namespace Au {

namespace cache_attr {
    /**
     *  \enum WritePolicy
     *  \brief Cache policy.
     */
    enum class EWritePolicy : Uint32
    {
        WriteBack = 1, /**< Write back. */
        WriteThrough   /**< Write through. */
    };

    /**
     * @enum AllocatePolicy
     * @brief Cache allocation type.
     */
    enum class EAllocatePolicy : Uint32
    {
        ReadAllocate = 1, /**< Read allocate. */
        WriteAllocate     /**< Write allocate. */
    };

    /**
     * @enum alc_cache_level_t
     * @brief Cache levels.
     */
    enum class ELevel
    {
        L1 = 1, /**< L1 cache.*/
        L2,     /**< L2 cache.*/
        L3,     /**< L3 cache.*/
        L4,     /**< L4 cache.*/
        L5,     /**< L5 cache.*/
        Unknown /**< Unknown cache.*/
    };

    /**
     * @enum alc_cache_type_t
     * @brief Cache types.
     */
    enum class EType
    {
        ICache = 1, /**< Instruction Cache. */
        DCache,     /**< Data Cache. */
        Unified,    /**< Unified Cache. */
        Unknown     /**< Unable to determine. */
    };
} // namespace cache_attr

std::ostream&
operator<<(std::ostream& os, cache_attr::ELevel const& lvl);

std::ostream&
operator<<(std::ostream& os, cache_attr::EWritePolicy const& lvl);

std::ostream&
operator<<(std::ostream& os, cache_attr::EAllocatePolicy const& lvl);

std::ostream&
operator<<(std::ostream& os, cache_attr::EType const& lvl);

/**
 * \class   CacheInfo
 * \brief   Class to get cache info.
 *
 * Details include core size, type, level, line size, number of ways.
 */
class CacheInfo
{
  public:
    using CacheLevel = cache_attr::ELevel;
    using CacheType  = cache_attr::EType;

    /**
     * @brief Constructor
     * @param[in] level Cache level.
     * @param[in] type  Cache type.
     *
     */
    CacheInfo(CacheLevel level, CacheType type);

    /**
     * @brief   Get Cache size (Kb) based on Cache Level and Cache Type.
     *
     * @param[in] level Cache level.
     * @param[in] type  Cache type.
     *
     * @return  Returns cache size in Kb.
     */
    size_t getSize() const { return m_size; }

    /**
     * @brief   Get number of ways to access Cache based on Cache Level and
     * Cache Type.
     *
     * Ex: 4-way cache will have 4 cache blocks.
     *
     * @return  Returns number of ways to access Cache.
     */
    Uint64 getWay() const { return m_way; }

    /**
     * @brief   Get cache line size (in bytes) to access Cache based on
     * Cache Level and Cache Type.
     *
     * Ex: 4-way cache will have 4 cache blocks. Each block size is called
     * as cache line size.
     *
     * @return  Returns cache line size.
     */
    Uint64 getLane() const { return m_lane; }

    /**
     * @brief
     * FIXME: Documentation
     */
    CacheLevel getLevel() const { return m_level; }

    /**
     * @brief
     * FIXME: Documentation
     */
    CacheType getType() const { return m_type; }

    /**
     * @brief
     * FIXME: Documentation
     */
    Uint64 getSets() const { return m_set; }

    friend bool operator==(CacheInfo const& a, CacheInfo const& b)
    {
        return a.m_level == b.m_level && a.m_type == b.m_type;
    }

    /**
     * @brief  Set Cache level based on the information from cpuid instruction.
     * @param[in] lvl Cache level.
     *
     * @return void
     */
    void setLevel(CacheLevel lvl);

    /**
     * @brief Set Cache type based on the information from cpuid instruction.
     * @param[in] ct Cache type.
     *
     * return void
     */
    void setType(CacheType ct);

    /**
     * @brief Set Cache size based on the number of ways partitions lanes and
     * sets.
     * @param[in] size Cache size.
     *
     * return void
     */
    void setSize(Uint64 size);

    /**
     * @brief Set Cache way based on the information from the cpuid instruction.
     * @param[in] way_size Cache way size.
     *
     * return void
     */
    void setWay(Uint64 way_size);

    /**
     * @brief Set Cache lane based on the information from the cpuid
     * instruction.
     * @param[in] lane_size Cache lane size.
     *
     * return void
     */
    void setLane(Uint64 lane_size);

    /**
     * @brief Set Cache sets based on the information from the cpuid
     * instruction.
     * @param[in] sets Cache sets.
     *
     * return void
     */
    void setSets(Uint64 sets);

  private:
    CacheLevel m_level;    /**< Identifies the cache level - L1/L2/L3. */
    CacheType  m_type;     /**< Identifies as D-cache, I-cache, etc.. */
    Uint64     m_size = 0; /**< Cache size in bytes. */
    Uint64     m_set  = 0; /**< Cache number of sets. */
    Uint64     m_lane = 0; /**< Cache line size in bytes. */
    Uint64     m_way  = 0; /**< Cache number of ways. */

    /* TODO: add support for the following if needed */
    //  uint64_t  m_partitions; /**< Cache physical line partitions
    //  uint8_t   m_associativity;    /**< 1 = fully associative cache
    //  uint8_t   m_self_initialization;  /**< 1 = Cache is self initializing;
    //                                            cache does not need
    //                                            software initialization
    //
};

class AUD_API_EXPORT CacheView final
{
  public:
    using member = std::vector<CacheInfo>;

    CacheView()
        : m_cache_info_list{}
    {
    }

    size_t getNumLevels() const { return m_cache_info_list.size(); }

    const member::iterator begin() { return m_cache_info_list.begin(); }
    const member::iterator end() { return m_cache_info_list.end(); }

  private:
    std::vector<CacheInfo> m_cache_info_list;
};

} // namespace Au
