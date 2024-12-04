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

#include "Au/Assert.hh"
#include "Au/Au.hh"
#include "Au/BufferBase.hh"

namespace Au::Memory {

/**
 * @details BufferView is similar to std::string_view, a non-owning buffer
 *          Easy to work with of pre-allocated memory block from other parts
 *          For eg: A C-application owning a block sends it across. This
 *          provides Safe way to access the memory with bounds checks
 */
template<class T>
class BufferView final : public BufferBase<T>
{
    using valueT          = T;
    using pointerT        = T*;
    using constPointerT   = const T*;
    using referenceT      = T&;
    using constReferenceT = const T&;

  public:
    explicit BufferView(pointerT mem_block, size_t c_size)
        : BufferBase<T>{ mem_block, c_size }
    {
        AUD_ASSERT(BufferBase<T>::data() != nullptr, "Invalid Pointer");
        AUD_ASSERT(BufferBase<T>::size() >= 0, "Invalid size");
    }

    BufferView(const BufferView& other) = delete;

    /**
     * @brief  We do not allow copy of buffer across objects
     *
     * @param other       A BufferView type to copy from
     *
     * @return BufferView Returns reference to self
     */
    BufferView& operator=(const BufferView& other) = delete;

    T& operator[](size_t idx)
    {
        auto arr = reinterpret_cast<T*>(BufferBase<T>::data());
        AUD_ASSERT(idx < BufferBase<T>::size(), "Index out of bound");
        return arr[idx];
    }

    T* begin() { return reinterpret_cast<T*>(BufferBase<T>::data()); }

    const T* begin() const
    {
        return reinterpret_cast<const T*>(BufferBase<T>::data());
    }

    T* end()
    {
        return reinterpret_cast<T*>(BufferBase<T>::data())
               + BufferBase<T>::size();
    }

    const T* end() const
    {
        return reinterpret_cast<const T*>(BufferBase<T>::data())
               + BufferBase<T>::size();
    }

    // Disable all heap allocations
    void* operator new(std::size_t)   = delete;
    void* operator new[](std::size_t) = delete;

    ~BufferView() override = default;

  private:
    // void* const m_ptr; // pointer to existing memory block
    // size_t      m_size;
    // const size_t m_allocated_size; // whatever we got from user
};
} // namespace Au::Memory
