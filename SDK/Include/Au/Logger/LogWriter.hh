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
#include "Au/Logger/Logger.hh"
#include "Au/Logger/Queue.hh"

namespace Au::Logger {

// FIXME: Make LogWriter singleton
/**
 * @brief LogWriter class
 */
class LogWriter
{
  private:
    std::thread                       m_thread;
    std::unique_ptr<ILogger>          m_logger;
    std::atomic<bool>                 m_running; // Modified to atomic boolean
    LockingQueue                      m_queue;
    static std::mutex                 instanceMutex;
    static std::shared_ptr<LogWriter> instance;

    void loggerThread();
    LogWriter(); // Private constructor for singleton

  public:
    LogWriter(const LogWriter&)            = delete;
    LogWriter& operator=(const LogWriter&) = delete;
    ~LogWriter();
    static std::shared_ptr<LogWriter> getLogWriter();
    static void setLogger(std::unique_ptr<ILogger> logger);
    void        start();
    void        stop();
    void        log(std::vector<Message>& msgs);
};
} // namespace Au::Logger
