/*
 * Copyright (C) 2022-2025, Advanced Micro Devices. All rights reserved.
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
    /**
     * @brief Constructs a new Status object from an existing IError reference
     *
     * @param ie Reference to an error or success state
     */
    explicit Status(IError const& ie)
        : m_code{ ie.code() }
        , m_is_ok{ !ie.isError() }
        , m_message{ ie.message() }
    {
    }

    /**
     * @brief Constructs a new Status object from an IError rvalue reference
     *
     * @param ie Rvalue reference to an error or success state
     */
    explicit Status(IError&& ie)
        : m_code{ ie.code() }
        , m_is_ok{ !ie.isError() }
        , m_message{ ie.message() }
    {
    }

    /**
     * @brief Constructs a new Status object from an IError reference with extra
     * text
     *
     * @param ie Reference to an error or success state
     * @param msg Additional message appended to the error description
     */
    explicit Status(IError const& ie, StringView msg)
        : Status{ ie }
    {
        m_message = makeMessage(ie.message(), msg);
        m_is_ok   = !ie.isError();
    }

    AUD_DEFAULT_CTOR_AND_DTOR(Status);
    AUD_DEFAULT_COPY_AND_ASSIGNMENT(Status);

    /**
     * @brief Checks if two Status objects have the same error code.
     *
     * @param other Another Status to compare with
     * @return true if codes match, false otherwise
     */
    bool operator==(const Status& other) const;

    /**
     * @brief Checks if two Status objects do not have the same error code.
     *
     * @param other Another Status to compare with
     * @return true if codes differ, false otherwise
     */
    bool operator!=(const Status& other) const;

    /**
     * @brief Indicates if the Status is error-free (ok).
     *
     * @return true if no error is present, false otherwise
     */
    AUD_MUST_USE_RETURN bool ok() const;
    String                   message() const { return m_message; }

    /**
     * @brief Returns encoded error code
     *
     * @return Uint64 encoded error code
     */
    Uint64 code() const { return m_code; }

    /**
     * @brief Update the error code and message only if there was no error
     * earlier
     *
     * @details This is done to preserve the very first error that happens
     *
     * @param ie IError interface from any component
     * @param msg Additional message to be added to the error message
     * @return true if the update was successful, false otherwise
     */
    bool update(IError& ie, const String& msg)
    {
        if (m_code)
            return false;

        m_code    = ie.code();
        m_message = makeMessage(ie.message(), msg);
        m_is_ok   = !ie.isError();
        return true;
    }

    /**
     * @brief Update the error code and message only if there was no error
     * earlier
     *
     * @details This is done to preserve the very first error that happens
     *
     * @param s New Status to be updated
     * @return true if the update was successful, false otherwise
     */
    bool update(const Status& s)
    {
        if (m_code)
            return false;

        m_code    = s.code();
        m_message = s.message();
        m_is_ok   = s.ok();
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
    bool   m_is_ok   = true;
    String m_message = {};

    // StringView m_err_message, m_err_specifics;
};

/**
 * @brief StatusOk()
 * Useful function when returning from a function
 *
 * @return Status  Status object with no error
 *
 * @example
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
    return m_is_ok;
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

/**
 * @brief Creates a Status indicating an "already exists" error.
 * @details This is useful when a resource is being created that already exists.
 *
 * @param msg String describing the specifics.
 * @return A Status with "already exists" error.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusAlreadyExists(StringView msg);

/**
 * @brief Creates a Status indicating an "internal error".
 * @param msg Descriptive message appended to the internal error.
 * @return A Status with "internal error".
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusInternalError(StringView msg);

/**
 * @brief Creates a Status indicating an "invalid argument" error.
 * @param msg Explanation of why the argument is invalid.
 * @return A Status with "invalid argument" error.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusInvalidArgument(StringView msg);

/**
 * @brief Creates a Status indicating a "not found" error.
 * @param msg Additional details about the missing resource.
 * @return A Status with "not found" error.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusNotFound(StringView msg);

/**
 * @brief Creates a Status indicating a "not available" error.
 * @param msg Explanation of what is unavailable.
 * @return A Status with "not available" error.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusNotAvailable(StringView msg);

/**
 * @brief Creates a Status indicating a "not implemented" error.
 * @param msg Additional descriptive text.
 * @return A Status with "not implemented" error.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusNotImplemented(StringView msg);

/**
 * @brief Creates a Status indicating an "unknown" error.
 * @param msg Supplemental message describing the unknown error.
 * @return A Status with an "unknown" error code.
 * @note Refer to GenericError.cc for the actual error string.
 */
Status StatusUnknown(StringView msg);
// clang-format on

} // namespace Au
