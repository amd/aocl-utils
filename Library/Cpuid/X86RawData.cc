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
static const std::array<QueryT, *EFlag::Max> CPUID_MAP = {{
    /* feature identifiers */
    {{0x00000001}, {0, 0, 0x00000001}, EFlag::sse3},
    {{0x00000001}, {0, 0, 0x00000002}, EFlag::pclmulqdq},
    {{0x00000001}, {0, 0, 0x00000004}, EFlag::dtes64},
    {{0x00000001}, {0, 0, 0x00000008}, EFlag::monitor},
    {{0x00000001}, {0, 0, 0x00000010}, EFlag::dscpl},
    {{0x00000001}, {0, 0, 0x00000020}, EFlag::vmx},
    {{0x00000001}, {0, 0, 0x00000040}, EFlag::smx},
    {{0x00000001}, {0, 0, 0x00000080}, EFlag::est},
    {{0x00000001}, {0, 0, 0x00000100}, EFlag::tm2},
    {{0x00000001}, {0, 0, 0x00000200}, EFlag::ssse3},
    {{0x00000001}, {0, 0, 0x00000400}, EFlag::cid},
    {{0x00000001}, {0, 0, 0x00001000}, EFlag::fma},
    {{0x00000001}, {0, 0, 0x00002000}, EFlag::cx16},
    {{0x00000001}, {0, 0, 0x00004000}, EFlag::xtpr},
    {{0x00000001}, {0, 0, 0x00008000}, EFlag::pdcm},
    {{0x00000001}, {0, 0, 0x00020000}, EFlag::pcid},
    {{0x00000001}, {0, 0, 0x00040000}, EFlag::dca},
    {{0x00000001}, {0, 0, 0x00080000}, EFlag::sse4_1},
    {{0x00000001}, {0, 0, 0x00100000}, EFlag::sse4_2},
    {{0x00000001}, {0, 0, 0x00200000}, EFlag::x2apic},
    {{0x00000001}, {0, 0, 0x00400000}, EFlag::movbe},
    {{0x00000001}, {0, 0, 0x00800000}, EFlag::popcnt},
    {{0x00000001}, {0, 0, 0x01000000}, EFlag::tsc_deadline},
    {{0x00000001}, {0, 0, 0x02000000}, EFlag::aes},
    {{0x00000001}, {0, 0, 0x04000000}, EFlag::xsave},
    {{0x00000001}, {0, 0, 0x08000000}, EFlag::osxsave},
    {{0x00000001}, {0, 0, 0x10000000}, EFlag::avx},
    {{0x00000001}, {0, 0, 0x20000000}, EFlag::f16c},
    {{0x00000001}, {0, 0, 0x40000000}, EFlag::rdrand},
    {{0x00000001}, {0, 0, 0x80000000}, EFlag::hypervisor},
    {{0x00000001}, {0, 0, 0, 0x00000001}, EFlag::fpu},
    {{0x00000001}, {0, 0, 0, 0x00000002}, EFlag::vme},
    {{0x00000001}, {0, 0, 0, 0x00000004}, EFlag::de},
    {{0x00000001}, {0, 0, 0, 0x00000008}, EFlag::pse},
    {{0x00000001}, {0, 0, 0, 0x00000010}, EFlag::tsc},
    {{0x00000001}, {0, 0, 0, 0x00000020}, EFlag::msr},
    {{0x00000001}, {0, 0, 0, 0x00000040}, EFlag::pae},
    {{0x00000001}, {0, 0, 0, 0x00000080}, EFlag::mce},
    {{0x00000001}, {0, 0, 0, 0x00000100}, EFlag::cx8},
    {{0x00000001}, {0, 0, 0, 0x00000200}, EFlag::apic},
    {{0x00000001}, {0, 0, 0, 0x00000800}, EFlag::sep},
    {{0x00000001}, {0, 0, 0, 0x00001000}, EFlag::mtrr},
    {{0x00000001}, {0, 0, 0, 0x00002000}, EFlag::pge},
    {{0x00000001}, {0, 0, 0, 0x00004000}, EFlag::mca},
    {{0x00000001}, {0, 0, 0, 0x00008000}, EFlag::cmov},
    {{0x00000001}, {0, 0, 0, 0x00010000}, EFlag::pat},
    {{0x00000001}, {0, 0, 0, 0x00020000}, EFlag::pse36},
    {{0x00000001}, {0, 0, 0, 0x00040000}, EFlag::pn},
    {{0x00000001}, {0, 0, 0, 0x00080000}, EFlag::clflush},
    {{0x00000001}, {0, 0, 0, 0x00200000}, EFlag::ds},
    {{0x00000001}, {0, 0, 0, 0x00400000}, EFlag::acpi},
    {{0x00000001}, {0, 0, 0, 0x00800000}, EFlag::mmx},
    {{0x00000001}, {0, 0, 0, 0x01000000}, EFlag::fxsr},
    {{0x00000001}, {0, 0, 0, 0x02000000}, EFlag::sse},
    {{0x00000001}, {0, 0, 0, 0x04000000}, EFlag::sse2},
    {{0x00000001}, {0, 0, 0, 0x08000000}, EFlag::ss},
    {{0x00000001}, {0, 0, 0, 0x10000000}, EFlag::ht},
    {{0x00000001}, {0, 0, 0, 0x20000000}, EFlag::tm},
    {{0x00000001}, {0, 0, 0, 0x40000000}, EFlag::ia64},
    {{0x00000001}, {0, 0, 0, 0x80000000}, EFlag::pbe},

    {{0x00000006}, {0x00000004 }, EFlag::arat},
    /* Structured extended feature identifiers */
    {{0x00000007}, {0, 0x00000001}, EFlag::fsgsbase},
    {{0x00000007}, {0, 0x00000002}, EFlag::tsc_adjust},
    {{0x00000007}, {0, 0x00000008}, EFlag::bmi1},
    {{0x00000007}, {0, 0x00000010}, EFlag::hle},
    {{0x00000007}, {0, 0x00000020}, EFlag::avx2},
    {{0x00000007}, {0, 0x00000080}, EFlag::smep},
    {{0x00000007}, {0, 0x00000100}, EFlag::bmi2},
    {{0x00000007}, {0, 0x00000200}, EFlag::erms},
    {{0x00000007}, {0, 0x00000400}, EFlag::invpcid},
    {{0x00000007}, {0, 0x00000800}, EFlag::rtm},
    {{0x00000007}, {0, 0x00004000}, EFlag::mpx},
    {{0x00000007}, {0, 0x00010000}, EFlag::avx512f},
    {{0x00000007}, {0, 0x00020000}, EFlag::avx512dq},
    {{0x00000007}, {0, 0x00040000}, EFlag::rdseed},
    {{0x00000007}, {0, 0x00080000}, EFlag::adx},
    {{0x00000007}, {0, 0x00100000}, EFlag::smap},
    {{0x00000007}, {0, 0x00200000}, EFlag::avx512ifma},
    {{0x00000007}, {0, 0x00400000}, EFlag::pcommit},
    {{0x00000007}, {0, 0x00800000}, EFlag::clflushopt},
    {{0x00000007}, {0, 0x01000000}, EFlag::clwb},
    {{0x00000007}, {0, 0x04000000}, EFlag::avx512pf},
    {{0x00000007}, {0, 0x08000000}, EFlag::avx512er},
    {{0x00000007}, {0, 0x10000000}, EFlag::avx512cd},
    {{0x00000007}, {0, 0x20000000}, EFlag::sha_ni},
    {{0x00000007}, {0, 0x40000000}, EFlag::avx512bw},
    {{0x00000007}, {0, 0x80000000}, EFlag::avx512vl},
    {{0x00000007}, {0, 0, 0x00000002}, EFlag::avx512vbmi},
    {{0x00000007}, {0, 0, 0x00000004}, EFlag::umip},
    {{0x00000007}, {0, 0, 0x00000008}, EFlag::pku},
    {{0x00000007}, {0, 0, 0x00000010}, EFlag::ospke},
    {{0x00000007}, {0, 0, 0x00000040}, EFlag::avx512vbmi2},
    {{0x00000007}, {0, 0, 0x00000200}, EFlag::vaes},
    {{0x00000007}, {0, 0, 0x00000400}, EFlag::vpclmulqdq},
    {{0x00000007}, {0, 0, 0x00000800}, EFlag::avx512_vnni},
    {{0x00000007}, {0, 0, 0x00001000}, EFlag::avx512_bitalg},
    {{0x00000007}, {0, 0, 0x00004000}, EFlag::avx512_vpopcntdq},
    {{0x00000007}, {0, 0, 0x00010000}, EFlag::la57},
    {{0x00000007}, {0, 0, 0x00400000}, EFlag::rdpid},
    {{0x00000007}, {0, 0, 0x08000000}, EFlag::movdiri},
    {{0x00000007}, {0, 0, 0x10000000}, EFlag::movdir64b},
    {{0x00000007}, {0, 0, 0, 0x00000004}, EFlag::avx512_4vnniw},
    {{0x00000007}, {0, 0, 0, 0x00000008}, EFlag::avx512_4fmaps},
    {{0x00000007}, {0, 0, 0, 0x00000100}, EFlag::avx512_vpintersect},
/* processor extended state enumeration */
    {{0x0000000d}, {0x00000001}, EFlag::xsaveopt},
    {{0x0000000d}, {0x00000002}, EFlag::xsavec},
    {{0x0000000d}, {0x00000004}, EFlag::xgetbv1},
    {{0x0000000d}, {0x00000008}, EFlag::xsaves},
#if 0
/* FIXME : do we need it  on host ?? */
    {{0x40000001}, {.eax = 0x00000001}, EFlag::kvmclock},
    {{0x40000001}, {.eax = 0x00000002}, EFlag::kvm_nopiodelay},
    {{0x40000001}, {.eax = 0x00000004}, EFlag::kvm_mmu},
    {{0x40000001}, {.eax = 0x00000008}, EFlag::kvmclock},
    {{0x40000001}, {.eax = 0x00000010}, EFlag::kvm_asyncpf},
    {{0x40000001}, {.eax = 0x00000020}, EFlag::kvm_steal_time},
    {{0x40000001}, {.eax = 0x00000040}, EFlag::kvm_pv_eoi},
    {{0x40000001}, {.eax = 0x00000080}, EFlag::kvm_pv_unhalt},
    {{0x40000001}, {.eax = 0x01000000}, EFlag::kvmclock_stable_bit},
#endif
    /* feature identifiers */
    {{0x80000001}, {0, 0, 0x00000001}, EFlag::lahf_lm},
    {{0x80000001}, {0, 0, 0x00000002}, EFlag::cmp_legacy},
    {{0x80000001}, {0, 0, 0x00000004}, EFlag::svm},
    {{0x80000001}, {0, 0, 0x00000008}, EFlag::extapic},
    {{0x80000001}, {0, 0, 0x00000010}, EFlag::cr8legacy},
    {{0x80000001}, {0, 0, 0x00000020}, EFlag::abm},
    {{0x80000001}, {0, 0, 0x00000040}, EFlag::sse4a},
    {{0x80000001}, {0, 0, 0x00000080}, EFlag::misalignsse},
    {{0x80000001}, {0, 0, 0x00000100}, EFlag::_3dnowprefetch},
    {{0x80000001}, {0, 0, 0x00000200}, EFlag::osvw},
    {{0x80000001}, {0, 0, 0x00000400}, EFlag::ibs},
    {{0x80000001}, {0, 0, 0x00000800}, EFlag::xop},
    {{0x80000001}, {0, 0, 0x00001000}, EFlag::skinit},
    {{0x80000001}, {0, 0, 0x00002000}, EFlag::wdt},
    {{0x80000001}, {0, 0, 0x00008000}, EFlag::lwp},
    {{0x80000001}, {0, 0, 0x00010000}, EFlag::fma4},
    {{0x80000001}, {0, 0, 0x00020000}, EFlag::tce},
    {{0x80000001}, {0, 0, 0x00080000}, EFlag::nodeid_msr},
    {{0x80000001}, {0, 0, 0x00200000}, EFlag::tbm},
    {{0x80000001}, {0, 0, 0x00400000}, EFlag::topoext},
    {{0x80000001}, {0, 0, 0x00800000}, EFlag::perfctr_core},
    {{0x80000001}, {0, 0, 0x01000000}, EFlag::perfctr_nb},
    {{0x80000001}, {0, 0, 0, 0x00000800}, EFlag::syscall},
    {{0x80000001}, {0, 0, 0, 0x00100000}, EFlag::nxxd},
    {{0x80000001}, {0, 0, 0, 0x00400000}, EFlag::mmxext},
    {{0x80000001}, {0, 0, 0, 0x02000000}, EFlag::fxsr_opt},
    {{0x80000001}, {0, 0, 0, 0x04000000}, EFlag::pdpe1gb},
    {{0x80000001}, {0, 0, 0, 0x08000000}, EFlag::rdtscp},
    {{0x80000001}, {0, 0, 0, 0x20000000}, EFlag::lmi64},
    {{0x80000001}, {0, 0, 0, 0x40000000}, EFlag::_3dnowext},
    {{0x80000001}, {0, 0, 0, 0x80000000}, EFlag::_3dnow},

    {{0x80000007}, {0, 0, 0, 0x00000100}, EFlag::invtsc},

    {{0x8000000A}, {0, 0, 0, 0x00000001}, EFlag::npt},
    {{0x8000000A}, {0, 0, 0, 0x00000002}, EFlag::lbrv},
    {{0x8000000A}, {0, 0, 0, 0x00000004}, EFlag::svm_lock},
    {{0x8000000A}, {0, 0, 0, 0x00000008}, EFlag::nrip_save},
    {{0x8000000A}, {0, 0, 0, 0x00000010}, EFlag::tsc_scale},
    {{0x8000000A}, {0, 0, 0, 0x00000020}, EFlag::vmcb_clean},
    {{0x8000000A}, {0, 0, 0, 0x00000040}, EFlag::flushbyasid},
    {{0x8000000A}, {0, 0, 0, 0x00000080}, EFlag::decodeassists},
    {{0x8000000A}, {0, 0, 0, 0x00000400}, EFlag::pause_filter},
    {{0x8000000A}, {0, 0, 0, 0x00001000}, EFlag::pfthreshold},
    {{0x8000000A}, {0, 0, 0, 0x00080000}, EFlag::x2avic},

    {{0xC0000001}, {0, 0, 0, 0x00000004}, EFlag::xstore},
    {{0xC0000001}, {0, 0, 0, 0x00000008}, EFlag::xstore_en},
    {{0xC0000001}, {0, 0, 0, 0x00000040}, EFlag::xcrypt},
    {{0xC0000001}, {0, 0, 0, 0x00000080}, EFlag::xcrypt_en},
    {{0xC0000001}, {0, 0, 0, 0x00000100}, EFlag::ace2},
    {{0xC0000001}, {0, 0, 0, 0x00000200}, EFlag::ace2_en},
    {{0xC0000001}, {0, 0, 0, 0x00000400}, EFlag::phe},
    {{0xC0000001}, {0, 0, 0, 0x00000800}, EFlag::phe_en},
    {{0xC0000001}, {0, 0, 0, 0x00001000}, EFlag::pmm},
    {{0xC0000001}, {0, 0, 0, 0x00002000}, EFlag::pmm_en},

    {{0x00000007, 0, 1}, { 0x00000020}, EFlag::avx512_bf16},
    {{0x00000007, 0, 1}, { 0x00000010}, EFlag::avxvnni},
}};
// clang-format on

