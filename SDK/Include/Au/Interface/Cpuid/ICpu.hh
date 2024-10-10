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

namespace Au {

enum class EGoverner
{
    ePerformance,
    ePowersave,
    eUserspace,
    eOndemand,
    eUnknown
};
std::ostream&
operator<<(std::ostream& os, const EGoverner& gov);

#if defined(cpu_num_t)
typedef cpu_num_t CpuNumT;
#else
typedef Uint32 CpuNumT;
#endif

/**
 * Sample information that can be gathered
 *
 * Package Info:
 *       Sockets : 02
 *       Cores   : 10 (per socket)
 *       Threads : 04 (per core)
 *
 * CPU Info:
 *       Vendor      : AMD
 *       Name        : Ryzen 7 PRO 2700U w/ Radeon Vega Mobile Gfx
 *       Family      : 0x17 (Zen/Zen+)
 *       Frequency   : 1.8 GHz
 *       Governer    : Performance
 *       Model       : 12   (Zen+)
 *       Stepping    : 0
 *         ID        : 0:2:3 (Socket:Core:Thread)
 *
 * Cache Info:
 *       L1 Cache  (D)  :
 *       L1 Cache  (I)  : 24 KB (2-way set-associative)
 *       L2 Cache  (U)  : 01 MB (fully associative)
 *       L3 Cache  (U)  : 32 MB (fully associative)
 */

class AUD_API_EXPORT ICpu
{
  protected:
    virtual ~ICpu(){};

  public:
    /**
     * @brief Interface to get frequency
     *
     * @return String containing frequency of processor
     */
    virtual String getFreq() const = 0;

    /**
     * @brief Interface to get Name of processor
     *
     * @return Full name of the processor
     */
    virtual String getName() const = 0;

    /**
     * @brief Interface to get Model of processor
     *
     * @return String containing frequency of processor
     */
    virtual String getModel() const = 0;

    /**
     * @brief Interface to get numerical index, as seen by the Operating system
     *
     * @return Numerical identifier of given processor
     */
    virtual CpuNumT getLogicalIdx() const = 0;

    /**
     * @brief Interface to get numerical index, as assigned by Firmware/BIOS
     *
     * @return Numerical identifier as assigned by BIOS
     */
    virtual CpuNumT getPhysicalIdx() const = 0;

    /**
     * @brief Interface to get number of cores in this processor.
     *
     * @return Integer specifying number of cores.
     */
    virtual Uint32 getNumCores() const = 0;

    /**
     * @brief Interface to get Freqency scaling governer
     *
     * @return EGoverner type
     */
    virtual EGoverner getGoverner() const = 0;
};

} // namespace Au
