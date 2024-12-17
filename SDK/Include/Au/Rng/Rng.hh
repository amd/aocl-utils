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

#pragma once

#include "Au/Au.hh"
#include "Au/Types.hh"

namespace Au::Rng {

AUD_API_EXPORT class IRng
{
  public:
    IRng()          = default;
    virtual ~IRng() = default;

    /**
     * @brief Seed the RNG
     * @param seed The seed to use
     * @note This is useful for RNGs that require seeding before use
     *     (e.g
     *      Mersenne
     *      Twister
     *     )
     */
    virtual void seed(Uint64 seed) = 0;

    /**
     * @brief Check if the RNG has been seeded
     * @return true if the RNG has been seeded
     * @return false if the RNG has not been seeded
     * @note This is useful for RNGs that require seeding before use
     *       (e.g
     *        Mersenne
     *        Twister
     *       )
     *       For RNGs that do not require seeding, this should always return
     *       true
     */
    virtual bool isSeeded() const = 0;

    /**
     * @brief Check if the RNG needs to be reseeded
     * @return true if the RNG needs to be reseeded
     * @return false if the RNG does not need to be reseeded
     * @note This is useful for RNGs that require reseeding after a certain
     *       number of calls
     */
    virtual bool needReseed() const = 0;

#if 0 // Good to have
    /**
     * @brief Get the next random number
     * @return Uint64 The next random number
     */
    virtual Uint64 next() = 0;

    /**
     * @brief Get the next random number in the range [min, max]
     * @param min The minimum value
     * @param max The maximum value
     * @return Uint64 The next random number in the range [min, max]
     */
    virtual Uint64 next(Uint64 min, Uint64 max) = 0;
#endif

    /**
     * @brief Randomize a buffer of data
     * @param data The buffer to randomize
     * @param size The size of the buffer
     * @return Uint64 The number of bytes randomized
     * @note This is useful for RNGs that require randomizing data
     *       Make sure to check the return value to ensure that the
     *       entire buffer was randomized, otherwise reseeding may be
     *       needed.
     */
    virtual Uint64 randomize(Uint8* data, Uint64 size) = 0;

    /**
     * @brief Get the name of the RNG
     * @return std::string The name of the RNG
     */
    virtual std::string name() const = 0;
};

} // namespace Au::Rng
