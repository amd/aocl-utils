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

#include <array>
#include <assert.h>
#include <cstdint>
#include <map>

// clang-format off
/**
 * \namespace alci
 * \brief     Contains functions/members to retrieve CPU and Cache information
 *            of the system.
 */
// clang-format on
namespace alci {

#include "alci/enum.h"

/**
 * \brief       Extract specified bits from 32-bit value.
 *
 * Extracts length number of bits starting from bit position
 * start in 32-bit value.
 *
 * \param[in]   value  32-bit value.
 * \param[in]   start  Starting bit position.
 * \param[in]   length Number of bits to be extracted.
 *
 * \return      integer Extracted value.
 */
static inline uint32_t
extract32(uint32_t value, int start, int length)
{
    assert(start >= 0 && length > 0 && length <= 32 - start);

    return (value >> start) & (~0U >> (32 - length));
}

// clang-format off
/**---------------------------------------------------------------------------------------------------------------------------------
 * |                                                 Processor Version Information                                                 |
 * ---------------------------------------------------------------------------------------------------------------------------------
 * |                                                           EAX = 1                                                             |
 * ---------------------------------------------------------------------------------------------------------------------------------
 * |  31 30 29 28  |  27 26 25 24 23 22 21 20  |  19 18 17 16  |  15  14  | 13 12          |  11 10 9 8  |  7 6 5 4  |   3 2 1 0   |
 * ---------------------------------------------------------------------------------------------------------------------------------
 * |   Reserved    |   Extended Family ID      |  Ext Model ID | Reserved | Processor Type |  Family ID  |   Model   | Stepping ID |
 * ---------------------------------------------------------------------------------------------------------------------------------
 */
// clang-format on

/**
 * \brief       Get Family ID from given 32-bit input value.
 *
 * Family[7:0] = (ExtendedFamily[7:0] + {0000b,BaseFamily[3:0]})
 * where ExtendedFamily[7:0] = var[27:20], BaseFamily[3:0] = var[11, 8]
 *
 * \param[in]   var  32-bit value.
 *
 * \return      Returns Family ID value.
 */
static inline uint16_t
alc_cpuid_get_family(uint32_t var)
{
    return static_cast<uint16_t>(extract32(var, 20, 8) + extract32(var, 8, 4));
}

/**
 * \brief       Get Model number from given input value.
 *
 *  Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
 *  where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
 *
 * \param[in]   var  32-bit value.
 *
 * \return      Returns Model number value.
 */
static inline uint16_t
alc_cpuid_get_model(uint32_t var)
{
    return static_cast<uint16_t>(extract32(var, 16, 4) << 4
                                 | extract32(var, 4, 4));
}

/**
 * \brief       Get Stepping ID from given input value.
 *
 * Get Stepping ID from given input value.
 * Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}
 * where ExtendedModel[3:0] = var[29:16], BaseModel[3:0] = var[7, 4]
 * For now, model number is stepping. TODO: need to revisit for each family.
 *
 * \param[in]   var  32-bit value.
 *
 * \return      Returns Stepping ID value.
 */
static inline uint16_t
alc_cpuid_get_stepping(uint32_t var)
{
    return static_cast<uint16_t>(extract32(var, 20, 8) + extract32(var, 8, 4));
}

/**
 * \brief       Get single bit from given input value.
 *
 * \param[in]   value  32-bit value.
 * \param[in]   bit    Bit number.
 *
 * \return      Returns specific bit value.
 */
static inline uint32_t
cpuid_bit(uint32_t value, int bit)
{
    return extract32(value, bit, 1);
}

} // namespace alci

#include "alci/cxx/enum.hh"
