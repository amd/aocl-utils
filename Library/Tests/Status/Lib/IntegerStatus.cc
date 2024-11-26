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

#include "IntegerStatus.hh"

namespace Au::Testing::Status {

Status
StatusOk()
{
    static auto& err = NoError();
    return Status{ err };
}

Status
StatusAlreadyExists(StringView msg)
{
    static auto& err = AlreadyExistsError();
    return Status{ err, msg };
}

Status
StatusInternalError(StringView msg)
{
    static auto& err = InternalError();
    return Status{ err, msg };
}

Status
StatusInvalidArgument(StringView msg)
{
    static auto& err = InvalidArgumentError();
    return Status{ err, msg };
}

Status
StatusNotFound(StringView msg)
{
    static auto& err = NotFoundError();
    return Status{ err, msg };
}

Status
StatusNotAvailable(StringView msg)
{
    static auto& err = NotAvailableError();
    return Status{ err, msg };
}

Status
StatusNotImplemented(StringView msg)
{
    static auto& err = NotImplementedError();
    return Status{ err, msg };
}

Status
StatusUnknown(StringView msg)
{
    static auto& err = UnknownError();
    return Status{ err, msg };
}

} // namespace Au::Testing::Status
