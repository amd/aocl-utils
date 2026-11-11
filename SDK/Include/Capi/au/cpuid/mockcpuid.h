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

#ifndef __AU_CPUID_MOCKCPUID_H__
#define __AU_CPUID_MOCKCPUID_H__

#include <stdbool.h>

#include "Capi/au/cpuid/cpuid.h"
#include "Capi/au/cpuid/mockcpuid_sentinel.h"

/* Linkage is plain declarations for consumers and export linkage in the mock
 * library translation units. */
#ifndef AU_MOCKCPUID_API
#define AU_MOCKCPUID_API
#endif

AUD_EXTERN_C_BEGIN

/* Apply a named profile to every CPU. Specific-core policy is reserved for a
 * later phase; this sentinel is deliberately distinct from CURRENT_CPU (-1). */
#define AU_CPUID_MOCK_ALL_CPUS ((au_cpu_num_t)AU_CPUID_MOCK_ALL_CPUS_VALUE)

typedef enum au_mock_status
{
    AU_MOCK_OK = 0,
    AU_MOCK_ERR_NO_PROFILE,            /* unknown profile name */
    AU_MOCK_ERR_HOST_LACKS,            /* profile needs a host-absent feature */
    AU_MOCK_ERR_IDENTITY_UPWARD,       /* newer or other-vendor identity */
    AU_MOCK_ERR_PERCPU_UNSUPPORTED,    /* cpu_num != AU_CPUID_MOCK_ALL_CPUS */
    AU_MOCK_ERR_BAD_CPU,               /* reserved for future CPU validation */
    AU_MOCK_ERR_IDENTITY_UNCLASSIFIED, /* identity uarch is not in registry */
    /* This enum is open; callers should treat unknown values as generic
     * errors so future status additions remain ABI-compatible. */
} au_mock_status_t;

/* Present a named CPU using the generated profile table. Feature policy is
 * always intersected with the host's real capabilities. REPLACE semantics
 * discard any prior profile. Strict mode is on by default. Set
 * AU_CPUID_MOCK_STRICT=0, false, no, or off to downgrade an upward/
 * other-vendor/unclassifiable identity or unknown profile from abort() to a
 * warning and safe-subset continuation. Any other value remains strict. A
 * plain named profile's host-absent features are narrowed out and are never
 * fatal; the deliberate abort mode is intended for test-harness use. Configure
 * before starting threads: setters are not thread-safe with concurrent CPUID
 * queries. */
AU_MOCKCPUID_API au_mock_status_t
au_mock_apply_profile(au_cpu_num_t cpu_num, const char* name);

/* Drop all mock policy; subsequent CPUID queries report the true host. */
AU_MOCKCPUID_API au_mock_status_t
au_mock_reset(au_cpu_num_t cpu_num);

AUD_EXTERN_C_END

#endif /* __AU_CPUID_MOCKCPUID_H__ */
