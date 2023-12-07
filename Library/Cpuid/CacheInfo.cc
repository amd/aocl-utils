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

#include "Au/Cpuid/CacheInfo.hh"
#include "Au/Assert.hh"
#include "Au/Misc.hh" /* for enum->int */

namespace Au {

CacheInfo::CacheInfo(CacheLevel level, CacheType type)
    : m_level{ level }
    , m_type{ type }
{
    AUD_ASSERT(*level > 0, "Level should be > 0");
    AUD_ASSERT(*type > 0, "Type should be > 0");
}

void
CacheInfo::setSize(Uint64 size)
{
    AUD_ASSERT(size != 0, "Size is 0");
    m_size = size;
}

void
CacheInfo::setWay(Uint64 way_size)
{
    AUD_ASSERT(way_size != 0, "Way is 0");
    m_way = way_size;
}

void
CacheInfo::setLane(Uint64 lane_size)
{
    AUD_ASSERT(lane_size != 0, "Lane is 0");
    m_lane = lane_size;
}

void
CacheInfo::setSets(Uint64 sets)
{
    AUD_ASSERT(sets != 0, "Set is 0");
    m_set = sets;
}

} // namespace Au
