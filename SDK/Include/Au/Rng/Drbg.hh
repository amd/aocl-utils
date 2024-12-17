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

#include <memory>
#include <vector>

#include "Au/Rng/Rng.hh"

namespace Au::Rng {
AUD_API_EXPORT class IDrbg : public IRng
{
  public:
    IDrbg()          = default;
    virtual ~IDrbg() = default;

    /**
     * @brief Initialize the DRBG
     * @param securityStrength The security strength of the DRBG
     * @param p_cPersonalizationString The personalization string
     * @return true if the DRBG was initialized successfully
     */
    virtual bool initialize(int                 securityStrength,
                            std::vector<Uint8>& p_cPersonalizationString) = 0;

    /**
     * @brief Reseed the DRBG
     * @param p_cAdditionalInput The additional input
     * @return true if the DRBG was reseeded successfully
     * @note This is useful for DRBGs that require reseeding
     */
    virtual Uint64 randomize(Uint8* data, Uint64 size) = 0;

    /**
     * @brief Generate random data
     * @param p_Output The output buffer
     * @param length The length of the output buffer
     * @param securityStrength The security strength of the DRBG
     * @param p_cAdditionalInput The additional input
     * @return The number of bytes generated
     */
    virtual Uint64 randomize(Uint8               p_Output[],
                             size_t              length,
                             int                 securityStrength,
                             std::vector<Uint8>& p_cAdditionalInput) = 0;

    /**
     * @brief Set the entropy source
     * @param entropyIn The entropy source
     * @return true if the entropy source was set successfully
     */
    virtual bool setRng(std::shared_ptr<Au::Rng::IRng> entropyIn) = 0;

    /**
     * @brief Set the nonce length
     * @param nonceLen The nonce length
     */
    virtual void setNonceLen(Uint64 nonceLen) = 0;

    /**
     * @brief Set the entropy length
     * @param entropyLen The entropy length
     */
    virtual void setEntropyLen(Uint64 entropyLen) = 0;

    /**
     * @brief Set the prediction resistance
     * @param pr The prediction resistance
     * @return true if the prediction resistance was set successfully
     */
    virtual bool setPredictionResistance(bool pr) = 0;
};

} // namespace Au::Rng
