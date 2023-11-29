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
#pragma once

#include "Au/Au.hh"
#include "Au/Interface/IBuffer.hh"
#include "Au/Pattern/DisableMoveCopy.hh"

namespace Au::Memory {

template<class T>
class BufferView;

template<class T>
class BufferBase
    : public IBuffer<T>
    , public DisableCopyMove
{
    using valueT          = T;
    using pointerT        = T*;
    using constPointerT   = const T*;
    using referenceT      = T&;
    using constReferenceT = const T&;

  public:
    BufferBase(pointerT ptr, size_t size)
        : m_size{ size }
        , m_ptr{ ptr }
    {
    }

    virtual ~BufferBase();

    BufferBase(const BufferBase&)            = delete;
    BufferBase& operator=(const BufferBase&) = delete;

  public:
    // void copyFrom();

    /**
     * @brief  return a pointer to byte sized memory
     * @param  N/A
     *
     * @return  byte *  A pointer to location of byte 0
     */
    pointerT      ptr() { return m_ptr; }
    constPointerT ptr() const { return m_ptr; }

    /**
     * @brief clear the contents pointed m_ptr
     * @return none
     */
    void erase();

    /**
     * @brief  */
    BufferView<T> getRange(size_t offset, size_t size) const;

    constPointerT const data() const { return m_ptr; }
    pointerT            data() { return m_ptr; }
    size_t              size() const { return m_size; }

    size_t len() const { return m_size * sizeof(valueT); }
    bool   isAllocated() const { return m_ptr != nullptr && m_size != 0; }

    operator valueT*() const { return m_ptr; }

  private:
    size_t   m_size = 0;
    pointerT m_ptr; // = nullptr;
};

template<class T>
BufferBase<T>::~BufferBase()
{
}

} // namespace Au::Memory
