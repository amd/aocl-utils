/*
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
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

#include "alci/cxx/alci.hh"

#include <array>
#include <cstdint>
#include <map>

namespace alci {

#include "alci/enum.h"

/**< Contains max cpu id value supported. */
const uint32_t kMaxCpuId = 32;
/**< Access extended id till 0x8000.001D */
const uint32_t kMaxCpuExtendedId = 256;

extern struct _cpuid_values cpuid_values[ALC_CPUID_MAX];

extern struct _cpuid_flag cpuid_flags[ALC_CPUID_FLAG_MAX];

/**
 * \struct  CpuIdRawData
 * \brief   Struct to query and hold the available CPUID results.
 */
struct CpuIdRawData
{
    /**
     * \brief   Updates CPU vendor info details.
     *
     * Initialize CPU Vendor info with Vendor name, manufacturing info and
     * all CPUID results with different eax, ecx register values.
     * It gives CPU Family, Model & Stepping.
     *
     */
    CpuIdRawData();

    /**
     *  The output of cpuid is always in '4' registers:
     *  eax, ebx, ecx, and edx namely.
     */
    typedef std::array<uint32_t, 4> regs;
    regs m_cpuid[kMaxCpuId]; /**< Array with all the available CPUID results. */

    /**< CpuInfo to store Vendor Information. */
    VendorInfo m_vendor_info;

    /**
     * \brief  Update Raw Data by re-reading the registers
     *
     * \return None
     */
    void update();

    /**
     * \brief       Convert given cpuid to offset by setting top most bit.
     *
     * \param[in]   eax   EAX register value.
     *
     * \return      Returns converted offset value.
     */
    constexpr uint32_t toOffset(uint32_t eax);
    /**
     * \brief       Convert offset to cpuid register value.
     *
     * \param[in]   offset   Offset value.
     *
     * \return      Returns converted cpuid value.
     */
    constexpr uint32_t toExtCpuId(uint32_t offset);

    /* TODO: add support for Ext_cpuid
       regs m_ext_cpuid[kMaxCpuExtendedId];
    */

    /**
     * \brief       Get CPUID output based on eax register value as input.
     *
     * \param[in]   eax   EAX register value.
     *
     * \return      Returns register values after performing cpuid.
     */
    regs at(uint32_t eax);

    /**
     * \brief       Get CPUID output based on eax, ecx register values as input.
     *
     * \param[in]   eax   EAX register value.
     * \param[in]   ecx   ECX register value.
     *
     * \return      Returns register values after performing cpuid.
     */
    regs at(uint32_t eax, uint32_t ecx);
};

} // namespace alci