// cppcheck-suppress-file constParameterCallback

/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

/*
 * au_mockcpuid.h - internal masking engine for the Mock-CPUID link shims.
 *
 * This header is deliberately internal to the mock target. Consumers include
 * the public CPUID header and link a mock library; they do not include this
 * file. The engine reads the real host once, then clears suppressed feature
 * bits from raw CPUID responses before the unmodified AOCL decoder sees them.
 * That keeps feature flags, x86-64 levels, vendor, and uarch answers coherent.
 *
 * The host descriptor is immutable after initialization. The policy fields are
 * intentionally mutable for the Phase-1 control API. Setters are not safe to
 * run concurrently with queries; configure before creating worker threads.
 */

#ifndef __AU_MOCKCPUID_H__
#define __AU_MOCKCPUID_H__

#include "Capi/au/cpuid/cpuid_core.h"
#include "Capi/au/cpuid/mockcpuid_sentinel.h"

#include "mockcpuid_profiles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These values mirror the public status enum without making the internal
 * engine include mockcpuid.h (which would include cpuid.h before this
 * translation unit installs its redirect). */
typedef enum au_mock_result
{
    AU_MOCK_RESULT_OK = 0,
    AU_MOCK_RESULT_NO_PROFILE,
    AU_MOCK_RESULT_HOST_LACKS,
    AU_MOCK_RESULT_IDENTITY_UPWARD,
    AU_MOCK_RESULT_PERCPU_UNSUPPORTED,
    AU_MOCK_RESULT_BAD_CPU,
    AU_MOCK_RESULT_IDENTITY_UNCLASSIFIED,
} au_mock_result_t;

/* The public typed sentinel and both implementation checks derive from the
 * same value-only definition in mockcpuid_sentinel.h. */
static const uint32_t au_mock_all_cpus = AU_CPUID_MOCK_ALL_CPUS_VALUE;

/* The complete host truth is read once. Only the policy members below change
 * after that point. */
typedef struct au_mock_state
{
    bool          active;
    au_cpu_info_t host;
    uint32_t      host_leaf1_eax;
    uint32_t      host_vendor_ebx;
    uint32_t      host_vendor_ecx;
    uint32_t      host_vendor_edx;
    au_flag_set_t suppress;
    uint32_t      leaf1_eax;
    uint32_t      vendor_ebx;
    uint32_t      vendor_ecx;
    uint32_t      vendor_edx;
} au_mock_state_t;

/* Defined by the export translation unit after the public API declaration is
 * visible. Keeping initialization there lets environment setup call the same
 * au_mock_apply_profile front door as programmatic setup. */
static inline au_mock_state_t*
au_mock_state(void);

static inline bool
au_mock_strict_mode(void)
{
    const char* value = getenv("AU_CPUID_MOCK_STRICT");

    /* Strict is on unless one of the explicit, case-insensitive off tokens is
     * supplied. Unknown values fail safe to strict. */
    if (value == NULL)
        return true;
    return au_mock_strcasecmp(value, "0") != 0
           && au_mock_strcasecmp(value, "false") != 0
           && au_mock_strcasecmp(value, "no") != 0
           && au_mock_strcasecmp(value, "off") != 0;
}

static inline void
au_mock_clear_policy(au_mock_state_t* state)
{
    int i;

    state->active     = false;
    state->leaf1_eax  = 0;
    state->vendor_ebx = 0;
    state->vendor_ecx = 0;
    state->vendor_edx = 0;
    for (i = 0; i < AU_CPUID_BITMAP_WORDS; i++)
        state->suppress.w[i] = 0;
}

static inline void
au_mock_read_host(au_mock_state_t* state)
{
    au_cpuid_regs_t r0;
    au_cpuid_regs_t r1;

    state->host.raw_fn  = NULL;
    state->host.raw_ctx = NULL;
    au_cpuid_init(&state->host, AU_CPUID_CURRENT_CPU, false);

    /* Cache the raw identity too; profile validation must not issue a fresh
     * host sweep every time a setter is called. */
    au_cpuid_raw(0x00000000, 0, &r0);
    au_cpuid_raw(0x00000001, 0, &r1);
    state->host_leaf1_eax  = r1.eax;
    state->host_vendor_ebx = r0.ebx;
    state->host_vendor_ecx = r0.ecx;
    state->host_vendor_edx = r0.edx;
}

