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

#include <ctime>
#include <fstream>
#include <iomanip> // for std::hex
#include <iostream>
#include <sstream>
#include <string>

#include "Au/Config.h"
#include "Au/Logger.hh"
#include "Au/Types.hh"

namespace Au {
enum class LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger
{
  private:
    LogLevel m_logLevel;
    bool     m_logToFile;
    Logger(LogLevel level = LogLevel::CRITICAL, bool logToFile = false)
        : m_logLevel{ level }
        , m_logToFile{ logToFile }
    {
        if (level < LogLevel::CRITICAL) {
            m_logLevel = level;
            return;
        }
        if (AU_BUILD_TYPE_RELEASE)
            m_logLevel = LogLevel::CRITICAL;
        else if (AU_BUILD_TYPE_DEBUG)
            m_logLevel = LogLevel::INFO;
        else if (AU_BUILD_TYPE_DEVELOPER)
            m_logLevel = LogLevel::DEBUG;
    }

    /**
     * @brief Append variables to the message
     * @param stream The stringstream to append to
     * @param value The value to append
     * @param args The rest of the values to append
     *
     * @return void
     */
    template<typename T, typename... Args>
    void appendVariables(std::stringstream& stream, T&& value, Args&&... args)
    {
        stream << " " << value;
        appendVariables(stream, std::forward<Args>(args)...);
    }

    /**
     * @brief Append variables to the message, The terminal condition for
     * appendVariables recursion
     * @param stream The stringstream to append to
     *
     * @return void
     */
    void appendVariables(std::stringstream& stream) {}

    /**
     * @brief Write the message to the log file
     * @param message The message to write
     *
     * @return void
     */
    void writeToLogFile(const String& message)
    {
        std::ofstream logFile;
        logFile.open("cpuidlog.txt", std::ios_base::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
        }
    }

    /**
     * @brief Write the message to the console
     * @param message The message to write
     *
     * @return void
     */
    void writeToConsole(const String& message)
    {
        std::cout << message << std::endl;
    }

  public:
    Logger(Logger const&)         = delete;
    void operator=(Logger const&) = delete;

    /**
     * @brief Get the logger instance
     * @return The logger instance
     */
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Set the log level
     * @param level The log level
     * @param logToFile If true, log to file
     * @return void
     */
    void setLevel(LogLevel level, bool logToFile = false)
    {
        this->m_logLevel  = level;
        this->m_logToFile = logToFile;
    }

    /**
     * @brief Log a message
     * @param level The log level
     * @param message The message to log
     * @param args The arguments to log
     * std::hex can be used to print variables in hex format
     * Example usage: log(LogLevel::DEBUG, "Value is: ", std::hex, value);
     *
     * @return void
     */
    template<typename... Args>
    void log(LogLevel level, const String& message, Args&&... args)
    {
        if (level < m_logLevel)
            return;
        String logLevelStr;
        switch (level) {
            case LogLevel::DEBUG:
                logLevelStr = "[DEBUG]";
                break;
            case LogLevel::INFO:
                logLevelStr = "[INFO]";
                break;
            case LogLevel::WARNING:
                logLevelStr = "[WARNING]";
                break;
            case LogLevel::ERROR:
                logLevelStr = "[ERROR]";
                break;
            case LogLevel::CRITICAL:
                logLevelStr = "[CRITICAL]";
                break;
        }

        std::time_t now       = std::time(nullptr);
        String timestamp = std::asctime(std::localtime(&now));
        timestamp =
            timestamp.substr(0, timestamp.length() - 1); // Remove newline

        std::stringstream logMessageStream;
        logMessageStream << timestamp << " " << logLevelStr << " " << message;
        appendVariables(logMessageStream, std::forward<Args>(args)...);

        String logMessage = logMessageStream.str();
        if (m_logToFile)
            writeToLogFile(logMessage);
        writeToConsole(logMessage);
    }
};

// Overload << operator for std::ostream to print variables in hex format
template<typename T, class = enableIf<std::is_integral<T>>>
std::ostream&
operator<<(std::ostream& os, const T& value)
{
    if constexpr (std::is_enum_v<T>) {
        os << std::hex << std::showbase
           << static_cast<std::underlying_type_t<T>>(value);
    } else {
        os << std::hex << std::showbase << value;
    }
    return os;
}

} // namespace Au
