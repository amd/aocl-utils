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

#include "Au/Environ.hh"

#include "gtest/gtest.h"

#include <atomic>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#ifndef TEST
#define TEST(suite, name) void suite##_##name()
#endif

#if defined(WIN32) || defined(_WINDOWS)
auto homeEnv = "USERPROFILE";
#else
auto homeEnv = "HOME";
#endif

namespace {
using namespace Au;

/**
 * IMPORTANT:
 * Since we use the global environment of application,
 * The test will modify its own environment.
 * Hence the order of these tests are important
 * be mindful of Env::init() being called.
 */

TEST(Environ, getUserEnv)
{
    // Get a user environment ($USER, or $HOME)
    auto home = Env::get(homeEnv);

    EXPECT_STREQ(home.data(), std::getenv(homeEnv));
}

TEST(Environ, setNewUserEnv)
{
    const auto nss = "not-so-sweet";
    Env::set("SWEETHOME", nss);
    auto shome = Env::get("SWEETHOME");

    /* Comparing returned pointer */
    EXPECT_EQ(shome, nss);

    /* Comparing text */
    EXPECT_STREQ(shome.data(), nss);
}

TEST(Environ, checkInit)
{
    Env::set("SWEETHOME", "yes-it-is-sweet");

    EXPECT_TRUE(Env::get("SWEETHOME").length() > 1);

    {
        const char* envp[] = {
            NULL,
        };
        Env::init(envp);
    }

    EXPECT_STREQ(Env::get("SWEETHOME").data(), "");
}

TEST(Environ, setExistingUserEnv)
{
    // A same-key get intervenes below, so retain the original in owning
    // storage.
    String orig_home{ Env::get(homeEnv) };
    auto   dummytxt = "abcd";

    Env::set(homeEnv, dummytxt);
    EXPECT_EQ(Env::get(homeEnv), dummytxt);

    Env::set(homeEnv, orig_home);
    EXPECT_EQ(Env::get(homeEnv), orig_home);
}

TEST(Environ, setUserEmpty)
{
    const char* envp[] = { NULL };

    Env::init(envp); /* get fresh userenv */

    /* in fresh env, we expect no SWEETHOME key */
    EXPECT_STREQ(Env::get("SWEETHOME").data(), "");

    Env::set("SWEETHOME", "not-so-sweet");
    EXPECT_STREQ(Env::get("SWEETHOME").data(), "not-so-sweet");

    Env::set("SWEETHOME", "");
    EXPECT_STREQ(Env::get("SWEETHOME").data(), "");
}

TEST(Environ, checkExists)
{
    const char* envp[] = {
        "MYHOME=/foo/bar",
        "MYNAME=foobar",
        "MYJOB=barfoo",
        NULL,
    };

    Env::init(envp);

    EXPECT_STREQ(Env::get("MYHOME").data(), "/foo/bar");
    EXPECT_STREQ(Env::get("MYNAME").data(), "foobar");
    EXPECT_STREQ(Env::get("MYJOB").data(), "barfoo");
}

TEST(Environ, checkSpaces)
{
    const char* envp[] = {
        "MYHOME = /foo/bar",
        " MYNAME = foobar",
        "MYJOB = barfoo ",
        " MYCAR = bugati ",
        NULL,
    };

    Env::init(envp);

    EXPECT_EQ(Env::get("MYHOME"), "/foo/bar");
    EXPECT_EQ(Env::get("MYNAME"), "foobar");
    EXPECT_EQ(Env::get("MYJOB"), "barfoo");
    EXPECT_EQ(Env::get("MYCAR"), "bugati");
}

TEST(Environ, checkSystemEnv)
{
    // modifications to Userenv
    // should not change system env

    // Same-key gets intervene before restore, so retain the original value.
    String orig_home{ Env::get(homeEnv) };

    Env::set(homeEnv, "abcd");
    EXPECT_STREQ(Env::get(homeEnv).data(), "abcd");

    // std::getenv gets it from system environment, independent of our
    // cached values.
    EXPECT_STRNE(Env::get(homeEnv).data(), std::getenv(homeEnv));

    Env::set(homeEnv, orig_home);
    EXPECT_STREQ(Env::get(homeEnv).data(), std::getenv(homeEnv));
}

TEST(Environ, checkScopedString)
{
    // We want to test if the stored 'std::string' in Env is a copy
    {
        {
            const char* envp[] = {
                NULL,
            };
            Env::init(envp);
            Env::set("SWEETHOME", "yes-it-is-sweet");
        }
    }

    EXPECT_STREQ(Env::get("SWEETHOME").data(), "yes-it-is-sweet");
}

TEST(Environ, getViewSurvivesMutationWithoutInterveningGet)
{
    // A view aliases a per-key snapshot, so mutating the backing map does not
    // invalidate it until this thread gets the same key again.
    Env::set("STABLE_KEY", "original-value");

    StringView captured = Env::get("STABLE_KEY");
    EXPECT_EQ(captured, "original-value");

    Env::set("STABLE_KEY", "a-different-and-much-longer-replacement-value");
    Env::unset("STABLE_KEY");
    {
        const char* envp[] = {
            "SOMETHING=else",
            NULL,
        };
        Env::init(envp);
    }

    EXPECT_EQ(captured, "original-value");
    EXPECT_STREQ(captured.data(), "original-value");
}

TEST(Environ, getViewsForDifferentKeysRemainIndependent)
{
    Env::set("FIRST_KEY", "first-value");
    Env::set("SECOND_KEY", "second-value");

    StringView first     = Env::get("FIRST_KEY");
    auto       getSecond = [] { return Env::get("SECOND_KEY"); };
    StringView second    = getSecond();

    EXPECT_EQ(first, "first-value");
    EXPECT_EQ(second, "second-value");
}

TEST(Environ, getViewsForDifferentInstancesRemainIndependent)
{
    Environ first_env;
    Environ second_env;
    first_env.set("SHARED_KEY", "first-instance-value");
    second_env.set("SHARED_KEY", "second-instance-value");

    StringView first  = first_env.get("SHARED_KEY");
    StringView second = second_env.get("SHARED_KEY");

    EXPECT_EQ(first, "first-instance-value");
    EXPECT_EQ(second, "second-instance-value");
}

TEST(Environ, concurrentUserMutationPreservesSystemFallback)
{
    const char* system_value = std::getenv(homeEnv);
    ASSERT_NE(system_value, nullptr);
    ASSERT_NE(*system_value, '\0');
    Env::unset(homeEnv);

    constexpr int       kIters = 100000;
    std::atomic<bool>   go{ false };
    std::atomic<size_t> missing{ 0 };

    std::thread writer([&] {
        while (!go.load(std::memory_order_acquire)) {
        }
        for (int i = 0; i < kIters; ++i) {
            Env::set(homeEnv, "override");
            Env::unset(homeEnv);
        }
    });
    std::thread reader([&] {
        go.store(true, std::memory_order_release);
        for (int i = 0; i < kIters; ++i) {
            if (Env::get(homeEnv).empty())
                missing.fetch_add(1, std::memory_order_relaxed);
        }
    });

    writer.join();
    reader.join();
    EXPECT_EQ(missing.load(), 0U);
}

TEST(Environ, concurrentSetUnsetGetIsSafe)
{
    // Concurrent readers and mutators on the same key must not race, crash, or
    // hand back a dangling view. Each reader reads only its own get() result
    // (its private per-key snapshot) while other threads churn the map, so
    // this must be clean under TSan and ASan.
    Env::set("CONC_KEY", "seed");

    constexpr int     kIters   = 20000;
    constexpr int     kReaders = 4;
    std::atomic<bool> go{ false };

    auto waitGo = [&] {
        while (!go.load(std::memory_order_acquire)) {
        }
    };

    std::vector<std::thread> readers;
    for (int r = 0; r < kReaders; ++r) {
        readers.emplace_back([&] {
            waitGo();
            for (int i = 0; i < kIters; ++i) {
                StringView v = Env::get("CONC_KEY");
                if (!v.empty()) {
                    // Read every byte of this thread's own snapshot; a shared
                    // or dangling buffer would surface as a race/UAF here.
                    volatile char c = 0;
                    for (char ch : v)
                        c = ch;
                    (void)c;
                }
            }
        });
    }
    std::thread setter([&] {
        waitGo();
        for (int i = 0; i < kIters; ++i)
            Env::set("CONC_KEY", "value-" + std::to_string(i % 8));
    });
    std::thread unsetter([&] {
        waitGo();
        for (int i = 0; i < kIters; ++i)
            Env::unset("CONC_KEY");
    });

    go.store(true, std::memory_order_release);
    for (auto& t : readers)
        t.join();
    setter.join();
    unsetter.join();

    SUCCEED();
}

} // namespace
