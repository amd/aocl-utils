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
 */

#pragma once

#include "Au/Assert.hh"
#include "Au/Au.hh"

#include <limits>
#include <type_traits> // for std::enable_if*

namespace Au {

/**
 * @brief Checks if two signed integers has opposite signs
 *
 * @tparam T    Any signed integer type
 * @tparam std::enable_if_t<std::is_signed<T>::value>
 *
 * @param a     Integer a
 * @param b     Integer b
 *
 * @return true if @a and @b has opposite signs
 * @return false if @a and @b are both possitive or both negative
 */
template<typename T, class = enableIf<std::is_integral<T>>>
inline bool
IsOppositeSign(T a, T b)
{
    return ((a ^ b) < 0);
}

/**
 * @brief   Check if given value is a power of 2
 *
 * @tparam T
 * @tparam std::enable_if_t<std::is_integral<T>::value>
 *
 * @param[in]  val  Value to be checked for power of 2
 *
 * @return true     if param 'a' is power of 2, false otherwise
 */
template<typename T, class = enableIf<std::is_integral<T>>>
inline bool
IsPowerOfTwo(T const& val)
{
    return (val && !(val & (val - 1)));
}

/**
 * @brief Set the Bit object
 *
 * @tparam T
 * @tparam std::enable_if_t<std::is_integral<T>::value>
 *
 * @param[in]       val  Value to set bits in
 * @param           bit  Bit position to set
 *
 * @return T        New integral value with bit set
 */
template<typename T, class = enableIf<std::is_integral<T>>>
inline T
SetBit(T val, long bit)
{
    AUD_ASSERT(bit > 0 || ((unsigned)bit < (sizeof(T) * 8)),
               "Expect bit position as positive integer");

    return (val | (static_cast<T>(1) << bit));
}

/**
 * @brief  Sign Extend a given integer
 *
 * @tparam  T
 * @tparam  enableIf<std::is_integral<T>>>
 *
 * @param[in]    val     Value to sign extend to
 * @param[in]    index   Index at which sign extension should start
 *
 * @return T     New sign extend integer
 */
template<typename T, class = enableIf<std::is_integral<T>>>
inline T
SignExtend(T const& val, int index)
{
    const auto bits  = std::numeric_limits<T>::digits;
    auto       shift = bits - index;

    return (val << shift) >> shift;
}

} // namespace Au
