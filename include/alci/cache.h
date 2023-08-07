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
#ifndef ALCI_CACHE_H
#define ALCI_CACHE_H

#include "alci/alci.h"

#include <stddef.h>
#include <stdint.h>

EXTERN_C_BEGIN

// clang-format off
/**
 * \struct alc_cache_info
 * \brief Members to hold cache info.
 */
struct alc_cache_info
{
    uint64_t          cachesize;     /**< Cache size in bytes.*/
    uint64_t          cachenumsets;  /**< cache number of sets.*/
    uint64_t          cachenumways;  /**< Cache number of ways.*/
    uint64_t          cachelinesize; /**< cache line size in bytes.*/
    alc_cache_level_t cachelevel;    /**< identifies the cache level.*/
    alc_cache_type_t  cachetype;     /**< Identifies the type of cache:
                                  00h Null; no more caches.
                                  01h Data cache.
                                  02h Instruction cache.
                                  03h Unified cache.
                                  1Fh-04h Reserved.*/

    // uint8_t        fullyassociative;    /**< 1 = Fully associative cache.*/
    // uint8_t        selfinitialization;  /**< 1 = Cache is self initializing;
    //                                         cache does not need software
    //                                         initialization.*/
    // uint16_t       cachephyspartitions; /**< Cache physical line partitions.*/
    // uint16_t       numsharingcache;     /**< Number of logical processors sharing
    // cache.*/
};

/** Pointer to hold cache info.*/
typedef struct alc_cache_info* alc_cache_infop;

/**
 * \brief       Provides pointer to cache info of the given cache level & type.
 *
 * \param[out]  cacheinfo Pointer to the cache information in alc_cache_info
 *                        struct
 * \param[in]   level     Cache level.
 * \param[in]   type      Cache type.
 *
 * \retval      ALC_E_SUCCESS      If valid cache information is found.
 * \retval      ALC_E_FAILURE      If valid cache information is not found.
 */
alc_cpuid_error_t
alcpu_cache_get_info(alc_cache_infop   cacheinfo,
                     alc_cache_level_t level,
                     alc_cache_type_t  type);

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
// clang-format on
uint64_t
alcpu_cache_get_size(alc_cache_level_t level, alc_cache_type_t type);

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
alci_cache_get_size(alc_core_num_t    core,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    size_t*           size);

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
alci_cache_get_way(alc_core_num_t    core,
                   alc_cache_level_t level,
                   alc_cache_type_t  type,
                   size_t*           way);

/**
 * \brief Get cache lanes of the given cache level and type for specific core.
 *
 * Provides cache size in KB.
 *
 * \param[in]   core  Core number for which cache size to be fetched
 * \param[in]   level Cache level.
 * \param[in]   type  Cache type.
 * \param[out]  lane  Cache lane returned (in bytes).
 *
 * \return      Returns success or failure via alc_cpuid_error_t
 */
alc_cpuid_error_t
alci_cache_get_lane(alc_core_num_t    core,
                    alc_cache_level_t level,
                    alc_cache_type_t  type,
                    size_t*           lane);

/**
 * \brief   Get last level cache.
 *
 * \param[in] core  CPU core number.
 *
 * \return  Returns last level cache.
 */
uint32_t
alci_cpu_get_last_cache_level(alc_core_num_t core);

EXTERN_C_END

#endif /* ALCI_CACHE_H */
