/*
 * Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
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

#include "Au/Au.hh"
#include "Au/Error.hh"

namespace Au {

/*
 * Example:
 *
 *   Au::Status sts = DoSomething();
 *   if (!sts.ok()) {
 *     LOG(ERROR) << sts;
 *   }
 */

class Status final
{
  public:
    explicit Status(IError const& ie)
        : m_code{ ie.code() }
        , m_message{ ie.message() }
    {
    }

    explicit Status(IError&& ie)
        : m_code{ ie.code() }
        , m_message{ ie.message() }
    {
    }

    explicit Status(IError const& ie, StringView msg)
        : Status{ ie }
    {
        m_message = makeMessage(ie.message(), msg);
    }

    AUD_DEFAULT_CTOR_AND_DTOR(Status);
    AUD_DEFAULT_COPY_AND_ASSIGNMENT(Status);

    bool operator==(const Status& other) const;
    bool operator!=(const Status& other) const;

    // Status::ok()
    // All is Well !!! if m_error is eOk or eNone
    AUD_MUST_USE_RETURN bool ok() const;
    String                   message() const { return m_message; }

    /**
     * @name code()
     *
     * @detail
     * Returns encoded error code
     *
     * @params
     * n/a
     *
     * @result          Uint64          encoded error code
     */
    Uint64 code() const { return m_code; }

    /**
     * @name update()
     *
     * @detail
     * Update the error code and message only if there was no error earlier;
     * this is done to presever the very first error that happens
     *
     * @param[in]       ie      IError interface from any component
     * @param[in]       msg     Additional message to be added to the error
     * message
     *
     * @return          boolean Indication if the update was successful
     */
    bool update(IError& ie, const String& msg)
    {
        if (m_code)
            return false;

        m_code    = ie.code();
        m_message = makeMessage(ie.message(), msg);
        return true;
    }

    /**
     * @name update()
     *
     * @detail
     * Update the error code and message only if there was no error earlier;
     * this is done to presever the very first error that happens
     *
     * @param[in]       s      New Status to be updated
     *
     * @return          boolean Indication if the update was successful
     */
    bool update(const Status& s)
    {
        if (m_code)
            return false;

        m_code    = s.code();
        m_message = s.message();
        return true;
    }

  private:
    String& makeMessage(const String& base_error, const String& details)
    {
        m_message = base_error + String(": ") + String(" ") + details;
        return m_message;
    }

    String& makeMessage(const StringView& base_error, const StringView& details)
    {
        m_message = String(base_error) + String(": ") + String(details);
        return m_message;
    }

    Uint64 m_code    = {};
    String m_message = {};

    // StringView m_err_message, m_err_specifics;
};

/**
 * @brief StatusOk()
 * Useful function when returning from a function
 *
 * Status some_function(some_arg_t arg)
 * {
 *    // .. do something important ..
 *
 *    return StatusOk();
 * }
 */
inline Status
StatusOk()
{
    auto& ie = NoError();

    return Status{ ie };
}

inline bool
Status::operator==(const Status& other) const
{
    return this->m_code == other.m_code;
}

inline bool
Status::ok() const
{
    return m_code == 0;
}

// clang-format off
/*
 * Easy to use chekers
 */
AUD_MUST_USE_RETURN bool IsAborted(const Status& status);
AUD_MUST_USE_RETURN bool IsAlreadyExists(const Status& status);
AUD_MUST_USE_RETURN bool IsInternal(const Status& status);
AUD_MUST_USE_RETURN bool IsInvalidArgument(const Status& status);
AUD_MUST_USE_RETURN bool IsNotFound(const Status& status);
AUD_MUST_USE_RETURN bool IsNotAvailable(const Status& status);
AUD_MUST_USE_RETURN bool IsNotImplemented(const Status& status);
AUD_MUST_USE_RETURN bool IsUnknown(const Status& status);


Status StatusAlreadyExists(StringView msg);
Status StatusInternalError(StringView msg);
Status StatusInvalidArgument(StringView msg);
Status StatusNotFound(StringView msg);
Status StatusNotAvailable(StringView msg);
Status StatusNotImplemented(StringView msg);
Status StatusUnknown(StringView msg);
// clang-format on

} // namespace Au
