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
#include "Types.hh"

#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>

namespace Au {

class BitMap
{
  public:
    /**
     * @brief  Returns the number of bits set
     *
     * @param
     *
     * @return A count of all the bits that are set in the bit mask
     */
    Uint32 count() const;

    /**
     * @brief  Size of the bitmask
     *         (total number of bits that can be represented)
     *
     * @return Total number of bits
     */
    Uint32 size() const;

    /**
     * @brief Set all bits in the bitmap
     *
     * @param
     *
     * @return none
     */
    void set();
    /**
     * @brief           Set a specific bit in the bitmap
     *
     * @details         Provided bit position should be less than size()
     *
     * @param[in] bitpos  Bit position that needs to be set
     *                    if the bit is already set, no action will be taken
     *                    if the bit is clear, it will be set
     * @return none
     */
    void set(Uint32 bitpos);

    /**
     * @brief       Set all the bits in the provided bitmap
     *
     * @detail      Can copy from another only if the size of source is less
     *              than or equal to that of current bitmap
     *
     * @param[in]       other   Bitmap which works as bits to set, no
     *                          restriction on the size of bits if the bitmap is
     *                          equal or less in size, the rest of of the bits
     *                          in 'this' are untouched, if the bitmap is larger
     *                          in size the rest of bits in 'other' are ignored
     *
     * @return      none
     */
    void set(BitMap const& other);

    /**
     * @brief  Clear all bits of a bitmask
     *
     * @param
     *
     * @return none
     */
    void clear();

    /**
     * @brief           Clear a specific bit in the bitmap
     *
     * @param[in] bitpos        Bit position of the bit that
     *                          will be cleared, if the bit is
     *                          already 0, no action is done
     *
     * @return none
     */
    void clear(Uint32 bitpos);

    /**
     * @brief           Clear a specific bit in the bitmap
     *
     * @detail          The other.size() should be less or equal
     *                  to that of this->size()
     *
     * @param[in]       other   Bitmap which works as bits to clear, no
     *                          restriction on the size of bits if the bitmap is
     *                          equal or less in size, the rest of of the bits
     *                          in 'this' are untouched, if the bitmap is larger
     *                          in size the rest of bits in 'other' are ignored
     * @return none
     */
    void clear(BitMap const& other);

    /**
     * @brief  Flip all bits of a bitmask
     *
     * @param
     *
     * @return none
     */
    void flip();

    /**
     * @brief           Clear a specific bit in the bitmap
     *
     * @param[in] bitpos        Bit position of the bit that
     *                          will be cleared, if the bit is
     *                          already 0, no action is done
     *
     * @return none
     */
    void flip(Uint32 bitpos);

    /**
     * @brief           Clear a specific bit in the bitmap
     *
     * @detail          The other.size() should be less or equal
     *                  to that of this->size()
     *
     * @param[in]       other   Bitmap which works as bits to flip, no
     *                          restriction on the size of bits if the bitmap is
     *                          equal or less in size, the rest of of the bits
     *                          in 'this' are untouched, if the bitmap is larger
     *                          in size the rest of bits in 'other' are ignored
     * @return none
     */
    void flip(BitMap const& other);

    /**
     * @brief   Check if this bitmap is all zeroes
     */
    bool isZero() const;

    /**
     * @brief   Check if this bitmap is all ones
     */
    bool isFull() const;

    /**
     * @brief   Check if 'other' is a subset of 'this'
     */
    bool isSubset(BitMap const& other);

  public: /* Operators */
    BitMap& operator|(BitMap const& other);
    BitMap& operator&(BitMap const& other);
    BitMap& operator^(BitMap const& other);
    BitMap& operator<<(BitMap const& other);
    BitMap& operator>>(BitMap const& other);
    BitMap& operator==(BitMap const& other);
    BitMap& operator!();

  public: /* Iterator trait */
    BitMap& begin();
    BitMap& end();

  public:
    explicit BitMap(Uint32 size);
    ~BitMap();

  private:
    using BitMapWordT = Uint64;

    template<typename T = BitMapWordT, class = enableIf<std::is_integral<T>>>
    constexpr int bits_per_word()
    {
        return std::numeric_limits<T>::digits;
    }

    template<typename T = BitMapWordT, class = enableIf<std::is_integral<T>>>
    constexpr bool all_set_mask(T a)
    {
        return a == std::numeric_limits<T>::max();
    }

    mutable std::mutex m_lock;
    Uint32             m_size;
    Uint32             m_nwords;
    /* TODO: replace eventually by Array */
    std::vector<BitMapWordT> m_bit_words;
};

} // namespace Au
