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
#include "Au/BufferBase.hh"

#include <memory>

namespace Au::Memory {

/**
 * @details     Creates a Buffer on the stack, similar to one using an
 *              array on the stack. Gets automatically de-allocated
 *              at the end of scope
 *
 * {
 *  BufferLocal<Uint32, 20> u20{};
 *  u20[2] = 5;
 *  u20[21] = 8; // should fail, at least in assert build
 * }
 *
 *
 */
template<size_t SIZE, class T = std::byte>
class BufferLocal final : public BufferBase<T>
{
    using valueT          = T;
    using pointerT        = T*;
    using constPointerT   = const T*;
    using referenceT      = T&;
    using constReferenceT = const T&;

  public:
    BufferLocal()
        : BufferBase<T>{ m_buf.data(), SIZE }
        , m_buf{}
    {
        AUD_ASSERT(SIZE >= 0, "Size is not 0 or possitive");
    }
    ~BufferLocal();

    // Disable all heap allocations
    void* operator new(std::size_t)   = delete;
    void* operator new[](std::size_t) = delete;

    referenceT operator[](size_t index)
    {
        AUD_ASSERT(index <= BufferBase<T>::size(), "Index out of bounds");
        return m_buf[index];
    }

  private:
#if 0
    struct _Deleter
    {
        void operator()(pointerT p)
        {
            /*
             * We'll not call free() or delete here, as the memory
             * owned is locally on the stack, will be freed upon
             * exit from the scope
             */
        }
    };
    using uniquePtrT = std::unique_ptr<T, _Deleter>;
#endif
  private:
    std::array<valueT, SIZE> m_buf;
    // uniquePtrT        m_ptr;
    // const std::size_t m_size;
};

template<size_t S, class T>
BufferLocal<S, T>::~BufferLocal()
{
}

} // namespace Au::Memory
