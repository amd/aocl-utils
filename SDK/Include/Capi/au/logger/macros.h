/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#ifndef __AU_LOGGER_MACROS_H__
#define __AU_LOGGER_MACROS_H__

#include "Capi/au/logger/logger.h"

// Helper macro for logging
#define AUD_LOG(level, msg)                                                    \
    {                                                                          \
        logger_ctx_t* logger = au_logger_create();                             \
        au_logger_log(logger, msg, level);                                     \
        au_logger_flush(logger);                                               \
        au_logger_destroy(logger);                                             \
    }

// Macros for logging
#define AUD_LOG_ERROR(msg) AUD_LOG(AUD_LOG_LEVEL_ERROR, msg)
#define AUD_LOG_INFO(msg)  AUD_LOG(AUD_LOG_LEVEL_INFO, msg)
#define AUD_LOG_WARN(msg)  AUD_LOG(AUD_LOG_LEVEL_WARN, msg)
#define AUD_LOG_DEBUG(msg) AUD_LOG(AUD_LOG_LEVEL_DEBUG, msg)
#define AUD_LOG_TRACE(msg) AUD_LOG(AUD_LOG_LEVEL_TRACE, msg)
#define AUD_LOG_FATAL(msg) AUD_LOG(AUD_LOG_LEVEL_FATAL, msg)

#endif /* __AU_LOGGER_MACROS_H__ */
