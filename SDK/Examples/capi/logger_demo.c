/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

#include "Capi/au/logger/logger.h"
#include "Capi/au/logger/macros.h"

void
log_capi()
{
    logger_ctx_t* logger = au_logger_create();
    au_logger_log(logger, "Hello From Logger", AUD_LOG_LEVEL_INFO);
    au_logger_log(logger, "This is trace message", AUD_LOG_LEVEL_TRACE);
    au_logger_log(logger, "This is debug message", AUD_LOG_LEVEL_DEBUG);
    au_logger_log(logger, "This is info message", AUD_LOG_LEVEL_INFO);
    au_logger_log(logger, "This is warn message", AUD_LOG_LEVEL_WARN);
    au_logger_log(logger, "This is error message", AUD_LOG_LEVEL_ERROR);
    au_logger_log(logger, "This is fatal message", AUD_LOG_LEVEL_FATAL);
    au_logger_flush(logger);
    au_logger_destroy(logger);
}

void
log_macro()
{
    AUD_LOG_INFO("This is info message from macro");
    AUD_LOG_ERROR("This is error message from macro");
    AUD_LOG_WARN("This is warn message from macro");
    AUD_LOG_DEBUG("This is debug message from macro");
    AUD_LOG_TRACE("This is trace message from macro");
}

int
main(int argc, char const* argv[])
{
    log_capi();
    log_macro();
    return 0;
}
