/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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
 *    without specific prior written permission.
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

#ifndef __AU_CAPI_ENVIRON_H__
#define __AU_CAPI_ENVIRON_H__

#include "Au/Defs.hh"
#include "Capi/au/au.h"
#include "Capi/au/error.h"
#include "Capi/au/macros.h"
#include "Au/Defs.hh"

AUD_EXTERN_C_BEGIN;

/**
 * @brief Initialize environment from applications's "const char * envp[]"
 *
 * @param[in]  Pointer to array of strings (null-terminated)
 *
 * @return None
 */
AUD_API_EXPORT void
au_env_init(const char** envp);

/**
 * @brief  Get an environment variable
 *
 * @detail Searches in system env, and user env
 *         When set in both environments, User env overrides system env.
 *
 * @param[in]  name Name of the environment varible to search
 *
 * @return NULL if the environment is not set
 *         pointer to char array (null-terminated)string if set.
 */
AUD_API_EXPORT const char*
au_env_get(const char* name);

/**
 * @brief  Set an environment variable
 *
 * @detail Sets the User Env variable
 *
 * @param[in]  name Name of the environment varible to set
 * @param[in]  val  Value of the variable as string
 *
 * @return NULL if the environment is not set
 *         pointer to char array (null-terminated)string if set.
 */
AUD_API_EXPORT const char*
au_env_set(const char* name, const char* val);

/**
 * @brief Unset a variable, delete from list
 *
 * @details  if the 'name' exists it will be deleted from user env.
 * @param[in] name      name of the variable to be unset
 *
 * @return void
 */
AUD_API_EXPORT void
au_env_unset(const char* name);

/**
 * @brief    Check if a given environment variable is enabled.
 *
 * @details Variable can be set with case insensitive values from command line
 *
 *              AU_BLIS_DEBUG=YES
 *              AU_BLIS_DEBUG=ON
 *              AU_BLIS_DEBUG=1
 *              AU_BLIS_DEBUG=TRUE
 *           all above will return 'true' or 1 when called like below
 * @example
 *    if (au_env_is_enabled("AU_BLIS_DEBUG")) {
 *    }
 *
 * However it will return 'false' for following cases (string values are case-
 * insensitive)
 *               AU_BLIS_DEBUG=0
 *               AU_BLIS_DEBUG=OFF
 *               AU_BLIS_DEBUG=FALSE
 *               AU_BLIS_DEBUG=NO
 */
bool AUD_API_EXPORT
au_env_is_enabled(const char*);

/**
 * @brief    Check if a given environment variable is set.
 *
 * @details  Variable can be set from command line as
 *              AU_BLIS_DEBUG=YES/NO
 *              AU_BLIS_DEBUG=ON/OFF
 *              AU_BLIS_DEBUG=1/0
 *              AU_BLIS_DEBUG=TRUE/FALSE
 *           all above will return 'true' or 1 when called like below
 *
 * @example
 *    if (au_env_is_set("AU_BLIS_DEBUG")) {
 *    }
 *
 * However it will return 'false' only if the variable is empty or not set.
 */
AUD_API_EXPORT bool
au_env_is_set(const char*);

AUD_EXTERN_C_END;

#endif /* AU_CAPI_ENVIRON_H */
