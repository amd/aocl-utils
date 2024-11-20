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

#include "Au/Logger/LogWriter.hh"

#include <iostream>
#include <mutex>

#define USE_NO_LOCK 0

namespace Au::Logger {

std::shared_ptr<LogWriter> LogWriter::instance = nullptr;
std::mutex                 LogWriter::instanceMutex;

// Class LogWriter begins
void
LogWriter::loggerThread()
{
    while (m_running) {
        if (!m_queue.empty()) {
            Message msg = m_queue.dequeue();

            // FIXME: Find better way, move to assert
            /*
            if (msg.getMsg().find("Empty Queue") != String::npos) {
                continue;
            }
            */
            m_logger->write(msg);
        }
    }
}

LogWriter::LogWriter()
    : m_thread{}
    , m_logger{ std::make_unique<ConsoleLogger>() } // Default to ConsoleLogger
    , m_running{ false }
    , m_queue{}
{
}

std::shared_ptr<LogWriter>
LogWriter::getLogWriter()
{
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance = std::shared_ptr<LogWriter>(new LogWriter());
    }
    if (instance->m_running == false)
        instance->start();
    return instance;
}

void
LogWriter::setLogger(std::unique_ptr<ILogger> logger)
{
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (instance) {
        instance->m_logger = std::move(logger);
    } else {
        instance           = std::shared_ptr<LogWriter>(new LogWriter());
        instance->m_logger = std::move(logger);
    }
}

void
LogWriter::start()
{
    if (m_running) {
        return;
    }
    m_running = true;
    m_thread  = std::thread(&LogWriter::loggerThread, this);
}

void
LogWriter::stop()
{
    if (!m_running) {
        return;
    }

    // Wait for queue to be empty
    while (!m_queue.empty()) {
        // Sleep for 0.01 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    m_running = false;
    m_logger->flush();
    m_thread.join();

    instance.reset();
}

LogWriter::~LogWriter()
{
    stop();
}

void
LogWriter::log(std::vector<Message>& msgs)
{
    for (auto& msg : msgs) {
        m_queue.enqueue(msg);
    }
}

// Class LogWriter ends
} // namespace Au::Logger
