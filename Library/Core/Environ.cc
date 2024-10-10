/*
 * Copyright (C) 2022-2022, Advanced Micro Devices. All rights reserved.
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

/**
 * FIXME:
 * Research
 * https://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/include/corelib/ncbienv.hpp#0156
 *
 */
#include "Au/Environ.hh"
#include "Au/Assert.hh"
#include "Os/Env.hh"

#include <mutex>

namespace Au {

static Environ&
__getStaticUserEnv()
{
    static Environ gsUserEnv{};
    return gsUserEnv;
}

static Environ&
__getStaticSystemEnv()
{
    static Environ gsSystem{};
    return gsSystem;
}

Environ&
Env::getUserEnv()
{
    return __getStaticUserEnv();
}

Environ&
Env::getSystemEnv()
{
    return __getStaticSystemEnv();
}

StringView const
Env::get(StringView const& key)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    if (Env::getUserEnv().exists(key))
        return Env::getUserEnv().get(key);

    return Env::getSystemEnv().get(key);
}

void
Env::set(String const& key, String const& val)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    Env::getUserEnv().set(key, val);
}

void
Env::unset(String const& key)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    Env::getUserEnv().unset(key);
}

void
Env::init(const char** envp)
{
    AUD_ASSERT(envp, "Key is empty");

    Env::getUserEnv().init(envp);
}

Environ::Environ()
    : m_environ{}
    , m_lock{}
{
    for (auto& i : Os::Env::getAll()) {
        m_environ.emplace(i);
    }
}

StringView const
Environ::get(StringView const key) const
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    if (exists(key)) {
        return m_environ.at(std::move(String{ key }));
    }

    return m_empty_string;
}

void
Environ::set(String const& key, String const& val)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    if (exists(key)) {
        /* FIXME: add this to log */
        // AUD_ASSERT(false, "Overrwriting existing key");
    }

    _set(key, val);
}

void
Environ::unset(String const& key)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    if (exists(key)) {
        m_environ.erase(key);
    }
}

bool
Environ::exists(StringView const& key) const
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    std::lock_guard<std::mutex> lock(m_lock);

    auto got = m_environ.find(key);
    if (got == m_environ.end())
        return false;

    return true;
}

void
Environ::_set(String const& key, String const& val)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");
    //AUD_ASSERT(val.length() != 0, "Value of key/val pair is empty");

    std::lock_guard<std::mutex> lock(m_lock);
    m_environ[key] = val;
}

void
Environ::init(const char** envp)
{
    auto e = envp;

    m_environ.clear();

    for (; e && *e; ++e) {
        String s(*e);
        if (*e == NULL) /* We reached end of list */
            break;

        auto keyval     = Os::Env::split(s);
        auto [key, val] = keyval;
        AUD_ASSERT(key.length() != 0, String{ "Invalid Key: " + key });
        AUD_ASSERT(val.length() != 0, String{ "Value is empty for " + key });

        _set(key, val);
    }
}

Environ::~Environ() = default;

} // namespace Au
