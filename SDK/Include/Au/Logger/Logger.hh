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

/**
 * @class ILogger
 * @brief Abstract interface for all logger types.
 *
 * Derived classes must implement write() and flush().
 */
class ILogger
{
  public:
    /**
     * @brief Writes a log message to the output.
     * @param msg The log message.
     */
    virtual void write(const Message& msg) = 0;

    /**
     * @brief Flush the output.
     */
    virtual void flush() = 0;

    /**
     * @brief Set the logger name.
     * @param loggerName Name of the logger.
     */
    virtual void setLoggerName(const String& loggerName) = 0;

    /**
     * @brief Get the logger name.
     * @return Logger name.
     */
    virtual String getLoggerName() const = 0;

    /**
     * @brief Get the logger type.
     * @return Logger type.
     */
    virtual String getLoggerType() const = 0;

    virtual ~ILogger() = default;
};

/**
 * @class GenericLogger
 * @brief Base logger class providing a default implementation of ILogger.
 */
class GenericLogger : public ILogger
{
  protected:
    String m_loggerName{}; ///< Logger name

  public:
    virtual void   write(const Message& msg) override;
    virtual void   setLoggerName(const String& loggerName) override;
    virtual String getLoggerName() const override;
    virtual String getLoggerType() const override;
    virtual void   flush() override;
    virtual ~GenericLogger() override = default;
};

/**
 * @class ConsoleLogger
 * @brief Outputs log messages to the console (stdout).
 */
class ConsoleLogger : public GenericLogger
{
  public:
    void   write(const Message& msg) override;
    String getLoggerType() const override;
    void   flush() override;
    ~ConsoleLogger() override = default;
};

/**
 * @class DummyLogger
 * @brief A no-op logger for testing or disabling logging.
 */
class DummyLogger : public GenericLogger
{
  public:
    void   write(const Message& msg) override;
    String getLoggerType() const override;
    void   flush() override;
    ~DummyLogger() override = default;
};

/**
 * @class FileLogger
 * @brief Writes log messages to a designated file.
 */
class FileLogger : public GenericLogger
{
  private:
    String m_filename; ///< Filename to write logs
    FILE*  m_file;     ///< File pointer

  public:
    /**
     * @brief Constructor for FileLogger.
     * @param filename The file to which logs should be written.
     */
    explicit FileLogger(const String& filename);

    // Disable copy constructor and assignment operator
    FileLogger(const FileLogger&)            = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    void   write(const Message& msg) override;
    void   flush() override;
    String getLoggerType() const override;

    ~FileLogger() override;
};

/**
 * @class LoggerFactory
 * @brief Provides methods to create and configure logger instances.
 *
 * Currently supported logger types for the @ref createLogger() method:
 * - "ConsoleLogger": Logs to console.
 * - "DummyLogger": Disables logging (no-op).
 * - "FileLogger": Logs to a file, specify filename as loggerName argument.
 */
class LoggerFactory
{
  public:
    /**
     * @brief Create a logger.
     * @param loggerType Type of the logger (e.g., "ConsoleLogger",
     * "DummyLogger", "FileLogger").
     * @param loggerName Logger name or filename, depending on the logger type.
     * @return Unique pointer to ILogger instance or nullptr if invalid
     * loggerType.
     */
    static std::unique_ptr<ILogger> createLogger(const String& loggerType,
                                                 const String& loggerName);

    /**
     * @brief Validate the logger type.
     * @param loggerType Type of the logger.
     */
    static void validateLoggerType(const String& loggerType);
};
} // namespace Au::Logger
