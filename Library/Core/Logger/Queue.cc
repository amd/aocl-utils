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

#include "Au/Logger/Queue.hh"

namespace Au::Logger {

// Class LockingQueue begins
LockingQueue::LockingQueue()
    : m_mutex{}
    , m_queue{}
{
}

void
LockingQueue::enqueue(const Message& msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(msg);
}

Message
LockingQueue::dequeue()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        return Message("Empty Queue");
    }
    Message msg = std::move(m_queue.front());
    m_queue.pop_front();
    return msg;
}

bool
LockingQueue::empty()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

Uint64
LockingQueue::getCount()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}
// Class LockingQueue ends
} // namespace Au::Logger