void
X86Cpu::Impl::update()
{
    std::map<RequestT, ResponseT> rawCpuid;

    /* manufacturer details */
    auto resp                = at(RequestT{ 0x0000'0001, 0, 0, 0 });
    m_vendor_info.m_mfg      = m_cutils->getMfgInfo(at(RequestT{ 0, 0, 0, 0 }));
    m_vendor_info.m_family   = m_cutils->getFamily(resp.eax);
    m_vendor_info.m_model    = m_cutils->getModel(resp.eax);
    m_vendor_info.m_stepping = m_cutils->getStepping(resp.eax);
    setUarch();
    for (const auto& query : CPUID_MAP) {
        const auto& [req, expected, flg] = query;
        if (rawCpuid.find(req) == rawCpuid.end()) {
            rawCpuid.insert(std::make_pair(req, at(req)));
        }
        updateflag(flg, m_cutils->hasFlag(expected, rawCpuid[req]));
    }

    /*
     * Globally disable some
     * *_USABLE flags, so that
     * all ifunc's sees them
     */
    if (m_vendor_info.m_mfg == EVendor::Amd
        && m_vendor_info.m_family >= EFamily::Zen) {
        /* todo */
    }

#if defined(AU_BUILD_TYPE_DEVELOPER)
    /* We override any form  */
    // auto env_cpuid_str = AU_CPUID_OVERRIDE;
    // au_env_get(env_cpuid_str, sizeof(env_cpuid_str), vinfo);
#endif

    /* Update cache info */
    m_cutils->updateCacheView(m_cache_view);
}
ResponseT
X86Cpu::Impl::at(RequestT& req) const
{
    return m_cutils->__raw_cpuid(req);
}

bool
X86Cpu::Impl::isIntel() const
{
    return m_vendor_info.m_mfg == EVendor::Intel;
}

bool
X86Cpu::Impl::isAMD() const
{
    return m_vendor_info.m_mfg == EVendor::Amd;
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
    return m_avail_flags.at(eflag) && m_usable_flags.at(eflag);
}

EUarch
X86Cpu::Impl::getUarch() const
{
    return m_vendor_info.m_uarch;
}

bool
X86Cpu::Impl::isUarch(EUarch uarch, bool strict) const
{
    if (uarch < EUarch::Unknown || uarch > EUarch::Max)
        return false;
    if (strict)
        return uarch == m_vendor_info.m_uarch;
    return uarch <= m_vendor_info.m_uarch;
}

VendorInfo
X86Cpu::Impl::getVendorInfo() const
{
    return m_vendor_info;
}

void
X86Cpu::Impl::setUsableFlag(EFlag const& eflag, bool res)
{
    m_usable_flags[eflag] = res;
}

#if 0
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

    auto found = std::ranges::find_if(
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
#endif

} // namespace Au
