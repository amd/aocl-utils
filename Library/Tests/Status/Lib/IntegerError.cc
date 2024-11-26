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

#include "IntegerError.hh"

using namespace Au::Testing::Status;

#include <string>
#include <string_view>
#include <unordered_map>

namespace Au::Testing::Status {
using Au::ErrorBase;
using Au::ErrorCodeGeneric;

/**
 * Some of the Error codes are similar but based on the
 * application one makes more sense than other.
 * Application developers are encouraged to use them accordingly.
 */
enum IntegerErrorCode : ErrorCodeGeneric
{
    /* ErrorCode:eOk
     *
     * Though this does not signify an error, but a status
     * that is a success. Application should always check
     * for this before proceeding.
     */
    eOk   = 0,
    eNone = eOk,

    /* ErrorCode:eUknown
     *
     * An error has occured but cannot be categorized under
     * any of the other
     */
    eUnknown = 1,

    /* ErrorCode:eInvalidArgument
     *
     * Argument passed to a function, or part of the configuration
     * Should be used to indicate that the application cannot request
     * for services as the configuration sent across is not valid
     */
    eInvalidArgument = 2,

    /* ErrorCode:eNotFound
     *
     * Useful in a collection/file when searching
     */
    eNotFound = 4,

    /* ErrorCode:eExists
     *
     * A plugin that already exists, but register is called again
     * A file that to be created but an entry exits, etc
     */
    eExists = 8,

    /* ErrorCode::eNotImplemented
     *
     * A feature, function, subsystem not yet implemented
     */
    eNotImplemented = 16,

    /* ErrorCode::eNotAvailable
     *
     * A feature, function, sybsystem, or a device exists/implemented but
     * not available for use
     */
    eNotAvailable = 32,

    /* ErrorCode::eInternal
     *
     * Internal Error could be described by rest of the error code
     */
    eInternal = 64,

    /* ErrorCode::eMaxDontUse
     *
     * Dont use, here to mark the largest error code
     */
    eMaxDontUse = 128,
};

IntegerError::IntegerError()
    : IntegerError{ IntegerErrorCode::eOk }
{
}

IntegerError::IntegerError(ErrorCodeGeneric ecode)
    : ErrorBase{}
{
    m_error.field.base_error = ecode;
}

std::string
IntegerError::message() const
{

    using ec        = IntegerErrorCode;
    using ErrorMapT = std::unordered_map<uint16_t, std::string>;

    static const ErrorMapT str_map = {
        { ec::eOk, "No Error" },
        { ec::eExists, "Already Exists" },
        { ec::eInternal, "Internal Math Error" },
        { ec::eInvalidArgument, "Argument Out of Scope" },
        { ec::eNotAvailable, "Not Available" },
        { ec::eNotFound, "Not Found" },
        { ec::eNotImplemented, "Not Implemented" },
    };

    ErrorMapT::const_iterator it = str_map.find(getBaseError());

    std::string s{};

    if (it != str_map.end()) {
        s = it->second;
    } else {
        s = std::string("Unknown Error");
    }

    return s;
}

bool
IntegerError::isError() const
{
    return getBaseError() != IntegerErrorCode::eOk;
}

IError const&
Aborted()
{
    static IntegerError gr{ IntegerErrorCode::eInternal };
    return gr;
}

IError const&
NoError()
{
    static IntegerError gr{ IntegerErrorCode::eOk };
    return gr;
}

IError const&
AlreadyExistsError()
{
    static IntegerError gr{ IntegerErrorCode::eExists };
    return gr;
}

IError const&
InternalError()
{
    static IntegerError gr{ IntegerErrorCode::eInternal };
    return gr;
}

IError const&
InvalidArgumentError()
{
    static IntegerError gr{ IntegerErrorCode::eInvalidArgument };
    return gr;
}

IError const&
NotFoundError()
{
    static IntegerError gr{ IntegerErrorCode::eNotFound };
    return gr;
}

IError const&
NotAvailableError()
{
    static IntegerError gr{ IntegerErrorCode::eNotAvailable };
    return gr;
}

IError const&
NotImplementedError()
{
    static IntegerError gr{ IntegerErrorCode::eNotImplemented };
    return gr;
}

IError const&
UnknownError()
{
    static IntegerError gr{ IntegerErrorCode::eUnknown };
    return gr;
}

} // namespace Au::Testing::Status
