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
#include "X86RawData.hh"

#include <array>
#include <list>
#include <tuple>

namespace Au {

// clang-format off
using QueryT = std::tuple<RequestT, ResponseT, EFlag>;
static const std::array<QueryT, *EFlag::MAX> CPUID_MAP = {{
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000001}, EFlag::sse3},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000002}, EFlag::pclmulqdq},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000004}, EFlag::dtes64},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000008}, EFlag::monitor},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000010}, EFlag::dscpl},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000020}, EFlag::vmx},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000040}, EFlag::smx},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000080}, EFlag::est},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000100}, EFlag::tm2},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000200}, EFlag::ssse3},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00000400}, EFlag::cid},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00001000}, EFlag::fma},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00002000}, EFlag::cx16},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00004000}, EFlag::xtpr},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00008000}, EFlag::pdcm},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00020000}, EFlag::pcid},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00040000}, EFlag::dca},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00080000}, EFlag::sse4_1},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00100000}, EFlag::sse4_2},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00200000}, EFlag::x2apic},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00400000}, EFlag::movbe},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x00800000}, EFlag::popcnt},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x01000000}, EFlag::tsc_deadline},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x02000000}, EFlag::aes},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x04000000}, EFlag::xsave},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x08000000}, EFlag::osxsave},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x10000000}, EFlag::avx},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x20000000}, EFlag::f16c},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x40000000}, EFlag::rdrand},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0x80000000}, EFlag::hypervisor},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000001}, EFlag::fpu},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000002}, EFlag::vme},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000004}, EFlag::de},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000008}, EFlag::pse},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000010}, EFlag::tsc},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000020}, EFlag::msr},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000040}, EFlag::pae},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000080}, EFlag::mce},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000100}, EFlag::cx8},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000200}, EFlag::apic},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000800}, EFlag::sep},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00001000}, EFlag::mtrr},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00002000}, EFlag::pge},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00004000}, EFlag::mca},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00008000}, EFlag::cmov},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00010000}, EFlag::pat},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00020000}, EFlag::pse36},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00040000}, EFlag::pn},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00080000}, EFlag::clflush},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00200000}, EFlag::ds},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00400000}, EFlag::acpi},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00800000}, EFlag::mmx},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x01000000}, EFlag::fxsr},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x02000000}, EFlag::sse},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x04000000}, EFlag::sse2},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x08000000}, EFlag::ss},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x10000000}, EFlag::ht},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x20000000}, EFlag::tm},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x40000000}, EFlag::ia64},
    {{.eax=0x00000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x80000000}, EFlag::pbe},

    {{.eax=0x00000006}, {.eax=0x00000004 }, EFlag::arat},

    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000001}, EFlag::fsgsbase},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000002}, EFlag::tsc_adjust},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000008}, EFlag::bmi1},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000010}, EFlag::hle},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000020}, EFlag::avx2},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000080}, EFlag::smep},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000100}, EFlag::bmi2},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000200}, EFlag::erms},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000400}, EFlag::invpcid},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00000800}, EFlag::rtm},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00004000}, EFlag::mpx},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00010000}, EFlag::avx512f},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00020000}, EFlag::avx512dq},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00040000}, EFlag::rdseed},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00080000}, EFlag::adx},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00100000}, EFlag::smap},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00200000}, EFlag::avx512ifma},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00400000}, EFlag::pcommit},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x00800000}, EFlag::clflushopt},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x01000000}, EFlag::clwb},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x04000000}, EFlag::avx512pf},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x08000000}, EFlag::avx512er},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x10000000}, EFlag::avx512cd},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x20000000}, EFlag::sha_ni},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x40000000}, EFlag::avx512bw},
    {{.eax=0x00000007}, {.eax=0, .ebx=0x80000000}, EFlag::avx512vl},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00000002}, EFlag::avx512vbmi},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00000004}, EFlag::umip},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00000008}, EFlag::pku},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00000010}, EFlag::ospke},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00004000}, EFlag::avx512_vpopcntdq},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00010000}, EFlag::la57},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0x00400000}, EFlag::rdpid},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000004}, EFlag::avx512_4vnniw},
    {{.eax=0x00000007}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000008}, EFlag::avx512_4fmaps},

    {{.eax=0x0000000d}, {.eax=0x00000001}, EFlag::xsaveopt},
    {{.eax=0x0000000d}, {.eax=0x00000002}, EFlag::xsavec},
    {{.eax=0x0000000d}, {.eax=0x00000004}, EFlag::xgetbv1},
    {{.eax=0x0000000d}, {.eax=0x00000008}, EFlag::xsaves},
