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

/**
 * @class LogWriter
 * @brief Manages the logging thread and writes messages through a chosen
 * logger.
 */
class LogWriter
{
  private:
    std::thread              m_thread; ///< Thread for logging
    std::unique_ptr<ILogger> m_logger; ///< Logger instance
    std::atomic<bool> m_running; ///< Atomic boolean to control running state
    LockingQueue      m_queue;   ///< Queue for log messages
    static std::mutex instanceMutex; ///< Mutex for singleton instance
    static std::shared_ptr<LogWriter> instance; ///< Singleton instance

    /**
     * @brief Main function executed by the logging thread to process queued
     * messages.
     */
    void loggerThread();

    /**
     * @brief Private constructor for singleton pattern.
     */
    LogWriter();

  public:
    LogWriter(const LogWriter&)            = delete;
    LogWriter& operator=(const LogWriter&) = delete;

    /**
     * @brief Destructor for LogWriter.
     */
    ~LogWriter();

    /**
     * @brief Retrieves the singleton instance of LogWriter, creating it if
     * necessary.
     * @return A shared pointer to the unique LogWriter instance.
     */
    static std::shared_ptr<LogWriter> getLogWriter();

    /**
     * @brief Specifies the ILogger implementation to use for output.
     * @param logger A unique pointer to an ILogger-derived object.
     */
    static void setLogger(std::unique_ptr<ILogger> logger);

    /**
     * @brief Starts the dedicated logging thread.
     */
    void start();

    /**
     * @brief Signals the logging thread to stop and waits for it to finish.
     */
    void stop();

    /**
     * @brief Sends a batch of messages to the logging queue.
     * @param msgs A vector of log messages to enqueue.
     */
    void log(std::vector<Message>& msgs);
};
} // namespace Au::Logger
