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

#pragma once

#include "Au/Au.hh"
#include "Au/Interface/IError.hh"
#include "Au/Status.hh"

#include <unordered_map>

namespace Au {

enum DlErrorCode : Uint16
{
    eInvalidPath,
    eInvalidVersion,

    eSymbolNotFound,
    // eAlgorithmNotFound,
};

class DynamicLoaderError : public ErrorBase
{
  public:
    explicit DynamicLoaderError(DlErrorCode ecode);

  protected:
    String message() const { return __toStr(ErrorBase::getModuleError()); }

  private:
    static String const __toStr(Uint16 mod_err)
    {
        using ec        = DlErrorCode;
        using ErrorMapT = std::unordered_map<Uint16, String>;
        static const ErrorMapT err_to_str_map = {
            { ec::eInvalidPath, "Invalid Path Encountered" },
            { ec::eInvalidVersion, "Invalid Version number encountered" },
            { ec::eSymbolNotFound, "Requested Symbol not found" },
        };

        ErrorMapT::const_iterator it =
            err_to_str_map.find(static_cast<DlErrorCode>(mod_err));

        if (it != err_to_str_map.end()) {
            return it->second;
        } else {
            return String{ "DynamicLoader: Unknown Error Occured" };
        }
    }
};

namespace status {

    AUD_API_EXPORT Status InvalidPathError(StringView msg);
    AUD_API_EXPORT Status InvalidVersionError(StringView msg);

} // namespace status

} // namespace Au
