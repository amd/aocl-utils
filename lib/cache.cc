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

/*
   CACHE APIS
*/

/**
 * \brief   Get Cache size (Kb) based on Cache Level and Cache Type.
 *
 * \param[in] level Cache level.
 * \param[in] type  Cache type.
 *
 * \return  Returns cache size in Kb.
 */
uint64_t
CacheInfo::getSize(alci::CacheLevel level, alci::CacheType type)
{
    if (update(level, type))
        return m_size;
    else
        return 0;
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
CacheInfo::getWay(alci::CacheLevel level, alci::CacheType type)
{
    if (update(level, type))
        return m_way;
    else
        return 0;
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
CacheInfo::getLane(alci::CacheLevel level, alci::CacheType type)
{
    if (update(level, type))
        return m_line;
    else
        return 0;
}

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
CacheInfo::update(alci::CacheLevel          level,
                  alci::CacheType           type)
{
    uint32_t cacheid             = 0;
    uint32_t cachephyspartitions = 0;

    switch (level) {
        case CacheLevel::e_Level_1:
            if (type == CacheType::eData)
                cacheid = ALC_CPUID_EAX_8_1D_ECX_00;

            else if (type == CacheType::eInstruction)
                cacheid = ALC_CPUID_EAX_8_1D_ECX_01;

            else
                return ALC_E_FAILURE;
            break;

        case CacheLevel::e_Level_2:
            if (type == CacheType::eUnified)
                cacheid = ALC_CPUID_EAX_8_1D_ECX_02;
            else
                return ALC_E_FAILURE;
            break;

        case CacheLevel::e_Level_3:
            if (type == CacheType::eUnified)
                cacheid = ALC_CPUID_EAX_8_1D_ECX_03;
            else
                return ALC_E_FAILURE;
            break;
        case CacheLevel::e_Level_4:
        case CacheLevel::e_Level_5:
        case CacheLevel::e_Level_Unknown:
        default:
            return ALC_E_FAILURE;
    }

    m_level = static_cast<alci::CacheLevel>(
        extract32(m_raw_data.m_cpuid[cacheid][0], 5, 3));
    m_type = static_cast<alci::CacheType>(
        extract32(m_raw_data.m_cpuid[cacheid][0], 0, 5));
    m_set               = (m_raw_data.m_cpuid[cacheid][2]) + 1;
    m_line              = extract32(m_raw_data.m_cpuid[cacheid][1], 0, 12) + 1;
    m_way               = extract32(m_raw_data.m_cpuid[cacheid][1], 22, 10) + 1;
    cachephyspartitions = extract32(m_raw_data.m_cpuid[cacheid][1], 12, 10) + 1;
    m_size              = m_way * cachephyspartitions * m_line * m_set;

    /* todo : check to add below support
    m_numsharingcache     = extract32(m_raw_data.m_cpuid[cacheid][0], 14,
    12) + 1; m_fullyassociative    =
    (bool)(m_raw_data.m_cpuid[cacheid][0] & ALC_CPUID_BIT_FULLYASSOCIATIVE);
    m_selfinitialization  = (bool)(m_raw_data.m_cpuid[cacheid][0] &
    ALC_CPUID_BIT_SELF_INITIALIZING);
    */
    return ALC_E_SUCCESS;
}
} // namespace alci