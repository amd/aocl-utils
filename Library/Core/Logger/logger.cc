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

#include "Au/Logger/logger.hh"

pthread_key_t Logger::m_key;

void
Logger::createKey()
{
    pthread_key_create(&Logger::m_key, nullptr);
}

Logger::Logger(LogWriter& logWriter)
    : m_storage{}
    , m_logWriter{ &logWriter }
{
    // Initialize key for all threads
    static pthread_once_t keyOnce = PTHREAD_ONCE_INIT;
    pthread_once(&keyOnce, Logger::createKey);

    m_storage = Logger::getStorage();
}

std::vector<Message>*
Logger::getStorage()
{
    std::vector<Message>* temp;
    if (pthread_getspecific(m_key) == nullptr) {
        temp = new std::vector<Message>();
        pthread_setspecific(m_key, temp);
    }
    temp = reinterpret_cast<std::vector<Message>*>(pthread_getspecific(m_key));
    return temp;
}

void
Logger::log(Message& msg)
{
    m_storage->push_back(msg);
}

void
Logger::flush()
{
    m_logWriter->log(*m_storage);
    m_storage->clear();
}

Logger::~Logger()
{
    // pthread_key_delete(m_key);
    // set thread local storage to nullptr
    pthread_setspecific(m_key, nullptr);
    delete m_storage;
}