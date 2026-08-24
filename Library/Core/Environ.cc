/*
 * Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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

using snapshot_mapT   = std::map<String, String, std::less<>>;
using snapshot_storeT = std::map<const Environ*, snapshot_mapT>;

static StringView const
snapshotValue(const Environ* instance, const String& key, const String& value)
{
    thread_local snapshot_storeT snapshot_store;
    auto&                        snapshots = snapshot_store[instance];
    auto snapshot = snapshots.insert_or_assign(key, value).first;
    return snapshot->second;
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

    auto& user_env = Env::getUserEnv();
    {
        std::lock_guard<std::mutex> lock(user_env.m_lock);
        auto&                       entries = user_env._entries();
        auto                        got     = entries.find(key);
        if (got != entries.end())
            return snapshotValue(&user_env, got->first, got->second);
    }

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
    AUD_ASSERT(envp, "Environment array is null");

    Env::getUserEnv().init(envp);
}

Environ::Environ()
    : m_environ{}
    , m_lock{}
{
    std::lock_guard<std::mutex> lock(m_lock);
    for (auto& i : Os::Env::getAll()) {
        _set(i.first, i.second);
    }
}

StringView const
Environ::get(StringView const key) const
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    /*
     * Copy under the lock into per-instance, per-key thread-local storage. This
     * avoids a view into a map node that another thread can erase. Other
     * instances and keys use different stable nodes; getting this key from this
     * instance again refreshes its snapshot and may invalidate the previous
     * view. Snapshots are released when the thread exits.
     */
    std::lock_guard<std::mutex> lock(m_lock);

    auto got = m_environ.find(key);
    if (got == m_environ.end())
        return m_empty_string;

    return snapshotValue(this, got->first, got->second);
}

void
Environ::set(String const& key, String const& val)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    std::lock_guard<std::mutex> lock(m_lock);
    _set(key, val);
}

void
Environ::unset(String const& key)
{
    AUD_ASSERT(key.length() != 0, "Key is empty");

    std::lock_guard<std::mutex> lock(m_lock);
    m_environ.erase(key);
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
    /* Precondition: m_lock is held by the caller. */
    AUD_ASSERT(key.length() != 0, "Key is empty");

    m_environ[key] = val;
}

void
Environ::init(const char** envp)
{
    AUD_ASSERT(envp, "Environment array is null");

    std::lock_guard<std::mutex> lock(m_lock);

    /* Replace the environment as a single critical section. */
    m_environ.clear();

    for (auto e = envp; e && *e; ++e) {
        auto [key, val] = Os::Env::split(String(*e));
        AUD_ASSERT(key.length() != 0, String{ "Invalid Key: " + key });
        AUD_ASSERT(val.length() != 0, String{ "Value is empty for " + key });

        _set(key, val);
    }
}

Environ::~Environ() = default;

} // namespace Au
