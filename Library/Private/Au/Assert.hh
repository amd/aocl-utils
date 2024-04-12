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

#include "Au/Config.h"

#include "Au/Au.hh"
#include "Au/Exception.hh"
#include "Au/SourceLocation.hh"

#include <string_view>
#include <sys/errno.h>

namespace Au {

#if defined(DEBUG) || AU_BUILD_TYPE_DEBUG
#define AU_USE_ASSERTIONS 1
#endif

#if defined(AU_USE_ASSERTIONS)
static constexpr bool AssertionsBuild = true;
#else
static constexpr bool AssertionsBuild = false;
#endif

template<typename E>
inline void
AssertOther(StringView s, SourceLocation const& loc)
{
    if constexpr (AssertionsBuild) {
        E myexcept{ loc, String{ s }, ENOSYS };
        throw myexcept;
    }
}

/**
 * @brief Assert using exceptions
 *
 * @tparam T Assert condition
 * @tparam E Exception to raise when assertion condition fails
 * @param assrt     is of type T
 * @param s         Source code location to print FILE:LINE:FUNCTION
 */
template<typename T, typename E = FatalErrorException>
inline void
Assert(T&& assrt, StringView s, const SourceLocation& loc)
{
    if constexpr (AssertionsBuild) {
        if (!assrt) {
            E throwing{ loc, String{ s }, EINVAL };
            throw throwing;
        }
    }
}

#if defined(AU_USE_ASSERTIONS)

#define AUD_ASSERT(cond, msg)                                                  \
    Au::Assert(cond, std::string_view(msg), AUD_SOURCE_LOCATION())

#define AUD_ASSERT_NOT_IMPLEMENTED()                                           \
    Au::AssertOther<NotImplementedException>("Not Implmented",                 \
                                             AUD_SOURCE_LOCATION())

#else

#define AUD_ASSERT(cond, msg)                                                  \
    {                                                                          \
    }

#define AUD_ASSERT_NOT_IMPLEMENTED()                                           \
    {                                                                          \
    }

#endif // if AU_USE_ASSERTIONS

} // namespace Au