static inline void
au_mock_refuse_feature(const char* name)
{
    fprintf(stderr,
            "AU_CPUID_MOCK_PROFILE: refusing feature '%s': the host CPU "
            "does not support it; it remains disabled.\n",
            name);
}

static inline void
au_mock_abort_request(au_mock_result_t result)
{
    fprintf(stderr,
            "AU_CPUID_MOCK: request refused (status %d); strict mode is "
            "on, so the process is aborting. Set AU_CPUID_MOCK_STRICT=0 "
            "to warn and continue.\n",
            (int)result);
    fflush(stderr);
    abort();
}

static inline au_mock_result_t
au_mock_identity_allowed(const au_mock_profile_t* profile,
                         const au_mock_state_t*   state,
                         const char**             why)
{
    au_cpu_info_t   profile_info = { 0 };
    au_cpuid_regs_t vendor_regs;

    if (profile->leaf1_eax == 0)
        return AU_MOCK_RESULT_OK;

    if (state->host_vendor_ebx != profile->vendor_ebx
        || state->host_vendor_ecx != profile->vendor_ecx
        || state->host_vendor_edx != profile->vendor_edx) {
        *why = "it is a different CPU vendor than the host";
        return AU_MOCK_RESULT_IDENTITY_UPWARD;
    }

    /* Resolve the profile through the same uarch registry as the real decoder.
     * The monotonic uarch enum is the directional identity check; family/model
     * numbers are not globally chronological. */
    vendor_regs.eax     = 0;
    vendor_regs.ebx     = profile->vendor_ebx;
    vendor_regs.ecx     = profile->vendor_ecx;
    vendor_regs.edx     = profile->vendor_edx;
    profile_info.vendor = au_cpuid_vendor(&vendor_regs);
    profile_info.family = au_cpuid_family(profile->leaf1_eax);
    profile_info.model  = au_cpuid_model(profile->leaf1_eax);
    profile_info.avail  = profile->flags;
    au_cpuid_setuarch(&profile_info);

    if (profile_info.uarch == AU_UARCH_UNKNOWN
        || state->host.uarch == AU_UARCH_UNKNOWN) {
        if (profile_info.uarch == AU_UARCH_UNKNOWN)
            *why = "the profile's microarchitecture is not in the uarch "
                   "registry";
        else
            *why = "the host's microarchitecture is not in the uarch "
                   "registry";
        return AU_MOCK_RESULT_IDENTITY_UNCLASSIFIED;
    }

    if (profile_info.uarch > state->host.uarch) {
        *why = "it is newer than the host CPU";
        return AU_MOCK_RESULT_IDENTITY_UPWARD;
    }

    return AU_MOCK_RESULT_OK;
}

/* Apply one named profile as a replacement baseline. The safe subset is
 * installed even when the profile asks for host-absent features or an unsafe
 * identity. Feature surplus is ordinary narrowing; the returned result is
 * reserved for profile/identity errors that strict mode may terminate. */
static inline au_mock_result_t
au_mock_policy_apply_profile(au_mock_state_t* state, const char* name)
{
    const au_mock_profile_t* profile;
    au_flag_set_t            suppress;
    const char*              why    = NULL;
    au_mock_result_t         result = AU_MOCK_RESULT_OK;
    au_mock_result_t         identity_result;
    unsigned                 i;

    profile = au_mock_profile_find(name);
    if (profile == NULL) {
        fprintf(stderr,
                "AU_CPUID_MOCK_PROFILE: unknown profile '%s'. Run with "
                "AU_CPUID_MOCK_PROFILE=list to list available profiles.\n",
                name != NULL ? name : "(null)");
        return AU_MOCK_RESULT_NO_PROFILE;
    }

    for (i = 0; i < AU_CPUID_BITMAP_WORDS; i++)
        suppress.w[i] = 0;

    for (i = 1; i < (unsigned)AU_FLAG_MAX; i++) {
        const bool in_profile = au_flag_get(&profile->flags, i);
        const bool on_host    = au_flag_get(&state->host.avail, i);

        if (on_host && !in_profile) {
            au_flag_set(&suppress, i);
        } else if (in_profile && !on_host) {
            au_mock_refuse_feature(au_mock_flag_name(i));
        }
    }

    state->suppress   = suppress;
    state->leaf1_eax  = 0;
    state->vendor_ebx = 0;
    state->vendor_ecx = 0;
    state->vendor_edx = 0;

    identity_result = au_mock_identity_allowed(profile, state, &why);
    if (identity_result != AU_MOCK_RESULT_OK) {
        fprintf(stderr,
                "AU_CPUID_MOCK_PROFILE: '%s' keeps the host CPU identity "
                "because %s; its safe feature subset is still applied.\n",
                name,
                why);
        result = identity_result;
    } else if (profile->leaf1_eax != 0) {
        state->leaf1_eax  = profile->leaf1_eax;
        state->vendor_ebx = profile->vendor_ebx;
        state->vendor_ecx = profile->vendor_ecx;
        state->vendor_edx = profile->vendor_edx;
    }

    state->active = true;
    return result;
}

