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

#include "Au/Buffer.hh"

#include <cstdlib>
#include <cstring> /* for memset() */

namespace Au::Memory {

template<class T>
void
BufferBase<T>::erase()
{
    std::memset(m_ptr, 0, m_size);
}

template<class T>
T*
Buffer<T>::allocate(size_t size, size_t alignment)
{
    std::byte* ptr =
        static_cast<std::byte*>(std::aligned_alloc(alignment, size));

    if ((uintptr_t)ptr & ~alignment) {
        BufferBase<T>::m_ptr  = ptr;
        BufferBase<T>::m_size = size;
        return ptr;
    }

    Buffer::free();
    return nullptr;
}

template<class T>
void
Buffer<T>::free()
{
    if (BufferBase<T>::isAllocated()) {
        std::free(BufferBase<T>::m_ptr);
        BufferBase<T>::m_ptr  = nullptr;
        BufferBase<T>::m_size = 0;
    }
}

} // namespace Au::Memory
