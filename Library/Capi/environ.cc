/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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

#include "Au/Environ.hh"
#include "Au/Au.hh"

#include "Capi/au/environ.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

using namespace Au;

static inline void
str_tolower(String& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
}

AUD_EXTERN_C_BEGIN;

AUD_API_EXPORT const char*
au_env_get(const char* name)
{
    if (!name)
        return nullptr;

    StringView sv{ name };

    auto val = Env::get(sv);

    return val.data();
}

AUD_API_EXPORT void
au_env_unset(const char* name)
{
    if (!name)
        return;
    Env::unset(name);
}

AUD_API_EXPORT const char*
au_env_set(const char* name, const char* val)
{
    if (!name || !val)
        return nullptr;

    String n{ name };
    String v{ val };

    Env::set(n, v);

    return name;
}

AUD_API_EXPORT bool
au_env_is_enabled(const char* cc)
{
    if (!cc)
        return false;

    String sv{
        Env::get(cc).data()
    }; /* returned string from Env::get() is a const */

    str_tolower(sv);

    if (sv == String("yes") || sv == String("on") || sv == String("1")
        || sv == String("true"))
        return true;

    return false;
}

AUD_API_EXPORT bool
au_env_is_set(const char* cc)
{
    if (!cc)
        return false;

    StringView sv{ cc };
    auto       val = Env::get(sv);

    if (val.length() == 0)
        return false;

    return true;
}

AUD_EXTERN_C_END;
