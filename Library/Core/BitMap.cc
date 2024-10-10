/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#include "Au/BitMap.hh"
#include "Au/Assert.hh"
#include "Au/Au.hh"
#include "Au/Exception.hh"

#include <functional>
#include <immintrin.h>

namespace Au {

enum class Operation
{
    eOpSet,
    eOpClear,
    eOpFlip
};

template<typename T>
static inline void
OpOr(T& x, T const& y)
{
    x |= y;
}

template<typename T>
static inline void
OpClear(T& x, T const& y)
{
    x &= ~y;
}

template<typename T>
static inline bool
OpEq(T const& x, T const& y)
{
    AUD_ASSERT_NOT_IMPLEMENTED();
    // x &= ~y;
    return false;
}

template<typename T>
static inline void
OpNot(T& x, T const& y)
{
    x = ~x;
}

template<typename T>
static inline void
OpXor(T& x, T const& y)
{
    x ^= y;
}

template<typename T>
static auto
getOp(Operation op = Operation::eOpSet) -> std::function<void(T&, T const&)>
{
    if (op == Operation::eOpClear)
        return OpClear<T>;

    if (op == Operation::eOpFlip)
        return OpXor<T>;

    return OpOr<T>;
}

Uint32
BitMap::size() const
{
    return m_size;
}

/**
 * NOTE:
 *   Assumption last word which is partially used has other bits set to 0
 */
Uint32
BitMap::count() const
{
    auto cnt = 0;

    std::lock_guard<std::mutex> lock(m_lock);
    for (auto& word : m_bit_words) {
#if defined(__GNUG__) || defined(__clang__)
        cnt += __builtin_popcountll(word);
#else
        cnt += __popcnt64(word);
#endif
    }
    return cnt;
}

void
BitMap::doSetOrClear(bool doclear)
{
    auto remaining_bits = m_size;
    auto mask           = doclear ? ~getAllSetMask() : getAllSetMask();
    auto op =
        getOp<BitMapWordT>(doclear ? Operation::eOpClear : Operation::eOpSet);

    for (auto& i : m_bit_words) {
        if (remaining_bits < bitsPerWord()) {
            mask &= (1ULL << remaining_bits) - 1;
        }

        /* Moving critical section to its own block */
        if (true) {
            std::lock_guard<std::mutex> lock(m_lock);
            op(i, mask);
        }

        remaining_bits -= bitsPerWord();
    }
}

void
BitMap::doSetOrClear(BitMap const& other, bool doclear)
{
    auto words = other.m_nwords;
    auto sz    = other.size();

    if (size() < sz) {
        words = m_nwords;
        sz    = size();
    }

    auto op =
        getOp<BitMapWordT>(doclear ? Operation::eOpClear : Operation::eOpSet);

    std::lock_guard<std::mutex> lock(m_lock);
    for (auto i = 0U; i < words - 1; i++) {
        op(m_bit_words[i], other.m_bit_words[i]);

        sz -= bitsPerWord();
    }

    AUD_ASSERT(sz < bitsPerWord(), "Unfavorable condition");

    /* Last word must be dealt separately */
    auto [idx, pos] = getWordIdxPair(sz);
    auto data       = getNBitMask(pos) & other.m_bit_words[idx];

    op(m_bit_words[idx], data);
}

void
BitMap::set()
{
    doSetOrClear(false);
}

void
BitMap::set(Uint32 bitpos)
{
    AUD_ASSERT(isValidPos(bitpos), "Invalid Size");

    if (isValidPos(bitpos)) {
        std::lock_guard<std::mutex> lock(m_lock);

        auto [idx, pos] = getWordIdxPair(bitpos);
        m_bit_words[idx] |= 1 << pos;
    }
}

void
BitMap::set(BitMap const& other)
{
    doSetOrClear(other, false);
}

void
BitMap::clear()
{
    std::lock_guard<std::mutex> lock(m_lock);

    /* memset() in C++ :) */
    std::fill(m_bit_words.begin(), m_bit_words.end(), 0);
}

void
BitMap::clear(Uint32 bitpos)
{
    AUD_ASSERT(isValidPos(bitpos), "Invalid Size");

    if (isValidPos(bitpos)) {
        auto [idx, pos] = getWordIdxPair(bitpos);

        std::lock_guard<std::mutex> lock(m_lock);
        m_bit_words[idx] &= ~(1 << pos);
    }
}

void
BitMap::clear(BitMap const& other)
{
    doSetOrClear(other, true);
}

void
BitMap::flip()
{ /* FIXME: Add at a later stage */
}

void
BitMap::flip(Uint32 bitpos)
{
    /* FIXME: Add at a later stage */
}

void
BitMap::flip(BitMap const& other)
{
    /* FIXME: Add at a later stage */
}

bool
BitMap::isSet(Uint32 bitpos) const
{
    AUD_ASSERT(isValidPos(bitpos), "Invalid Size");

    if (bitpos <= m_size) {
        auto [idx, pos] = getWordIdxPair(bitpos);

        std::lock_guard<std::mutex> lock(m_lock);
        return m_bit_words[idx] & (1 << pos);
    }

    return false;
}

bool
BitMap::isClear(Uint32 bitpos) const
{
    return !isSet(bitpos);
}

bool
BitMap::isZero() const
{
    auto iszero = true;

    std::lock_guard<std::mutex> lock(m_lock);
    for (auto word : m_bit_words) {
        if (word != 0) {
            iszero = false;
            break;
        }
    }

    return iszero;
}

bool
BitMap::isFull() const
{
    return count() == size();
}

bool
BitMap::isSubset(BitMap const& other)
{
    AUD_ASSERT_NOT_IMPLEMENTED();
    return false;
}

/* Operators */
BitMap&
BitMap::operator|(BitMap const& other)
{
    doSetOrClear(other, true);

    return *this;
}

BitMap&
BitMap::operator&(BitMap const& other)
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return *this;
}

BitMap&
BitMap::operator^(BitMap const& other)
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return *this;
}

bool
BitMap::operator==(BitMap const& other) const
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return false;
}

BitMap&
BitMap::operator!()
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return *this;
}

/* Iterator trait */
BitMap&
BitMap::begin()
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return *this;
}

BitMap&
BitMap::end()
{
    /* FIXME: */
    AUD_ASSERT_NOT_IMPLEMENTED();
    return *this;
}

#if 0
BitMap&
operator=(BitMap&& src)
{
    if (this != &src) {
        std::lock_guard<std::mutex> lock(m_lock);
        std::move(m_bit_words, src.m_bit_words);
        m_size  = src.size();
        m_count = src.count();
    }

    return *this;
}
#endif

BitMap::BitMap(Uint32 size)
    : m_size{ size }
    , m_nwords{ size / bitsPerWord() + 1 }
    , m_bit_words{ std::vector<BitMapWordT>(m_nwords) }
    , m_lock{}
{
    AUD_ASSERT(size, "Invalid size for BitMap");
}

BitMap::~BitMap() {}

} // namespace Au
