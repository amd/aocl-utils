/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 *    without specific prior written permission.
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

#include "Au/Au.hh"

#include "Os/Thread.hh"

namespace Au {

class ThreadBase
{

  public:
    using ThreadIdT                 = Os::ThreadIdT;
    static constexpr Uint32 NR_CPUS = 1024;

  public:
    AUD_DEFAULT_CTOR_AND_VIRTUAL_DTOR(ThreadBase);

    explicit ThreadBase(ThreadIdT tid)
        : m_thread_id{ tid } {};

  protected:
    ThreadIdT m_thread_id;
};

class Thread final
    : public ThreadBase
    , public std::thread
{
  public:
    using ThreadFuncT = Os::ThreadFuncT;

  public:
    /**
     * @brief:
     * @param:
     * @return
     */
    ThreadIdT getId() const;

    /**
     * @brief:
     * @param:
     * @return
     */
    void sleep() const;

    /**
     * @brief  Start the thread with given function parameter
     * @param[in] tfptr Pointer to a function to run
     * @return nothing
     */
    void start(ThreadFuncT tfptr);

    /**
     * @brief   Join the thread
     * @param
     * @return
     */

  private:
    ThreadIdT __getId() const;
};

} // namespace Au