#if 0
/* FIXME : do we need it  on host ?? */
    {{.eax=0x40000001}, {.eax=0x00000001}, EFlag::kvmclock},
    {{.eax=0x40000001}, {.eax=0x00000002}, EFlag::kvm_nopiodelay},
    {{.eax=0x40000001}, {.eax=0x00000004}, EFlag::kvm_mmu},
    {{.eax=0x40000001}, {.eax=0x00000008}, EFlag::kvmclock},
    {{.eax=0x40000001}, {.eax=0x00000010}, EFlag::kvm_asyncpf},
    {{.eax=0x40000001}, {.eax=0x00000020}, EFlag::kvm_steal_time},
    {{.eax=0x40000001}, {.eax=0x00000040}, EFlag::kvm_pv_eoi},
    {{.eax=0x40000001}, {.eax=0x00000080}, EFlag::kvm_pv_unhalt},
    {{.eax=0x40000001}, {.eax=0x01000000}, EFlag::kvmclock_stable_bit},
#endif

    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000001}, EFlag::lahf_lm},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000002}, EFlag::cmp_legacy},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000004}, EFlag::svm},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000008}, EFlag::extapic},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000010}, EFlag::cr8legacy},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000020}, EFlag::abm},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000040}, EFlag::sse4a},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000080}, EFlag::misalignsse},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000100}, EFlag::_3dnowprefetch},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000200}, EFlag::osvw},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000400}, EFlag::ibs},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00000800}, EFlag::xop},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00001000}, EFlag::skinit},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00002000}, EFlag::wdt},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00008000}, EFlag::lwp},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00010000}, EFlag::fma4},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00020000}, EFlag::tce},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00080000}, EFlag::nodeid_msr},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00200000}, EFlag::tbm},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00400000}, EFlag::topoext},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x00800000}, EFlag::perfctr_core},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0x01000000}, EFlag::perfctr_nb},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000800}, EFlag::syscall},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00100000}, EFlag::nxxd},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00400000}, EFlag::mmxext},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x02000000}, EFlag::fxsr_opt},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x04000000}, EFlag::pdpe1gb},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x08000000}, EFlag::rdtscp},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x20000000}, EFlag::lmi64},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x40000000}, EFlag::_3dnowext},
    {{.eax=0x80000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x80000000}, EFlag::_3dnow},

    {{.eax=0x80000007}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000100}, EFlag::invtsc},

    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000001}, EFlag::npt},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000002}, EFlag::lbrv},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000004}, EFlag::svm_lock},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000008}, EFlag::nrip_save},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000010}, EFlag::tsc_scale},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000020}, EFlag::vmcb_clean},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000040}, EFlag::flushbyasid},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000080}, EFlag::decodeassists},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000400}, EFlag::pause_filter},
    {{.eax=0x8000000A}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00001000}, EFlag::pfthreshold},

    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000004}, EFlag::xstore},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000008}, EFlag::xstore_en},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000040}, EFlag::xcrypt},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000080}, EFlag::xcrypt_en},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000100}, EFlag::ace2},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000200}, EFlag::ace2_en},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000400}, EFlag::phe},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00000800}, EFlag::phe_en},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00001000}, EFlag::pmm},
    {{.eax=0xC0000001}, {.eax=0, .ebx=0, .ecx=0, .edx=0x00002000}, EFlag::pmm_en},
}};
// clang-format on

