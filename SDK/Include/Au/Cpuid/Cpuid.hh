/*
 * Copyright (C) 2023-2024, Advanced Micro Devices. All rights reserved.
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
#include "Au/Interface/Cpuid/ICpu.hh"

#include <iostream>

namespace Au {

#define AU_CPUID_OVERRIDE "AU_CPUID_OVERRIDE"

/**
 * @brief placeholder for ICpu, to return 'something'
 * */
class CpuInfo : public ICpu
{
  public:
    explicit CpuInfo(CpuNumT idx);

    virtual ~CpuInfo() {}

    /**
     * @brief Interface to get Freqency scaling governer
     *
     * @return EGoverner type
     */
    virtual EGoverner getGoverner() const override = 0;

  public:
    /* FIXME: All the virtual from ICpu */
    virtual String getFreq() const override { return "Unknown-Freq"; }

    virtual String getName() const override { return "Unknown-Name"; }

    virtual String getModel() const override { return "Unknown-Model"; }

    virtual CpuNumT getLogicalIdx() const override { return 0; }

    virtual CpuNumT getPhysicalIdx() const override { return 0; }

    virtual Uint32 getNumCores() const override { return 0; }

  protected:
    void setLogicalIdx(CpuNumT physicalId) { m_physical_core_num = physicalId; }

    void setPhysicalIdx(CpuNumT logicalId) { m_logical_core_num = logicalId; }

  private:
    CpuNumT m_logical_core_num;
    CpuNumT m_physical_core_num;
};

} // namespace Au
