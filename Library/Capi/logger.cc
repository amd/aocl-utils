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

#include "Capi/au/logger.h"
#include "Au/Logger/LoggerCtx.hh"

AUD_EXTERN_C_BEGIN

Logger*
au_logger_create()
{
    Logger* logger = new Logger();
    logger->logger = std::make_unique<Au::Logger::Logger>(logger->logWriter);
    logger->logWriter.start();
    return logger;
}

void
au_logger_add_sink(Logger* logger, const char* sink_type)
{
    std::unique_ptr<Au::Logger::ISink> sink =
        Au::Logger::SinkFactory::createSink(sink_type, sink_type);
    logger->logWriter.addSink(sink);
}

void
au_logger_log(Logger* logger, const char* message, log_level_t level)
{
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
    logger->logger->log(msg);
}

void
au_logger_flush(Logger* logger)
{
    logger->logger->flush();
}

void
au_logger_destroy(Logger* logger)
{
    delete logger;
}
AUD_EXTERN_C_END
