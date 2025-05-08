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

#include <iostream>
#include <thread>
#include <vector>

#include "Au/Logger/LogManager.hh"

#include "Au/Logger/Macros.hh"

using namespace Au::Logger;

void
log_cpp_api()
{
    std::unique_ptr<ILogger> consoleLogger =
        LoggerFactory::createLogger("ConsoleLogger", "Main Console");

    // Set the logger to the singleton LogWriter
    LogWriter::setLogger(std::move(consoleLogger));

    // Get the singleton instance of LogWriter
    auto logWriter = LogWriter::getLogWriter();

    LogManager logger(logWriter);

    for (int i = 0; i < 10; ++i) {
        Message msg("This is a message " + std::to_string(i));
        logger.log(msg);
    }

    logger.flush();

    logger << Message("This is a message using operator '<<'");
    logger << "This is a string message using operator '<<'";

    logger.flush();
    logWriter->stop(); // Add this line to stop the logger thread
}

void
log_macro()
{
    AU_LOGGER_LOG_INFO("This is an info message using macro");
    AU_LOGGER_LOG_WARN("This is a warning message using macro");
    AU_LOGGER_LOG_ERROR("This is an error message using macro");
    AU_LOGGER_LOG_FATAL("This is a fatal message using macro");
    AU_LOGGER_LOG_PANIC("This is a panic message using macro");
    AU_LOGGER_LOG_TRACE("This is a trace message using macro");
    AU_LOGGER_LOG_DEBUG("This is a debug message using macro");
    AU_LOGGER_LOG_NOTICE("This is a notice message using macro");
}

int
main(int argc, char const* argv[])
{
    log_cpp_api();
    log_macro();
    return 0;
}
