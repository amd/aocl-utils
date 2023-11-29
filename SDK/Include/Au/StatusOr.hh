/*
 * Copyright (C) 2022-2022, Advanced Micro Devices. All rights reserved.
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
 */

#pragma once

#include "Au/Assert.hh"
#include "Au/Error.hh"
#include "Au/Status.hh"

#include "Error.hh"

#include <optional>

namespace Au {

template<typename T>
class StatusOr
{
  public:
    using value_type = T;
    using type       = T;

  public:
    inline StatusOr();
    inline StatusOr(Au::Status& sts);

    inline StatusOr(const T& val); // copy ctor
    inline StatusOr(T&& val);      // move ctor

    AUD_DEFAULT_COPY_AND_ASSIGNMENT(StatusOr);

    template<typename U>
    StatusOr(const StatusOr<U>& sts);

    template<typename U>
    const StatusOr& operator=(const StatusOr<U>& sts);

    /**
     * @brief status() will return the underlying status
     * Status doWork()
     * {
     *      StatusOr<Aes256Context> sts = Aes::Build("aes-256-cbc")
     *
     *      if (!sts.ok()) {
     *          return sts.status();
     *      }
     *
     *      auto val = *sts;
     *
     *      // ... do something with val which is an 'Aes256Context'
     * }
     *
     * @return const Status&
     */
    inline const Status& status() const { return m_status; }

    inline void set(Status const& st)
    {
        if (ok())
            m_status = st;
    }

    inline void set(T& v)
    {
        if (ok())
            m_value = v;
    }

    /**
     * @brief ok() will return if underlaying status was ok
     *
     * @return true if actual status was ok(), false otherwise
     */
    inline bool ok() const { return m_status.ok(); }

    T*       operator->() { return &(m_value.value()); }
    const T* operator->() const { return &(m_value.value()); }

    T&        operator*() & { return *m_value; }
    T const&  operator*() const& { return *m_value; }
    T&&       operator*() && { return *std::move(m_value); }
    T const&& operator*() const&& { return *std::move(m_value); }

    inline T const&  value() const& { return m_value; }
    inline T&        value() & { return *m_value; }
    inline T const&& value() const&& { return *std::move(m_value); }
    inline T&&       value() && { return *std::move(m_value); }

  private:
    Status           m_status;
    std::optional<T> m_value;

    template<typename U>
    friend class StatusOr;

  private:
    inline bool assertNotOk() const
    {
        AUD_ASSERT(!m_status.ok(), m_status.message());
        return true;
    }
};

template<typename T>
inline StatusOr<T>::StatusOr()
    : m_status{ UnknownError() }
{
}

template<typename T>
inline StatusOr<T>::StatusOr(Au::Status& sts)
    : m_status{ sts }
{
    AUD_ASSERT(!m_status.ok(), "Assigned status not ok!!");
}

template<typename T>
inline StatusOr<T>::StatusOr(const T& value)
    : m_status{ StatusOk() }
    , m_value{ value }
{
}

template<typename T>
inline StatusOr<T>::StatusOr(T&& value)
    : m_status{ StatusOk() }
    , m_value{ std::move(value) }
{
}

template<typename T>
template<typename U>
inline StatusOr<T>::StatusOr(const StatusOr<U>& sts)
    : m_status{ sts.m_status }
    , m_value{ sts.m_value }
{
}
} // namespace Au
