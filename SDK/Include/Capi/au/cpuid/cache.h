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

#ifndef __AU_CAPI_CACHE_H__
#define __AU_CAPI_CACHE_H__

#include "Capi/au/au.h"
#include "Capi/au/macros.h"

AUD_EXTERN_C_BEGIN

typedef enum
{
    AU_CPU_CACHE_LEVEL_UNKNOWN,
    AU_CPU_CACHE_LEVEL_L1,
    AU_CPU_CACHE_LEVEL_L2,
    AU_CPU_CACHE_LEVEL_L3,
    AU_CPU_CACHE_LEVEL_L4,
    AU_CPU_CACHE_LEVEL_MAX,
} au_cpu_cache_level_t;

typedef enum
{
    AU_CPU_CACHE_TYPE_UNKNOWN,
    AU_CPU_CACHE_TYPE_INST,
    AU_CPU_CACHE_TYPE_DATA,
    AU_CPU_CACHE_TYPE_UNIFIED,
    AU_CPU_CACHE_TYPE_MAX,
} au_cpu_cache_type_t;

/**
 * @brief
 *
 * @param
 *
 * @return
 **/
au_error_t
au_cpu_cache_get_size();

/**
 * @brief
 *
 * @param
 *
 * @return
 **/
au_error_t
au_cpu_cache_get_way();

/**
 * @brief
 *
 * @param
 *
 * @return
 **/
au_error_t
au_cpu_cache_get_lane();

AUD_EXTERN_C_END

#endif