void
X86Cpu::Impl::update()
{

    /* FIXME:
     * We need to make sure that
     * the core is switched to
     * correct cpunum, using
     * sched_setaffinity()
     */

    std::map<RequestT, ResponseT> rawCpuid;

    /* manufacturer details */
    auto resp             = at(RequestT{ 0x0000'0001, 0, 0, 0 });
    mVendorInfo.mMfg      = mCUtils->getMfgInfo(at(RequestT{ 0, 0, 0, 0 }));
    mVendorInfo.mFamily   = mCUtils->getFamily(resp.eax);
    mVendorInfo.mModel    = mCUtils->getModel(resp.eax);
    mVendorInfo.mStepping = mCUtils->getStepping(resp.eax);

    for (const auto& query : CPUID_MAP) {
        const auto& [req, expected, flg] = query;
        if (rawCpuid.find(req) == rawCpuid.end()) {
            rawCpuid.insert(std::make_pair(req, at(req)));
        }
        updateflag(flg, mCUtils->hasFlag(expected, rawCpuid[req]));
    }

    /*
     * Globally disable some
     * *_USABLE flags, so that
     * all ifunc's sees them
     */
    if (mVendorInfo.mMfg == EVendor::Amd
        && mVendorInfo.mFamily >= EFamily::Zen) {
        /* todo */
    }

#if defined(AU_BUILD_TYPE_DEVELOPER)
    /* We override any form  */
    // auto env_cpuid_str = AU_CPUID_OVERRIDE;
    // au_env_get(env_cpuid_str, sizeof(env_cpuid_str), vinfo);
#endif

    /* Update cache info */
    mCUtils->updateCacheView(mCacheView);
}

ResponseT
X86Cpu::Impl::at(RequestT& req) const
{
    return mCUtils->__raw_cpuid(req);
}

bool
X86Cpu::Impl::isIntel() const
{
    return mVendorInfo.mMfg == EVendor::Intel;
}

bool
X86Cpu::Impl::isAMD() const
{
    return mVendorInfo.mMfg == EVendor::Amd;
}

bool
X86Cpu::Impl::isX86_64v2() const
{
    static const std::vector<EFlag> featureArr{
        EFlag::cx16, EFlag::lahf_lm, EFlag::popcnt, EFlag::sse4_2, EFlag::ssse3
    };

    return isUsable(featureArr);
}

bool
X86Cpu::Impl::isX86_64v3() const
{
    static const std::vector<EFlag> featureArr{
        EFlag::avx, EFlag::avx2, EFlag::bmi1,  EFlag::bmi2, EFlag::f16c,
        EFlag::fma, EFlag::abm,  EFlag::movbe, EFlag::xsave
    };

    return isX86_64v2() && isUsable(featureArr);
}

bool
X86Cpu::Impl::isX86_64v4() const
{

    static const std::vector<EFlag> featureArr{ EFlag::avx512f,
                                                EFlag::avx512bw,
                                                EFlag::avx512cd,
                                                EFlag::avx512dq,
                                                EFlag::avx512vl };

    return isX86_64v3() && isUsable(featureArr);
}

bool
X86Cpu::Impl::hasFlag(EFlag const& eflag) const
{
    return mAvailableFlags.at(eflag) && mUsableFlags.at(eflag);
}

void
X86Cpu::Impl::setUsableFlag(EFlag const& eflag, bool res)
{
    mUsableFlags[eflag] = res;
}

void
X86Cpu::Impl::apply(RequestT& regs)
{
    switch (regs.eax) {
        case 0x0000'0001:
        case 0x0000'0007:
        case 0x0000'000C:
        case 0x4000'0001:
        case 0x8000'0001:
        case 0x8000'0007:
        case 0x8000'000A:
        case 0xC000'0001:
            break;

            /* FIXME: we silently fail here, we could also raise exceptions or
             * return Status().
             */
        default:
            return;
    }

    const auto* const found = std::ranges::find_if(
        begin(CPUID_MAP), end(CPUID_MAP), [&](const auto& input) {
            const auto& [req, expected, flg] = input;
            return req == regs;
        });

    if (found != end(CPUID_MAP)) {
        const auto& [_, __, flg] = *found; // NOLINT
        updateflag(flg, true);
    }

    /* FIXME: we silently fail here, we could also raise exceptions or
     * return Status().
     */
}

} // namespace Au
