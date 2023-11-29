/*
 * Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

#pragma once

#include "Au/Au.hh"

#include <map>
#include <mutex>
#include <string_view>

namespace Au {

class Environ
{
  public:
    AUD_DECLARE_CTOR_AND_DTOR(Environ);
    void init(const char** envp);

  public:
    /**
     * @brief   'get' usual getter functions, retrieves the environment
     *      variable's value
     *
     * @param[in]   key  Readonly reference to a String like object
     *
     * @return  String  Environtment Variables value or a null string
     */
    StringView const get(StringView key) const;
    // StringView get(StringView key) const;

  public:
    /**
     * @brief 'set' function will update if the key exists, creates
     *      new key-value pair otherwise
     *
     * @param[in]  key     Environment variable
     * @param[in]  value   Environment variable's value
     *
     * @return nothing
     */
    void set(String const& key, String const& value);

    /**
     * @brief 'unset' a key in the user environment
     *           The modifications are limited to user-env
     *
     * @param[in] key name of the key to be unset
     *
     * return nothing
     */
    void unset(String const& key);

    /**
     * @brief search if a key exists in current environ
     *
     * @param key Environment variable key
     *
     * @return true if key exists
     *         false if not
     */
    bool exists(StringView const& key) const;

  private:
    void _set(String const& key, String const& val);

    using env_mapT = std::map<String, String, std::less<>>;
    env_mapT           m_environ;
    mutable std::mutex m_lock;
    const String       m_empty_string{ "" };
};

class Env
{

    /**
     * @brief  Getters for the User Env and System Env
     */
    static Environ& getUserEnv();
    static Environ& getSystemEnv();

  public:
    /**
     * @brief Getter for environment variable
     *
     * @details The preference is given to UserEnv as it is subjected to
     * modification if the key is not present in UserEnv then SystemEnv is
     * searched
     *
     * @param[in]    key  Name of the key to get
     *
     * return StringView of the value for given 'key'
     */
    static StringView const get(StringView const& key);

    /**
     * @brief Setter for environment variable
     *
     * @details Only UserEnv is subjected to modification
     *
     * @param[in]    key  Name of the key to get
     * @param[in]    val  Value of the environment variable
     *
     * return StringView of the value for given 'key'
     */
    static void set(String const& key, String const& val);

    /**
     * @brief Setter for environment variable
     *
     * @details Only UserEnv is subjected to modification
     *
     * @param[in]    key  Name of the key to get
     * @param[in]    val  Value of the environment variable
     *
     * return StringView of the value for given 'key'
     */
    static void unset(String const& key);

    /**
     * @brief Initialize new UserEnv from a given array of string values
     *
     * @details Only UserEnv is subjected to modification
     *
     * @param[in]    key  Name of the key to get
     * @param[in]    val  Value of the environment variable
     *
     * return StringView of the value for given 'key'
     */
    static void init(const char** envp);
};

} // namespace Au