/* The raw hook reads the real host response, substitutes an accepted downward
 * identity, then clears exactly the bits selected by the current policy. */
static inline void
au_mock_raw_hook(uint32_t eax, uint32_t ecx, au_cpuid_regs_t* out, void* ctx)
{
    const au_mock_state_t* state = (const au_mock_state_t*)ctx;
    int                    i;

    au_cpuid_raw(eax, ecx, out);

    if (state == NULL || !state->active)
        return;

    if (state->leaf1_eax != 0) {
        if (eax == 0x00000000 && ecx == 0) {
            out->ebx = state->vendor_ebx;
            out->ecx = state->vendor_ecx;
            out->edx = state->vendor_edx;
        } else if (eax == 0x00000001 && ecx == 0) {
            out->eax = state->leaf1_eax;
        }
    }

    for (i = 0; i < AU_CPUID_MAP_COUNT; i++) {
        const au_cpuid_entry_t* entry = &au_cpuid_map[i];

        if (entry->req.eax != eax || entry->req.ecx != ecx)
            continue;
        if (!au_flag_get(&state->suppress, entry->flag))
            continue;

        out->eax &= ~entry->expected.eax;
        out->ebx &= ~entry->expected.ebx;
        out->ecx &= ~entry->expected.ecx;
        out->edx &= ~entry->expected.edx;
    }
}

/* Public Phase-1 operations call these internal functions after validating the
 * reserved all-CPU selector. */
static inline au_mock_result_t
au_mock_apply_profile_impl(uint32_t cpu_num, const char* name)
{
    au_mock_state_t* state;
    au_mock_result_t result;

    if (cpu_num != au_mock_all_cpus)
        return AU_MOCK_RESULT_PERCPU_UNSUPPORTED;

    state  = au_mock_state();
    result = au_mock_policy_apply_profile(state, name);
    if (result != AU_MOCK_RESULT_OK && au_mock_strict_mode())
        au_mock_abort_request(result);
    return result;
}

static inline au_mock_result_t
au_mock_reset_impl(uint32_t cpu_num)
{
    if (cpu_num != au_mock_all_cpus)
        return AU_MOCK_RESULT_PERCPU_UNSUPPORTED;

    au_mock_clear_policy(au_mock_state());
    return AU_MOCK_RESULT_OK;
}

/* Drop-in replacement for the real resolver. When no policy is active this is
 * a direct pass-through. When active, the hook is installed and the real
 * decoder runs over masked raw registers. */
static inline bool
au_mock_cpuid_init(au_cpu_info_t* c, int cpu_num, bool strict)
{
    au_mock_state_t* state = au_mock_state();
    au_affinity_t    saved;
    bool             saved_valid = false;
    bool             ok;

    if (!state->active) {
        c->raw_fn  = NULL;
        c->raw_ctx = NULL;
        return au_cpuid_init(c, cpu_num, strict);
    }

    /* The real resolver skips pinning whenever raw_fn is non-NULL. Preserve
     * the old mock behavior by doing the requested selection around the sweep
     * ourselves, then restore the caller's original affinity. */
    if (cpu_num != AU_CPUID_CURRENT_CPU) {
        bool sel_err = false;
        int  target;

        saved_valid = au_affinity_save(&saved);
        target      = au_affinity_select_target(cpu_num, strict, &sel_err);
        if (strict && sel_err) {
            if (saved_valid)
                au_affinity_restore(&saved);
            return false;
        }
        if (target >= 0)
            (void)au_affinity_pin(target);
    }

    c->raw_fn  = au_mock_raw_hook;
    c->raw_ctx = (void*)state;
    ok         = au_cpuid_init(c, cpu_num, strict);

    if (saved_valid)
        au_affinity_restore(&saved);
    return ok;
}

#endif /* __AU_MOCKCPUID_H__ */
