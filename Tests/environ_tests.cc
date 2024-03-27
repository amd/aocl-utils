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

#include "Capi/au/environ.h"

#include "gtest/gtest.h"

#include <cstdlib>

namespace {

TEST(Integration, au_env_get)
{
    EXPECT_STREQ(au_env_get("HOME"), std::getenv("HOME"));
}

TEST(Integration, au_env_set)
{
    au_env_set("MYNAME", "AOCL");
    EXPECT_STREQ(au_env_get("MYNAME"), "AOCL");

    /* cleanup */
    au_env_unset("MYNAME");
}

TEST(Integration, au_env_is_enabled_true)
{
    { /* test uppercase TRUE */
        au_env_set("MYOPTION1", "TRUE");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION1"));
        au_env_unset("MYOPTION2");
    }

    { /* test lowercase 'true' */
        au_env_set("MYOPTION4", "true");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION4"));
        au_env_unset("MYOPTION4");
    }

    { /* mixed case 'TrUe' */
        au_env_set("MYOPTION31", "TrUe");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION31"));
        au_env_unset("MYOPTION31");
    }
}

TEST(Integration, au_env_is_enabled_yes)
{
    { /* test for YES */
        au_env_set("MYOPTION3", "YES");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION3"));
        au_env_unset("MYOPTION3");
    }
    { /* test lowercase 'yes' */
        au_env_set("MYOPTION5", "yes");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION5"));
        au_env_unset("MYOPTION5");
    }

    { /* test mixed 'yes' */
        au_env_set("MYOPTION51", "yEs");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION51"));
        au_env_unset("MYOPTION51");
    }
}

TEST(Integration, au_env_is_enabled_on)
{
    { /* test lowercase 'on' */
        au_env_set("MYOPTION6", "on");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION6"));
        au_env_unset("MYOPTION6");
    }
    { /* set to ON */
        au_env_set("MYOPTION2", "ON");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION2"));
        au_env_unset("MYOPTION2");
    }

    { /* set to 1 */
        au_env_set("MYOPTION21", "1");
        EXPECT_TRUE(au_env_is_enabled("MYOPTION21"));
        au_env_unset("MYOPTION21");
    }
}

TEST(Integration, au_env_is_enabled_other)
{

    { /* garbage, should return false */
        au_env_set("MYOPTION4", "gArbAge");
        EXPECT_FALSE(au_env_is_enabled("MYOPTION4"));
        au_env_unset("MYOPTION4");
    }

    {
        au_env_set("MYOPTION41", "10");
        EXPECT_FALSE(au_env_is_enabled("MYOPTION41"));
        au_env_unset("MYOPTION41");
    }

    {
        au_env_set("MYOPTION42", "15.03");
        EXPECT_FALSE(au_env_is_enabled("MYOPTION42"));
        au_env_unset("MYOPTION42");
    }
}

TEST(Integration, au_env_is_set)
{

    { /* garbage, should return false */
        EXPECT_FALSE(au_env_is_set("MYENV"));
        au_env_set("MYENV", "gArbAge");
        EXPECT_TRUE(au_env_is_set("MYENV"));
        au_env_unset("MYENV");
    }
}

} // namespace
