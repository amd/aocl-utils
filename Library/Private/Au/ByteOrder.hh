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

/**
 * https://github.com/TileDB-Inc/TileDB/blob/dev/tiledb/sm/misc/endian.h
 *
 *
 */

#include "Au/Au.hh"
#include "Au/Types.hh"

namespace Au {

template<typename T>
class ByteOrder final
{
  private:
    enum class Layout
    {
        eHostEndian,   // Endianness of Host running this code
        eBigEndian,    // force Big Endian
        eLittleEndian, // force Little Endian
    };

  public:
    typedef Layout::eHostEndian eHostEndian;
    using eBigEndian = Layout::eBigEndian;

  public:
    /**
     * @brief
     *
     * @param loc
     * @param lat
     */
    void swapBytes(T& loc, Layout lat = eHostEndian) const;
    /**
     * @brief
     *
     * @param loc
     * @param lat
     * @return T
     */
    T swapBytes(T const& loc, Layout lat = eHostEndian) const;

    /**
     * @brief
     *
     * @param loc
     */
    void toBigEndian(T& loc);
    /**
     * @brief
     *
     * @param loc
     * @return T
     */
    T toBigEndian(T const& loc);

    /**
     * @brief
     *
     * @param loc
     */
    void toLittleEndian(T& loc);
    /**
     * @brief
     *
     * @param loc
     * @return T
     */
    T toLittleEndian(T const& loc);

    /**
     * @brief
     *
     * @param loc
     */
    void toHostEndian(T& loc);
    T    toHostEndian(T const& loc);

    /**
     * @brief
     *
     */
    bool isHostBigEndian() const;

    bool isHostLittleEndian() const;

    ByteOrder::Layout getHostEndian() const;

  private:
    void swap4(void* memPtr)
    {
        auto p = std::static_cast<std::byte*> memPtr;
        std::swap(p[0], p[3]);
        std::swap(p[1], p[2]);
    }

    void swap8(void* memPtr)
    {
        auto p = std::static_cast<std::byte*> memPtr;
        std::swap(p[0], p[7]);
        std::swap(p[1], p[6]);
        std::swap(p[2], p[5]);
        std::swap(p[3], p[4]);
    }
};

template<>
inline void
ByteOrder<Uint64>::swapBytes(Uint64 T& loc, Layout lt) const
{
}

template<>
inline Uint64
ByteOrder<Uint64>::swapBytes(Uint64 const& val, Layout lt) const
{
    Uint64 newval = val;
    swap8(&newval);

    return newval;
}

} // namespace Au
