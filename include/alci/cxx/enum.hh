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

#include <cstdint>
#include <iostream>

namespace alci {

/** \enum WritePolicy
 *  \brief Cache policy.
 */
enum class WritePolicy : uint32_t
{
    eWriteBack = 1, /**< Write back. */
    eWriteThrough   /**< Write through. */
};

/** \enum AllocatePolicy
 *  \brief Cache allocation type.
 */
enum class AllocatePolicy : uint32_t
{
    eReadAllocate = 1, /**< Read allocate. */
    eWriteAllocate     /**< Write allocate. */
};

/**
 * \enum  Vendor
 * \brief CPU vendors.
 */
enum class Vendor : uint32_t
{
    eIntel = 1, /**< Intel. */
    eAmd,       /**< AMD. */
    eOther      /**< Others. */
};

/**
 * \struct  VendorInfo
 * \brief   CPU core info.
 */
struct VendorInfo
{
    alci::Vendor m_mfg;      /**< CPU manufacturing vendor. */
    uint16_t     m_family;   /**< CPU family ID. */
    uint16_t     m_model;    /**< CPU model number. */
    uint16_t     m_stepping; /**< CPU stepping. */
};

/**
 * \enum  Uarch
 * \brief AMD CPU microarchitectures.
 */
enum class Uarch : uint32_t
{
    eZen = 1, /**< Zen. */
    eZen2,    /**< Zen 2. */
    eZen3,    /**< Zen 3. */
    eZen4,    /**< Zen 4. */
    eOther    /**< Others. */
};

/** \enum CacheType
 *  \brief Cache type.
 */
enum class CacheType : uint32_t
{
    eInstruction = 1, /**< Instruction Cache. */
    eData,            /**< Data Cache. */
    eUnified,         /**< Unified Cache. */
    eUnknown          /**< Unknown type. */
};

/** \enum CacheLevel
 *  \brief Cache level.
 */
enum class CacheLevel : uint32_t
{
    e_Level_1 = 1,  /**< L1 Cache. */
    e_Level_2,      /**< L2 Cache. */
    e_Level_3,      /**< L3 Cache. */
    e_Level_4,      /**< L4 Cache. */
    e_Level_5,      /**< L5 Cache. */
    e_Level_Unknown /**< Unknown Cache level. */
};

/** \enum CacheParam
 *  \brief Cache Info Parameters.
 */
enum class CacheParam : uint32_t
{
    eSize = 1,        /**< Cache Size. */
    eNumSets,         /**< Number of sets. */
    eNumWays,         /**< Number of ways. */
    eLineSize,        /**< Line size. */
    ePhysPartitions,  /**< Physical partitions. */
    eNumSharingCache, /**< Num of logical processors sharing cache. */
    eFullyAssoc,      /**< Fully associative cache. */
    eSelfInit,        /**< Self initializing Cache. */
    eInclusive,       /**< Cache Inclusive. */
    eUnknown          /**< Unknown Cache Param. */
};

/**
 * \brief   Overloading the ostream output (<<) operator with cache level.
 *
 * L1/L2/L3 etc cache levels will be returned.
 *
 * \param[in] os    Pointer to output stream.
 * \param[in] level Pointer to cache level.
 *
 * \return  Returns ostream output with cache level.
 */
std::ostream&
operator<<(std::ostream& os, const CacheLevel& level);

/**
 * \brief   Overloading the ostream output (<<) operator with cache type.
 *
 * Instruction/data/unified cache types will be returned.
 *
 * \param[in] os    Pointer to output stream.
 * \param[in] type  Pointer to cache type.
 *
 * \return  Returns ostream output with cache type.
 */
std::ostream&
operator<<(std::ostream& os, const CacheType& type);

/**
 * \brief   Overloading the ostream output (<<) operator with vendor name.
 *
 * \param[in] os  Pointer to output stream.
 * \param[in] av  Pointer to Vendor.
 *
 * \return  Returns ostream output with Vendor name.
 */
std::ostream&
operator<<(std::ostream& os, const Vendor& av);

/**
 * \brief   Overloading the ostream output operator (<<) with architecture name.
 *
 * \param[in] os  Pointer to output stream.
 * \param[in] av  Pointer to architecture name.
 *
 * \return  Returns ostream output with architecture name.
 */
std::ostream&
operator<<(std::ostream& os, const Uarch& av);

} // namespace alci
