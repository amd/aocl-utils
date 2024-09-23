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
#include "Au/Logger/message.hh"
namespace Au::Logger {

class IQueue
{
  public:
    virtual void    enqueue(const Message& msg) = 0;
    virtual Message dequeue()                   = 0;
    virtual bool    empty()                     = 0;
    virtual Uint64  getCount()                  = 0;
    virtual ~IQueue()                           = default;
};

class LockingQueueElement
{
  public:
    Message m_msg;

    explicit LockingQueueElement(const Message msg);

    // Disable copy constructor and assignment operator
    LockingQueueElement(const LockingQueueElement&)            = delete;
    LockingQueueElement& operator=(const LockingQueueElement&) = delete;

    LockingQueueElement* m_next;
};

class LockingQueue : public IQueue
{
  private:
    std::mutex           m_mutex;
    LockingQueueElement* m_queue = nullptr; // Head of the queue
    LockingQueueElement* m_tail  = nullptr; // Tail of the queue
    Uint64               m_count = 0;

  public:
    LockingQueue();

    // Disable copy constructor and assignment operator
    LockingQueue(const LockingQueue&)            = delete;
    LockingQueue& operator=(const LockingQueue&) = delete;

    virtual void    enqueue(const Message& msg) override;
    virtual Message dequeue() override;
    virtual bool    empty() override;
    virtual Uint64  getCount() override;

    ~LockingQueue() override = default;
};

// Classes implementing no locking queue

class NoLockQueueElement
{
  public:
    Message m_msg;
    explicit NoLockQueueElement(const Message msg);
    std::atomic<NoLockQueueElement*> m_next;
};

/**
 * @brief Queue using atomic operations
 */
class NoLockQueue : public IQueue
{
  private:
    std::vector<String>              log_messages = {};
    std::atomic<NoLockQueueElement*> m_queue = nullptr; // Head of the queue
    std::atomic<NoLockQueueElement*> m_tail  = nullptr; // Tail of the queue
    std::atomic<Uint64>              m_count = 0;       // Atomic count variable

  public:
    NoLockQueue();
    void    enqueue(const Message& msg) override;
    Message dequeue() override;
    bool    empty() override;
    Uint64  getCount() override;
};

// Class QueueFactory begins
class QueueFactory
{
  public:
    static std::unique_ptr<IQueue> createQueue(const String& queueType);
    static bool                    validateQueueType(const String& queueType);
};

} // namespace Au::Logger
