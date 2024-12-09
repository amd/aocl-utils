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
#include "Au/Logger/Message.hh"

/**
 * @brief ISink class - Writes the message to the output
 */
namespace Au::Logger {
class ILogger
{
  public:
    virtual void   write(const Message& msg)               = 0;
    virtual void   flush()                                 = 0;
    virtual void   setLoggerName(const String& loggerName) = 0;
    virtual String getLoggerName() const                   = 0;
    virtual String getLoggerType() const                   = 0;
    virtual ~ILogger()                                     = default;
};

class GenericLogger : public ILogger
{
  protected:
    String m_loggerName{};

  public:
    virtual void   write(const Message& msg) override;
    virtual void   setLoggerName(const String& loggerName) override;
    virtual String getLoggerName() const override;
    virtual String getLoggerType() const override;
    virtual void   flush() override;
    virtual ~GenericLogger() override = default;
};

/**
 * @brief ConsoleLogger class - Writes the message to the console
 */
class ConsoleLogger : public GenericLogger
{
  public:
    void   write(const Message& msg) override;
    String getLoggerType() const override;
    void   flush() override;
    ~ConsoleLogger() override = default;
};

class DummyLogger : public GenericLogger
{
  public:
    void   write(const Message& msg) override;
    String getLoggerType() const override;
    void   flush() override;
    ~DummyLogger() override = default;
};

/**
 * @brief FileSink class - Writes the message to the file
 */
class FileLogger : public GenericLogger
{
  private:
    String m_filename;
    FILE*  m_file;

  public:
    explicit FileLogger(const String& filename);

    // Disable copy constructor and assignment operator
    FileLogger(const FileLogger&)            = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    void   write(const Message& msg) override;
    void   flush() override;
    String getLoggerType() const override;

    ~FileLogger() override;
};

// TODO: Implement NetworkSink class

/**
 * @brief LoggerFactory class - Factory class to create the logger
 */
class LoggerFactory
{
  public:
    static std::unique_ptr<ILogger> createLogger(const String& loggerType,
                                                 const String& loggerName);
    static void validateLoggerType(const String& loggerType);
};
} // namespace Au::Logger
