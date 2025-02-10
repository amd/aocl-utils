/*
 * Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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

#ifndef __AU_LOGGER_LOGGER_H__
#define __AU_LOGGER_LOGGER_H__

#include "Au/Config.h"
#include "Au/Defs.hh"
#include "Capi/au/macros.h"

#include "Capi/au/logger_ctx.h"

AUD_EXTERN_C_BEGIN

/**
 * @enum log_level_t
 * @brief Defines available severity levels for C-API logging.
 *
 * The higher the level, the more severe the event.
 */
typedef enum
{
    AUD_LOG_LEVEL_TRACE,
    AUD_LOG_LEVEL_DEBUG,
    AUD_LOG_LEVEL_INFO,
    AUD_LOG_LEVEL_WARN,
    AUD_LOG_LEVEL_ERROR,
    AUD_LOG_LEVEL_FATAL
} log_level_t;

/**
 * @brief Creates a new logger context for the C-API.
 *
 * @return A pointer to the newly allocated logger context.
 */
AUD_API_EXPORT logger_ctx_t*
au_logger_create();

/**
 * @brief Logs a message at the specified log level.
 *
 * @param[in] logger  Pointer to the logger context.
 * @param[in] message Null-terminated string message.
 * @param[in] level   Desired log severity level.
 */
AUD_API_EXPORT void
au_logger_log(logger_ctx_t* logger, const char* message, log_level_t level);

/**
 * @brief Forces any buffered messages to be flushed.
 *
 * @param[in] logger  Pointer to the logger context.
 */
AUD_API_EXPORT void
au_logger_flush(logger_ctx_t* logger);

/**
 * @brief Destroys the logger context and releases its resources.
 *
 * @param[in] logger  Pointer to the logger context.
 */
AUD_API_EXPORT void
au_logger_destroy(logger_ctx_t* logger);

AUD_EXTERN_C_END

#endif // __AU_LOGGER_LOGGER_H__
