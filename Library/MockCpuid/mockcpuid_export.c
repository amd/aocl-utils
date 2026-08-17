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

/*
 * The whole of the C surface of libaoclutils_mock_c, and the common masked
 * CPUID portion of libaoclutils_mock.
 *
 * The au_cpuid_* and alci_* entry points are intentionally not hand-written
 * here. Including the same cpuid.h / cpuid_legacy.h bodies the real library
 * compiles emits them with the CPUID-reaching calls redirected through the
 * mask and AU_CPUID_API set to the export attribute.
 *
 * That is what keeps the surface in sync. A new entry point added to cpuid.h
 * appears in this library on the next build, with masking applied, because it
 * is literally the same source text. The parity ctest (MockCpuidParityTest)
 * guards the remaining gap -- an entry point that reaches CPUID by some route
 * none of the redirects below cover would link but not mask, and the
 * behavioural tests catch that.
 *
 * Generating the sources from the headers, as the design document floats, would
 * be strictly more machinery for strictly less coverage: a generator re-derives
 * the bodies and can re-derive them wrongly, whereas #include cannot.
 */

#include "Capi/au/macros.h"

/* Mask first: au_mockcpuid.h pulls in cpuid_core.h with no AU_CPUID_API set,
 * so the core's static-inline helpers keep internal linkage as usual. */
#include "au_mockcpuid.h"

/* au_capi_resolve (defined inside cpuid.h, shared by every flag/uarch entry
 * point including alci_cpu_has_flag) zeroes the hook and calls au_cpuid_init.
 * Redirecting au_cpuid_init therefore reaches all of them at once, and keeps
 * au_capi_resolve's own definition intact rather than shadowing it. The
 * masking init re-arms the hook that au_capi_resolve just cleared. */
#define au_cpuid_init(c, cpu_num, strict) au_mock_cpuid_init(c, cpu_num, strict)

/* au_cpuid_is_hybrid / au_cpuid_get_core_type bypass the resolver and read
 * their leaf directly with a NULL hook. Route those through the mask too, so no
 * entry point escapes it. Both take (raw_fn, ctx) as their first arguments. */
#define au_cpuid_read_is_hybrid(fn, ctx)                                       \
    au_cpuid_read_is_hybrid(au_mock_raw_hook, (void*)au_mock_state())
#define au_cpuid_read_core_type(fn, ctx)                                       \
    au_cpuid_read_core_type(au_mock_raw_hook, (void*)au_mock_state())

#define AU_CPUID_API AUD_API_EXPORT
#define AU_CPUID_IMPLEMENTATION

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/cpuid/cpuid_legacy.h"

/* The control API lives in this same translation unit as the query bodies, so
 * its setters and the query hook share one mutable policy object. */
#define AU_MOCKCPUID_API AUD_API_EXPORT
#include "Capi/au/cpuid/mockcpuid.h"

extern AU_MOCKCPUID_API au_mock_status_t
// cppcheck-suppress staticFunction
au_mock_apply_profile(au_cpu_num_t cpu_num, const char* name)
{
    return (au_mock_status_t)au_mock_apply_profile_impl((uint32_t)cpu_num,
                                                        name);
}

AU_MOCKCPUID_API au_mock_status_t
au_mock_reset(au_cpu_num_t cpu_num)
{
    return (au_mock_status_t)au_mock_reset_impl((uint32_t)cpu_num);
}

/* Initialize the immutable host truth once. Mark the state ready before
 * entering the environment front door: au_mock_apply_profile() then reuses
 * this object instead of recursing, and both environment and API setup share
 * exactly one profile implementation. */
static inline au_mock_state_t*
au_mock_state(void)
{
    static au_mock_state_t state = { 0 };
    static bool            ready = false;

    if (!ready) {
        const char* profile;

        au_mock_read_host(&state);
        au_mock_clear_policy(&state);
        ready = true;

        profile = getenv("AU_CPUID_MOCK_PROFILE");
        if (profile != NULL && profile[0] != '\0') {
            if (strcmp(profile, "list") == 0)
                au_mock_profile_list(stderr);
            else
                (void)au_mock_apply_profile(AU_CPUID_MOCK_ALL_CPUS, profile);
        }
    }
    return &state;
}
