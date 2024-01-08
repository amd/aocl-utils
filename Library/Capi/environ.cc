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

#include "Au/Environ.hh"
#include "Au/Au.hh"

#include "Capi/au/environ.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

using namespace Au;

template<typename S>
static inline S
str_tolower(S s)
{
    S t;
    std::transform(s.begin(), s.end(), t.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return t;
}

AUD_EXTERN_C_BEGIN;

const char* AUD_API_EXPORT
au_env_get(const char* name)
{
    if (!name)
        return nullptr;

    StringView sv{ name };

    auto val = Env::get(sv);

    return val.data();
}

void AUD_API_EXPORT
au_env_unset(const char* name)
{
    if (!name)
        return;
    Env::unset(name);
}

const char* AUD_API_EXPORT
au_env_set(const char* name, const char* val)
{
    if (!name || !val)
        return nullptr;

    String n{ name };
    String v{ val };

    Env::set(n, v);

    return name;
}

bool AUD_API_EXPORT
au_env_is_enabled(const char* cc)
{
    if (!cc)
        return false;

    String sv{
        Env::get(cc).data()
    }; /* returned string from Env::get() is a const */
    auto val = str_tolower(sv);

    if (val == String("yes") || val == String("1") || val == String("true"))
        return true;

    return false;
}

bool AUD_API_EXPORT
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
