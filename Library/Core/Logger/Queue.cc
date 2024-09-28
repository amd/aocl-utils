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

// Class LockingQueueElement begins
LockingQueueElement::LockingQueueElement(const Message msg)
    : m_msg{ msg }
    , m_next{ nullptr }
{
}
// Class LockingQueueElement ends

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
    LockingQueueElement*        newElement = new LockingQueueElement(msg);
    if (m_queue == nullptr) {
        m_queue = newElement;
        m_tail  = newElement;
    } else {
        m_tail->m_next = newElement;
        m_tail         = newElement;
    }
    m_count++;
}

Message
LockingQueue::dequeue()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue == nullptr) {
        return Message("Empty Queue");
    }
    LockingQueueElement* oldhead = m_queue;
    m_queue                      = oldhead->m_next;
    Message msg                  = std::move(oldhead->m_msg);
    delete oldhead;
    m_count--;
    return msg;
}

bool
LockingQueue::empty()
{
    return getCount() == 0;
}

Uint64
LockingQueue::getCount()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_count;
}
// Class LockingQueue ends

// Class QueueElement begins
NoLockQueueElement::NoLockQueueElement(const Message msg)
    : m_msg{ msg }
    , m_next{ nullptr }
{
}
// Class QueueElement ends

// Class NoLockQueue begins
NoLockQueue::NoLockQueue()
    : m_queue{ nullptr }
    , m_tail{ nullptr }
    , m_count{ 0 }
{
}

void
NoLockQueue::enqueue(const Message& msg)
{
    // Atomic QueueElement creation
    NoLockQueueElement* newElement = new NoLockQueueElement(msg);
    NoLockQueueElement* oldtail =
        m_tail.exchange(newElement, std::memory_order_acq_rel);

    if (oldtail != nullptr) {
        oldtail->m_next.store(newElement, std::memory_order_acq_rel);
    }

    // If queue is empty, set head to newElement
    if (m_queue.load(std::memory_order_acquire) == nullptr) {
        m_queue.store(newElement, std::memory_order_acq_rel);
    }

    // Increment m_count
    m_count.fetch_add(1, std::memory_order_acq_rel);
}

Message
NoLockQueue::dequeue()
{
    NoLockQueueElement* oldhead = m_queue.load(std::memory_order_acquire);
    if (oldhead == nullptr) {
        // Decrement m_count
        m_count.fetch_sub(1, std::memory_order_acq_rel); // To prevent deadlock
        // Release lock on m_queue
        m_queue.store(nullptr, std::memory_order_release);
        return Message("Empty Queue");
    }
    NoLockQueueElement* nextnode =
        oldhead->m_next.load(std::memory_order_acquire);

    if (nextnode == nullptr) {
        // Set head and tail to nullptr
        m_tail.store(nullptr, std::memory_order_acq_rel);
    }

    Message msg = std::move(oldhead->m_msg);
    m_queue.store(oldhead->m_next.load(std::memory_order_release),
                  std::memory_order_release);

    // Decrement m_count
    m_count.fetch_sub(1, std::memory_order_acq_rel);

    // Delete oldhead after returning msg
    delete oldhead;

    return msg;
}

bool
NoLockQueue::empty()
{
    // print getCount
    // std::cout << "Count: " << getCount() << std::endl;
    // print m_queue
    // std::cout << "m_queue: " << m_queue.load(std::memory_order_relaxed)
    //           << std::endl;
    return getCount() == 0;
}

Uint64
NoLockQueue::getCount()
{
    return m_count.load(std::memory_order_acq_rel);
}

// Class NoLockQueue ends

// Class QueueFactory begins
std::unique_ptr<IQueue>
QueueFactory::createQueue(const String& queueType)
{
    if (queueType == "NoLockQueue") {
        return std::make_unique<NoLockQueue>();
    } else if (queueType == "LockingQueue") {
        return std::make_unique<LockingQueue>();
    } else {
        return nullptr; // Marking no queue was created
    }
}

bool
QueueFactory::validateQueueType(const String& queueType)
{
    std::vector<String> validQueueTypes = { "NoLockQueue", "LockingQueue" };
    for (auto& type : validQueueTypes) {
        if (type == queueType) {
            return true;
        }
    }
    return false;
}
} // namespace Au::Logger
