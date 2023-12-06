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

#ifndef __AU_ERROR_H__
#define __AU_ERROR_H__

#include "Capi/au/macros.h"
#include "Capi/au/types.h"

AUD_EXTERN_C_BEGIN

#define AUD_BAD_PTR_ERR_RET(ptr, err)                                          \
    do {                                                                       \
        if (nullptr == ptr) {                                                  \
            return (au_error_t)eError_BadPointer;                              \
        }                                                                      \
    } while (0)

#define AUD_ZERO_LEN_ERR_RET(len, err)                                         \
    do {                                                                       \
        if (0 == len) {                                                        \
            return (au_error_t)eError_InvalidArgument;                         \
        }                                                                      \
    } while (0)

typedef enum __au_error_code
{
    eError_Ok   = 0,
    eError_None = eError_Ok,

    eError_Generic,

    eError_NotSupported,

    eError_InvalidArgument,

    eError_BadPointer,

    eError_NoMemory,

    eError_InvalidSize,
} au_error_code_t;

#if !defined(au_error_t)
typedef Uint64 au_error_t;
#endif

#ifndef __cplusplus
#ifndef bool
typedef enum __au_bool
{
    false = 0,
    true
} bool;
#endif
#endif

/**
 * @brief  Checks if an value returned by an API is an error
 *
 * @param[in]  err     an au_error_t
 *
 * @returns true if an error has occured, false otherwise
 */
bool
au_is_error(const au_error_t err);

AUD_EXTERN_C_END

#endif /* __AU_ERROR_H__ */
