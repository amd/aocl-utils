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

#include "Au/Environ.hh"

#include "gtest/gtest.h"

#include <atomic>
#include <cstdlib>
#include <string>
#include <thread>

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
    auto orig_home = Env::get(homeEnv);
    auto dummytxt  = "abcd";

    Env::set(homeEnv, dummytxt);
    EXPECT_EQ(Env::get(homeEnv), dummytxt);

    Env::set(homeEnv, String(orig_home));
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

    auto orig_home = Env::get(homeEnv);

    Env::set(homeEnv, "abcd");
    EXPECT_STREQ(Env::get(homeEnv).data(), "abcd");

    // std::getenv gets it from system environment, independent of our
    // cached values.
    EXPECT_STRNE(Env::get(homeEnv).data(), std::getenv(homeEnv));

    Env::set(homeEnv, orig_home.data());
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

TEST(Environ, getViewOutlivesMutation)
{
    // A view returned by get() must keep reading its original value even after
    // the key is overwritten, unset, and the environment re-initialised.
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

TEST(Environ, concurrentSetUnsetGetIsSafe)
{
    // Concurrent readers and mutators on the same key must not race, crash, or
    // hand back a dangling view. Meant to run clean under TSan and ASan.
    Env::set("CONC_KEY", "seed");

    constexpr int    kIters = 20000;
    std::atomic<bool> go{ false };

    auto waitGo = [&] {
        while (!go.load(std::memory_order_acquire)) {
        }
    };

    std::thread reader([&] {
        waitGo();
        for (int i = 0; i < kIters; ++i) {
            StringView v = Env::get("CONC_KEY");
            if (!v.empty()) {
                volatile char c = v.data()[0]; // touch first byte only when present
                (void)c;
            }
        }
    });
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
    reader.join();
    setter.join();
    unsetter.join();

    SUCCEED();
}

} // namespace
