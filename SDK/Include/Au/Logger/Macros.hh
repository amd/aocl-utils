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

#pragma once

#include "Au/Logger/LogManager.hh"

using Au::Logger::LogManager;
using Au::Logger::LogWriter;
using Au::Logger::Message;
using Au::Logger::Priority;

#define AU_LOGGER_LOG(msg, level)                                              \
    {                                                                          \
        Priority   priority(Priority::PriorityLevel::level);                   \
        Message    message(std::string(msg), priority);                        \
        LogManager logger = LogManager(LogWriter::getLogWriter());             \
        logger << message;                                                     \
        logger.flush();                                                        \
        LogWriter::getLogWriter()->stop();                                     \
    }

#define AU_LOGGER_LOG_INFO(msg) AU_LOGGER_LOG(msg, eInfo)

#define AU_LOGGER_LOG_WARN(msg) AU_LOGGER_LOG(msg, eWarning)

#define AU_LOGGER_LOG_ERROR(msg) AU_LOGGER_LOG(msg, eError)

#define AU_LOGGER_LOG_FATAL(msg) AU_LOGGER_LOG(msg, eFatal)

#define AU_LOGGER_LOG_PANIC(msg) AU_LOGGER_LOG(msg, ePanic)

#define AU_LOGGER_LOG_TRACE(msg) AU_LOGGER_LOG(msg, eTrace)

#define AU_LOGGER_LOG_DEBUG(msg) AU_LOGGER_LOG(msg, eDebug)

#define AU_LOGGER_LOG_NOTICE(msg) AU_LOGGER_LOG(msg, eNotice)
