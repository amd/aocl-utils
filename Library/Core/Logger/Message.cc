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

// C++ Standard header files
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

// C++ User header files
#include "Au/Logger/Message.hh"

namespace Au::Logger {

// Class Timestamp begins
Timestamp::Timestamp()
    : m_now(std::chrono::system_clock::now())
{
}

String
Timestamp::getTimestamp() const
{
    auto    now_c = std::chrono::system_clock::to_time_t(m_now);
    std::tm time  = {};

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time, &now_c); // Use localtime_s for thread safety on Windows
#else
    localtime_r(&now_c, &time); // Use localtime_r for thread safety on Linux
#endif

    std::ostringstream oss{};
    oss << std::put_time(&time, "%a %b %d %Y %H:%M:%S");
    return oss.str();
}

Uint64
Timestamp::getHour() const
{
    auto    now_c = std::chrono::system_clock::to_time_t(m_now);
    std::tm time  = {};

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time, &now_c); // Use localtime_s for thread safety on Windows
#else
    localtime_r(&now_c, &time); // Use localtime_r for thread safety on Linux
#endif

    return time.tm_hour;
}

Uint64
Timestamp::getMinute() const
{
    auto    now_c = std::chrono::system_clock::to_time_t(m_now);
    std::tm time  = {};

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time, &now_c); // Use localtime_s for thread safety on Windows
#else
    localtime_r(&now_c, &time); // Use localtime_r for thread safety on Linux
#endif

    return time.tm_min;
}

Uint64
Timestamp::getSecond() const
{
    auto    now_c = std::chrono::system_clock::to_time_t(m_now);
    std::tm time  = {};

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time, &now_c); // Use localtime_s for thread safety on Windows
#else
    localtime_r(&now_c, &time); // Use localtime_r for thread safety on Linux
#endif

    return time.tm_sec;
}

Uint64
Timestamp::getMillisecond() const
{
    // auto now_c = std::chrono::system_clock::to_time_t(m_now);
    // auto time  = std::localtime(&now_c);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               m_now.time_since_epoch())
        .count();
}

Uint64
Timestamp::getMicrosecond() const
{
    // auto now_c = std::chrono::system_clock::to_time_t(m_now);
    // auto time  = std::localtime(&now_c);
    return std::chrono::duration_cast<std::chrono::microseconds>(
               m_now.time_since_epoch())
        .count();
}

Uint64
Timestamp::getNanosecond() const
{
    // auto now_c = std::chrono::system_clock::to_time_t(m_now);
    // auto time  = std::localtime(&now_c);
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               m_now.time_since_epoch())
        .count();
}

// Class Timestamp ends

// Class Priority begins
std::unique_ptr<std::map<Priority::PriorityLevel, String>> Priority::m_str_map;
std::once_flag                                             Priority::initFlag;

std::map<Priority::PriorityLevel, String>*
Priority::getMap()
{
    if (m_str_map.get() == nullptr) {
        std::call_once(initFlag, []() {
            m_str_map = std::make_unique<std::map<PriorityLevel, String>>();
            *(m_str_map.get()) = { { PriorityLevel::eFatal, "Fatal" },
                                   { PriorityLevel::ePanic, "Panic" },
                                   { PriorityLevel::eError, "Error" },
                                   { PriorityLevel::eWarning, "Warning" },
                                   { PriorityLevel::eInfo, "Info" },
                                   { PriorityLevel::eNotice, "Notice" },
                                   { PriorityLevel::eDebug, "Debug" },
                                   { PriorityLevel::eTrace, "Trace" } };
        });
    }
    return m_str_map.get();
}

Priority::Priority()
    : m_level{ PriorityLevel::eInfo }
{
}

Priority::Priority(PriorityLevel level)
    : m_level{ level }
{
}

String
Priority::toStr() const
{
    auto map = Priority::getMap();
    return map->at(this->m_level);
}

// Operator overloads to compare the priority
bool
Priority::operator<(const Priority& rhs) const
{
    return this->m_level < rhs.m_level;
}

bool
Priority::operator>(const Priority& rhs) const
{
    return this->m_level > rhs.m_level;
}

bool
Priority::operator<=(const Priority& rhs) const
{
    return this->m_level <= rhs.m_level;
}

bool
Priority::operator>=(const Priority& rhs) const
{
    return this->m_level >= rhs.m_level;
}

bool
Priority::operator==(const Priority& rhs) const
{
    return this->m_level == rhs.m_level;
}

bool
Priority::operator!=(const Priority& rhs) const
{
    return this->m_level != rhs.m_level;
}

// Class Priority ends

// Class Message begins

Message::Message(const String& msg)
    : m_msg{ msg }
    , m_priority{ Priority() }
    , m_timestamp{ Timestamp() }
{
}

Message::Message(const String& msg, Priority& priority)
    : m_msg(msg)
    , m_priority{ priority }
    , m_timestamp{ Timestamp() }
{
}

String
Message::getMsg() const
{
    // Example "Mon Sep 02 2024 11:31:36  : Info : This is a message"
    std::ostringstream oss;
    oss << m_timestamp.getTimestamp() << " : " << std::left << std::setw(7)
        << m_priority.toStr() << " : " << m_msg;
    return oss.str();
}

Priority
Message::getPriority() const
{
    return m_priority;
}

Timestamp
Message::getTimestamp() const
{
    return m_timestamp;
}

// Class Message ends

} // namespace Au::Logger
