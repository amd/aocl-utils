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

#include "Au/Rng/HardwareRng.hh"

#include <immintrin.h>

namespace Au::Rng {

#define ATTRIBUTE_RAND __attribute__((__target__("rdrnd")))
//
// in C++20 Use
// std::is_trivial<T>::value && std::is_standard_layout<T>::value
// instead of std::is_pod

template<typename T,
         size_t W,
         typename = typename std::enable_if<sizeof(T) == W>::type>
bool
read_rdrand(T* ptr) __attribute__((__target__("rdrnd")));

/**
 * Read random bytes from hardware Rng on x86
 *
 * \param        success         Updates status in this variable
 *                               received by reference
 * \param        T               Should be an simple-integer type
 */
template<typename T = Uint16>
bool ATTRIBUTE_RAND
read_rdrand(T* ptr)
{
    int rc = 0;
    T   result;

    if constexpr (sizeof(T) == 1) {
        // Uint8
        Uint16 temp_result;
        rc     = _rdrand16_step(&temp_result);
        result = static_cast<Uint8>(temp_result & 0xFF);
    } else {
        // Uint16
        rc = _rdrand16_step(&result);
    }

    *ptr = result;

    return (1 == rc);
}

HardwareRng::HardwareRng() {}

Uint64
HardwareRng::randomize(Uint8 output[], size_t length)
{
    const int stride        = 2;
    size_t    new_length    = length / stride;
    Uint16*   ptr           = reinterpret_cast<Uint16*>(output);
    size_t    bytes_written = 0;

    if (reinterpret_cast<uintptr_t>(output) % alignof(Uint16) == 0) {
        // Data is 16-bit aligned
        while (new_length--) {
            bool is_success = read_rdrand<Uint16>(ptr);
            if (!is_success) {
                return bytes_written;
            }
            ptr++;
            bytes_written += stride;
        }
    } else {
        // Data is 8-bit aligned, downgrade stride to 1
        new_length      = length;
        Uint8* byte_ptr = reinterpret_cast<Uint8*>(output);
        while (new_length--) {
            bool is_success = read_rdrand<Uint8>(byte_ptr);
            if (!is_success) {
                return bytes_written;
            }
            byte_ptr++;
            bytes_written += 1;
        }
    }

    // Handle the case where length is odd
    if (length % stride != 0) {
        Uint8* byte_ptr   = reinterpret_cast<Uint8*>(ptr);
        bool   is_success = read_rdrand<Uint8>(byte_ptr);
        if (!is_success) {
            // No Entropy
            return bytes_written;
        }
        bytes_written += 1;
    }

    return bytes_written;
}

bool
HardwareRng::isSeeded() const
{
    return true;
}

bool
HardwareRng::needReseed() const
{
    return false;
}

void
HardwareRng::seed(Uint64 seed)
{
}
} // namespace Au::Rng
