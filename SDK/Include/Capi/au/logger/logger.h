/*
 * Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
