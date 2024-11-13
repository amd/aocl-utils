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
 */

#pragma once

#include "Au/Au.hh"
#include "Au/ErrorBase.hh"

namespace Au {

using ErrorCodeGeneric = Uint16;

class GenericError final : public ErrorBase
{
  public:
    GenericError();
    explicit GenericError(ErrorCodeGeneric ecode);

    AUD_DEFAULT_DTOR(GenericError);
    virtual String message() const override;
    virtual bool   isError() const override;

  protected:
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
IError const& Aborted();
IError const& AlreadyExistsError();
IError const& InternalError();
IError const& InvalidArgumentError();
IError const& NotFoundError();
IError const& NotAvailableError();
IError const& NotImplementedError();
IError const& UnknownError();
IError const& NoError();

} // namespace Au
