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

#include "Au/Cpuid/Cpuid.hh"
#include "Au/Assert.hh"
#include "Au/Cpuid/X86Cpu.hh"

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/macros.h"

AUD_EXTERN_C_BEGIN

using namespace Au;

AUD_API_EXPORT
bool
au_cpuid_is_amd(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };

    return cpu.isAMD();
}
AUD_API_EXPORT
bool
alci_is_amd(au_cpu_num_t cpu_num)
{
    return au_cpuid_is_amd(cpu_num);
}

AUD_API_EXPORT
void
au_cpuid_get_vendor(au_cpu_num_t cpu_num, char* vend_info, size_t size)
{
    X86Cpu             cpu{ cpu_num };
    VendorInfo         v_info = cpu.getVendorInfo();
    std::ostringstream ss;
    ss << static_cast<Uint32>(v_info.m_mfg) << "\n"
       << static_cast<Uint32>(v_info.m_family) << "\n"
       << v_info.m_model << "\n"
       << v_info.m_stepping << "\n"
       << static_cast<Uint32>(v_info.m_uarch) << "\n";

    AUD_ASSERT(size >= ss.str().size(), "Buffer too small");
#if defined(AU_TARGET_OS_IS_WINDOWS)
    strncpy(vend_info, (ss.str()).c_str(), fmin(ss.str().size(), size));
#else
    strncpy(vend_info, (ss.str()).c_str(), std::min(ss.str().size(), size));
#endif
}

AUD_API_EXPORT
au_error_t
alci_cpu_get_vendor(au_cpu_num_t cpu_num, char* arr, size_t size)
{
    au_cpuid_get_vendor(cpu_num, arr, size);
    return 1;
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::Zen);
}

AUD_API_EXPORT
bool
alci_arch_is_zen(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen(cpu_num);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zenplus(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::ZenPlus);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::Zen2);
}

AUD_API_EXPORT
bool
alci_arch_is_zen2(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen2(cpu_num);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::Zen3);
}

AUD_API_EXPORT
bool
alci_arch_is_zen3(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen3(cpu_num);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::Zen4);
}

AUD_API_EXPORT
bool
alci_arch_is_zen4(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen4(cpu_num);
}

AUD_API_EXPORT
bool
alci_arch_is_zen5(au_cpu_num_t cpu_num)
{
    return au_cpuid_arch_is_zen5(cpu_num);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isUarch(EUarch::Zen5);
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_x86_64v2(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isX86_64v2();
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_x86_64v3(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isX86_64v3();
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_x86_64v4(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isX86_64v4();
}

AUD_API_EXPORT
bool
au_cpuid_arch_is_zen_family(au_cpu_num_t cpu_num)
{
    X86Cpu cpu{ cpu_num };
    return cpu.isZenFamily();
}

AUD_API_EXPORT
bool*
au_cpuid_has_flag(au_cpu_num_t      cpu_num,
                  const char* const flag_array[],
                  int               count)
{
    AUD_ASSERT(count > 0, "No flags to check");
    if (count == 0)
        return nullptr;

    std::stringstream        ss;
    std::vector<std::string> flag_names(flag_array, flag_array + count);
    ss << flag_names;
    AUD_ASSERT(flag_names.size() > 1, "The flags list is empty");
    if (flag_names.size() == 1)
        return nullptr;

    X86Cpu cpu{ cpu_num };
    String token;
    int    index = 0;
    // Use malloc to allocate memory, as it is used in C API. and will be freed
    // using free in a cprogram.
    bool* result = reinterpret_cast<bool*>(malloc(count * sizeof(bool)));
    AUD_ASSERT(result, "Memory allocation failed");
    if (!result)
        return nullptr;

    while (std::getline(ss, token, ':')) {
        au_cpu_flag_t flag = stoi(token);
        AUD_ASSERT(flag > static_cast<Uint32>(ECpuidFlag::Min)
                       && flag < static_cast<Uint32>(ECpuidFlag::Max),
                   "Flag not supported");
        if (flag < static_cast<Uint32>(ECpuidFlag::Min)
            || flag > static_cast<Uint32>(ECpuidFlag::Max))
            continue;
        auto cpuid_flag = static_cast<ECpuidFlag>(flag);
        result[index++] = cpu.hasFlag(cpuid_flag);
    }
    return result;
}

AUD_API_EXPORT
bool
au_cpuid_has_flags(au_cpu_num_t      cpu_num,
                   const char* const flag_array[],
                   int               count)
{
    AUD_ASSERT(count > 0, "No flags to check");
    if (count == 0)
        return false;

    std::stringstream        ss;
    std::vector<std::string> flag_names(flag_array, flag_array + count);
    ss << flag_names;
    AUD_ASSERT(flag_names.size() > 0, "The flags list is empty");
    if (flag_names.size() == 0)
        return false;

    X86Cpu cpu{ cpu_num };
    String token;
    bool   result = 1;

    while (std::getline(ss, token, ':')) {
        au_cpu_flag_t flag = stoi(token);
        AUD_ASSERT(flag > static_cast<Uint32>(ECpuidFlag::Min)
                       && flag < static_cast<Uint32>(ECpuidFlag::Max),
                   "Flag not supported");
        if (flag < static_cast<Uint32>(ECpuidFlag::Min)
            || flag > static_cast<Uint32>(ECpuidFlag::Max))
            continue;
        auto cpuid_flag = static_cast<ECpuidFlag>(flag);
        result &= cpu.hasFlag(cpuid_flag);
    }
    return result;
}

AUD_API_EXPORT
bool
alci_cpu_has_flag(au_cpu_num_t cpu_num, au_cpu_flag_t flag)
{
    X86Cpu cpu{ cpu_num };
    return cpu.hasFlag(static_cast<ECpuidFlag>(flag));
}

AUD_API_EXPORT
bool
au_cpuid_is_error(au_error_t err)
{
    AUD_ASSERT(err >= 0, "Invalid error code"); // Never be negative.
    if (static_cast<int32_t>(err))
        return true;
    return false;
}
AUD_EXTERN_C_END
