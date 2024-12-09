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

#include "Au/Logger/Logger.hh"
#include <cassert>
#include <iostream>

namespace Au::Logger {

// Class GenericLogger begins
void
GenericLogger::write(const Message& msg)
{
    assert(false); // Not implemented
}

void
GenericLogger::setLoggerName(const String& loggerName)
{
    m_loggerName = loggerName;
}

String
GenericLogger::getLoggerName() const
{
    return m_loggerName;
}

String
GenericLogger::getLoggerType() const
{
    return "GenericLogger";
}

void
GenericLogger::flush()
{
    assert(false); // Not implemented
}

// Class GenericLogger ends

// Class ConsoleLogger begins
void
ConsoleLogger::write(const Message& msg)
{
    std::cout << msg.getMsg() << std::endl;
}

String
ConsoleLogger::getLoggerType() const
{
    return "ConsoleLogger";
}

void
ConsoleLogger::flush()
{
    std::cout.flush();
}
// Class ConsoleLogger ends

// Class DummyLogger begins
void
DummyLogger::write(const Message& msg)
{
    // Do nothing
}

String
DummyLogger::getLoggerType() const
{
    return "DummyLogger";
}

void
DummyLogger::flush()
{
    // Do nothing
}

// Class DummyLogger ends

// Class FileLogger begins
FileLogger::FileLogger(const String& filename)
    : m_filename{ filename }
    , m_file{ fopen(filename.c_str(), "a") }
{
    if (m_file == nullptr) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

void
FileLogger::write(const Message& msg)
{
    if (m_file != nullptr) {
        fprintf(m_file, "%s\n", msg.getMsg().c_str());
    }
}

void
FileLogger::flush()
{
    if (m_file != nullptr) {
        fflush(m_file);
    }
}

String
FileLogger::getLoggerType() const
{
    return "FileLogger";
}

FileLogger::~FileLogger()
{
    if (m_file != nullptr) {
        fclose(m_file);
    }
}

// Class FileLogger ends

// Class LoggerFactory begins
std::unique_ptr<ILogger>
LoggerFactory::createLogger(const String& loggerType, const String& loggerName)
{
    if (loggerType == "ConsoleLogger") {
        return std::make_unique<ConsoleLogger>();
    } else if (loggerType == "DummyLogger") {
        return std::make_unique<DummyLogger>();
    } else if (loggerType == "FileLogger") {
        return std::make_unique<FileLogger>(loggerName);
    } else {
        return nullptr;
    }
}

// FIXME: Find better implementation
void
LoggerFactory::validateLoggerType(const String& loggerType)
{
    if (loggerType != "ConsoleLogger" && loggerType != "DummyLogger"
        && loggerType != "FileLogger") {
        throw std::invalid_argument("Invalid logger type");
    }
}

// Class LoggerFactory ends
} // namespace Au::Logger
