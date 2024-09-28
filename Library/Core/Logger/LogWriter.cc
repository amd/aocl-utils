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

#define USE_NO_LOCK 0

namespace Au::Logger {

// Class LogWriter begins
void
LogWriter::loggerThread()
{
    while (m_running) {
        if (!m_queue->empty()) {
            // Print the size of queue
            // if (verbose) {
            // std::cout << "Queue size: " << m_queue.getCount() << std::endl;
            // }
            Message msg = m_queue->dequeue();
            // std::cout << "HERE" << std::endl;

            // FIXME: Find better way
            if (msg.getMsg().find("Empty Queue") != String::npos) {
                continue;
            }

            for (auto& sink : m_sinks) {
                sink->write(msg);
                // Announce writing message
                // if (verbose) {
                // std::cout << "Writing message: " << msg.getMsg() <<
                // std::endl;
                // }
            }
        }
    }
}

LogWriter::LogWriter()
    : m_thread{}
    , m_sinks{}
    , m_running{ false }
    , m_queue{ nullptr }
{
    QueueFactory factory;
#if USE_NO_LOCK == 0
    m_queue = factory.createQueue("LockingQueue");
#else
    m_queue = factory.createQueue("NoLockQueue");
#endif
}

void
LogWriter::addSink(std::unique_ptr<ISink>& sink)
{
    m_sinks.push_back(std::move(sink));
}

void
LogWriter::removeSinkByType(const String& sinkType)
{
    for (auto it = m_sinks.begin(); it != m_sinks.end(); ++it) {
        if ((*it)->getSinkType() == sinkType) {
            m_sinks.erase(it);
            break;
        }
    }
}

void
LogWriter::removeSinkByName(const String& sinkName)
{
    for (auto it = m_sinks.begin(); it != m_sinks.end(); ++it) {
        if ((*it)->getSinkName() == sinkName) {
            m_sinks.erase(it);
            break;
        }
    }
}

void
LogWriter::start()
{
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
    while (!m_queue->empty()) {
        // Sleep for 0.01 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    m_running = false;
    m_thread.join();
}

LogWriter::~LogWriter()
{
    stop();

    // Flush all sinks
    for (auto it = m_sinks.begin(); it != m_sinks.end(); ++it) {
        (*it)->flush();
    }
}

void
LogWriter::log(std::vector<Message>& msgs)
{
    for (auto& msg : msgs) {
        m_queue->enqueue(msg);
    }
}

// Class LogWriter ends
} // namespace Au::Logger
