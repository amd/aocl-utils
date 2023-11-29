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

#include "Au/Cpuid/Cpuid.hh"
#include "Au/Cpuid/X86Cpu.hh"
#include "Au/Defs.hh"

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/macros.h"

AUD_EXTERN_C_BEGIN

using namespace Au;

bool AUD_API_EXPORT
au_cpuid_is_amd(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };

    return cpu.isAMD();
}

au_error_t AUD_API_EXPORT
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size)
{
    /* FIXME:  */
    AUD_ASSERT(false, "FIXME:");
    return 0;
}

bool AUD_API_EXPORT
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num)
{
    /* FIXME */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

bool AUD_API_EXPORT
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num)
{
    /* FIXME: */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

bool AUD_API_EXPORT
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num)
{
    /* FIXME: */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

bool AUD_API_EXPORT
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num)
{
    /* FIXME: */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

bool AUD_API_EXPORT
au_cpuid_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag)
{
    /* FIXME: */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

bool AUD_API_EXPORT
au_cpuid_is_error(au_error_t err)
{
    /* FIXME: */
    AUD_ASSERT(false, "FIXME:");
    return false;
}

AUD_EXTERN_C_END
