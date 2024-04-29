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

#pragma once

#include "Au/Config.h"

#include "Au/Au.hh"
#include <string>

#ifdef AU_TARGET_OS_IS_LINUX
#include <cstdlib>
extern const char** environ;
#else // Windows environment ?
#endif

#include <map>
#include <tuple>
#include <vector>

namespace Au { namespace Os::Env {

    using env_map = std::map<String, String, std::less<>>;

    const String      spaces = "\t\n\r ";
    static inline String& trim_left(String& sv)
    {
        sv.erase(0, sv.find_first_not_of(spaces));
        return sv;
    }

    static inline String& trim_right(String& sv)
    {
        sv.erase(sv.find_last_not_of(spaces) + 1);
        return sv;
    }

    static inline String trim(String sv)
    {
        return trim_left(trim_right(sv));
    }

    using env_tuple = std::tuple<String, String>;
    static inline env_tuple split(String sv)
    {
        auto pos = sv.find('=');
        AUD_ASSERT(pos > 0 && pos < sv.length(), "Invalid pos");
        auto key = trim(sv.substr(0, pos));
        auto val = trim(sv.substr(pos + 1, sv.length()));

        return std::make_tuple(key, val);
    }

#ifdef AU_TARGET_OS_IS_LINUX
    namespace Linux {
        static env_map _getAll()
        {
            env_map __env;
            auto    envp = environ;

            for (; envp && *envp; ++envp) {
                String s(*envp);
                auto   keyval    = split(s);
                auto& [key, val] = keyval;
                __env[key]       = val;
            }

            return __env;
        }
    } // namespace Linux
#else
    namespace Windows {
        static env_map _getAll()
        {
            /* FIXME: Add windows specific code */
            env_map __env;

            return __env;
        }
    } // namespace Windows
#endif

    env_map getAll()
    {
#ifdef AU_TARGET_OS_IS_LINUX
        return Linux::_getAll();
#else
        return Windows::_getAll();
#endif
    }
}} // namespace Au::Os::Env
