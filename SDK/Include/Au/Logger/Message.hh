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

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "Au/Types.hh"

using Au::String;
namespace Au::Logger {

// Class to create a timestamp
/**
 * @class Timestamp
 * @brief Represents a time point and provides convenience methods for
 * extracting time components.
 */
class Timestamp
{
  private:
    std::chrono::time_point<std::chrono::system_clock> m_now;

  public:
    Timestamp();
    /**
     * @brief Returns a formatted timestamp string.
     * @return Timestamp in "HH:MM:SS.milli" format.
     */
    String getTimestamp() const;
    Uint64 getHour() const;
    Uint64 getMinute() const;
    Uint64 getSecond() const;
    Uint64 getMillisecond() const;
    Uint64 getMicrosecond() const;
    Uint64 getNanosecond() const;
};

// Class to keep the priority of the log message
/**
 * @class Priority
 * @brief Indicates the severity level of a log message.
 *
 * Use PriorityLevel to distinguish between various log severities.
 */
class Priority
{

  public:
    enum class PriorityLevel
    {
        eFatal   = (1 << 0), /* Highest, Application will terminate (mostly) */
        ePanic   = (1 << 1), /* Application might not work as expected */
        eError   = (1 << 2), /* API wont complete, but app will continue */
        eWarning = (1 << 3), /* API completes (with error), app will continue */
        eNotice  = (1 << 4), /* Just like INFO but higher priority */
        eInfo    = (1 << 5), /* usually denoting successful completion  */
        eDebug   = (1 << 6), /* For debugging purpose only */
        eTrace   = (1 << 7), /* Lowest priority */
    };

    Priority();
    explicit Priority(PriorityLevel level);

    String toStr() const;

    // Operator overloads to compare the priority
    bool operator<(const Priority& rhs) const;
    bool operator>(const Priority& rhs) const;
    bool operator<=(const Priority& rhs) const;
    bool operator>=(const Priority& rhs) const;
    bool operator==(const Priority& rhs) const;
    bool operator!=(const Priority& rhs) const;

    static std::map<Priority::PriorityLevel, String>* getMap();

  private:
    static std::unique_ptr<std::map<PriorityLevel, String>> m_str_map;
    static std::once_flag                                   initFlag;

    PriorityLevel m_level;
};

// Class for message.
/**
 * @class Message
 * @brief Encapsulates a log message with content, priority, and timestamp.
 */
class Message
{
  private:
    String    m_msg;       ///< Log message
    Priority  m_priority;  ///< Priority of the message
    Timestamp m_timestamp; ///< Timestamp of the message

  public:
    /**
     * @brief Constructor for Message.
     * @param msg Log message.
     */
    explicit Message(const String& msg);

    /**
     * @brief Constructor for Message with priority.
     * @param msg Log message.
     * @param priority Priority of the message.
     */
    explicit Message(const String& msg, Priority& priority);

    /**
     * @brief Get the log message.
     * @return Log message.
     */
    String getMsg() const;

    /**
     * @brief Get the priority of the message.
     * @return Priority of the message.
     */
    Priority getPriority() const;

    /**
     * @brief Get the timestamp of the message.
     * @return Timestamp of the message.
     */
    Timestamp getTimestamp() const;
};
} // namespace Au::Logger
