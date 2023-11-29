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

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Au {

using Uint8  = ::uint8_t;
using Int8   = ::int8_t;
using pUint8 = ::int8_t*;
using pInt8  = ::int8_t*;

using Uint16  = ::uint16_t;
using Int16   = ::int16_t;
using pUint16 = ::int16_t*;
using pInt16  = ::int16_t*;

using Uint32  = ::uint32_t;
using Int32   = ::int32_t;
using pUint32 = ::int32_t*;
using pInt32  = ::int32_t*;

using Uint64  = ::uint64_t;
using Int64   = ::int64_t;
using pUint64 = ::int64_t*;
using pInt64  = ::int64_t*;

using Float32 = float;
using Float64 = double;

//
// Some useful aliases
//

using String     = std::string;
using StringView = std::string_view;
//
//
//
//

/**
 * Some useful macros:
 *
 *       template <typename T,
 *       Au::enableIf<std::is_arithmetic<T>>>
 *       class Size {
 *         public:
 *           explicit Size(T s)
 *            :m_size{static_cast<LDouble>(s)}
 *           {}
 *
 *           auto KiB()
 *           {
 *               return m_size / 1024.0;
 *           }
 *        };
 *
 */

template<typename cond, typename T = void>
using enableIf = typename std::enable_if<cond::value, T>::type;

template<typename cond, typename T = void>
using disableIf = typename std::enable_if<!cond::value, T>::type;

} // namespace Au
