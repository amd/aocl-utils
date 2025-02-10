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

#include "Capi/au/logger/logger.h"
#include "Au/Logger/LogManager.hh"
#include "Au/Logger/LoggerCtx.hh"

AUD_EXTERN_C_BEGIN

logger_ctx_t*
au_logger_create()
{
    LoggerCtx* loggerCtx = new LoggerCtx();
    // Check if memory allocation is successful
    if (loggerCtx == nullptr) {
        return nullptr;
    }

    loggerCtx->logger = std::make_unique<Au::Logger::LogManager>(
        Au::Logger::LogWriter::getLogWriter());
    // Check if memory allocation is successful
    if (loggerCtx->logger == nullptr) {
        delete loggerCtx;
        return nullptr;
    }

    return loggerCtx;
}

void
au_logger_log(logger_ctx_t* logger, const char* message, log_level_t level)
{
    LoggerCtx* loggerCtx = reinterpret_cast<LoggerCtx*>(logger);
    Au::Logger::Priority::PriorityLevel priority;
    switch (level) {
        case AUD_LOG_LEVEL_TRACE:
            priority = Au::Logger::Priority::PriorityLevel::eTrace;
            break;
        case AUD_LOG_LEVEL_DEBUG:
            priority = Au::Logger::Priority::PriorityLevel::eDebug;
            break;
        case AUD_LOG_LEVEL_INFO:
            priority = Au::Logger::Priority::PriorityLevel::eInfo;
            break;
        case AUD_LOG_LEVEL_WARN:
            priority = Au::Logger::Priority::PriorityLevel::eWarning;
            break;
        case AUD_LOG_LEVEL_ERROR:
            priority = Au::Logger::Priority::PriorityLevel::eError;
            break;
        case AUD_LOG_LEVEL_FATAL:
            priority = Au::Logger::Priority::PriorityLevel::eFatal;
            break;
        default:
            priority = Au::Logger::Priority::PriorityLevel::eInfo;
            break;
    }
    Au::Logger::Priority p(priority);
    Au::Logger::Message  msg(message, p);
    loggerCtx->logger->log(msg);
}

void
au_logger_flush(logger_ctx_t* logger)
{
    LoggerCtx* loggerCtx = reinterpret_cast<LoggerCtx*>(logger);
    loggerCtx->logger->flush();
}

void
au_logger_destroy(logger_ctx_t* logger)
{
    LoggerCtx* loggerCtx = reinterpret_cast<LoggerCtx*>(logger);
    Au::Logger::LogWriter::getLogWriter()->stop();
    delete loggerCtx;
}

AUD_EXTERN_C_END
