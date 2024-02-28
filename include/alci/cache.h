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
#include <stdbool.h>

EXTERN_C_BEGIN

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
                    uint64_t*           value);

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
alcpu_cache_get_size(alc_cache_level_t level, alc_cache_type_t type);

/**
 * \brief Get cache size of the given cache level and type for specific core.
 *
 * Provides cache size in KB.
 *
 * \param[in]   core  Core number of Cpu.
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
                    uint64_t*         size);

/**
 * \brief Get cache ways of the given cache level and type for specific core.
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
                        uint64_t*         numsets);

/**
 * \brief Get cache ways of the given cache level and type for specific core.
 *
 * \param[in]   core  Core number of Cpu.
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
 * \param[in]   core  Core number of Cpu.
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
                    uint32_t*         partitions);

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
                    uint32_t*         numlp);

/**
 * \brief Get fully associative cache details for given cache level and type
 *        for specific core.
 *
 * fullassoc flag:  If cache is full associative, then sets 1.
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
                    bool*             fullassoc);

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
                    bool*             selfinit);

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
                    bool*             inclusive);

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
