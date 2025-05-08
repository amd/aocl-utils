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
 */

#pragma once

#include "Au/Au.hh"
#include "Au/ErrorBase.hh"

namespace Au {

using ErrorCodeGeneric = Uint16;

/**
 * @class GenericError
 * @brief Implements generic error handling functionality
 * @details Provides basic error codes and messages for common error scenarios
 */
class GenericError final : public ErrorBase
{
  public:
    /**
     * @brief Default constructor that initializes with no error (OK state)
     */
    GenericError();

    /**
     * @brief Constructs GenericError with a specific error code
     * @param ecode The error code to initialize with
     */
    explicit GenericError(ErrorCodeGeneric ecode);

    AUD_DEFAULT_DTOR(GenericError);

    /**
     * @brief Get the human-readable error message
     * @return String containing the error message
     * @see GenericError.cc for the actual message strings
     */
    virtual String message() const override;

    /**
     * @brief Check if this represents an error condition
     * @return true if this represents an error, false if OK
     */
    virtual bool isError() const override;

  protected:
    /**
     * @brief Get the module identifier for generic errors
     * @return Always returns 0 for generic errors
     */
    virtual Uint16 getModuleId() const override { return 0; }
};

/*
 * Easy to use creators
 * usage:
 * Status my_func()
 * {
 *      return Status{Aborted("")};
 * }
 */
// clang-format off

/**
 * @brief Returns an Error object signifying an operation was aborted
 * @return Reference to an ErrorBase-derived object representing aborted state
 * @see GenericError.cc for the actual error message
 */
IError const& Aborted();

/**
 * @brief Returns an Error object signifying that a resource already exists
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& AlreadyExistsError();

/**
 * @brief Returns an Error object signifying an internal error
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& InternalError();

/**
 * @brief Returns an Error object for invalid arguments
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& InvalidArgumentError();

/**
 * @brief Returns an Error object signifying a resource was not found
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& NotFoundError();

/**
 * @brief Returns an Error object for an unavailable resource or functionality
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& NotAvailableError();

/**
 * @brief Returns an Error object indicating a feature is not implemented
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& NotImplementedError();

/**
 * @brief Returns an Error object signifying an unknown error
 * @return Reference to an ErrorBase-derived object representing this error
 * @see GenericError.cc for the actual error message
 */
IError const& UnknownError();

/**
 * @brief Returns an Error object representing success (no error)
 * @return Reference to an ErrorBase-derived object representing success
 * @see GenericError.cc for the actual error message
 */
IError const& NoError();

} // namespace Au
