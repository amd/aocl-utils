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

#include "Au/BitMap.hh"
#include <memory>

namespace Au {

BitMap::BitMap(Uint32 size)
    : m_lock{}
    , m_size{ size }
    , m_nwords{ size / bits_per_word() + 1 }
{
    m_bit_words.resize(m_nwords);
}

BitMap::~BitMap() {}

Uint32
BitMap::count() const
{
    Uint32 count = 0;

    return count;
}

Uint32
BitMap::size() const
{
    return m_size;
}

void
BitMap::set()
{
    std::lock_guard<std::mutex> lock(m_lock);
    auto                        len = m_size / bits_to_word();
    auto                        ptr = m_bit_words.get();

    while (len--) {
        ptr[len] = all_set();
    }
}

void
BitMap::set(Uint32 bitpos)
{
    std::lock_guard<std::mutex> lock(m_lock);
    auto                        off = bitpos / bits_to_word();
    auto                        ptr = m_bit_words.get();

    bitpos = bitpos % bits_to_word();

    ptr[off] |= 1 << bitpos;
}

void
BitMap::set(BitMap const& other)
{
}

void
BitMap::clear()
{
    std::lock_guard<std::mutex> lock(m_lock);
}

void
BitMap::clear(Uint32 bitpos)
{
    std::lock_guard<std::mutex> lock(m_lock);
}

void
BitMap::clear(BitMap const& other)
{
    std::lock_guard<std::mutex> lock(m_lock);
}

void
BitMap::flip()
{
    std::lock_guard<std::mutex> lock(m_lock);

    for (auto& i : m_bit_words) {
        i = (~i);
    }
}

void
BitMap::flip(Uint32 bitpos)
{
    AUD_ASSERT(bitpos <= m_size, "Invalid bit position");

    std::lock_guard<std::mutex> lock(m_lock);

    /* TODO:
     * 1. calculate word for 'bitpos'
     * 2. calculate word offset for 'bitpos'
     * 3. m_bit_words[i] ^= (1<<offset);
     */
}

void
BitMap::flip(BitMap const& other)
{
    std::lock_guard<std::mutex> lock(m_lock);
}

bool
BitMap::isFull() const
{
    for (auto a : m_bit_words) {
        if (!all_set_mask(a))
            return false;
    }

    return true;
}

} // namespace Au
