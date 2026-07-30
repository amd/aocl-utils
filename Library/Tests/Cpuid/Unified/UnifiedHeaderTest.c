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
 * Standalone C11 test for the standalone header (au_cpuid_header_only.h only, no libs).
 * Uses simnow-replay mock to validate against MockTest.hh expectations.
 */

#include "Capi/au/cpuid/au_cpuid_header_only.h"

/* Undef NDEBUG so asserts fire in Release builds. */
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PROJECT_SOURCE_DIR fallback for standalone compiles. */
#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR "."
#endif

/* Simnow-replay mock: parse {req}:{resp} lines into (eax,ecx)->regs table. */

#define MOCK_MAX_ENTRIES 64

typedef struct
{
    uint32_t        eax;
    uint32_t        ecx;
    au_cpuid_regs_t regs;
} mock_entry_t;

typedef struct
{
    mock_entry_t entries[MOCK_MAX_ENTRIES];
    int          count;
} mock_table_t;

static void
mock_table_load(mock_table_t* t, const char* repo_root, const char* model)
{
    char  path[1024];
    FILE* f;

    t->count = 0;
    snprintf(path,
             sizeof(path),
             "%s/Library/Tests/Cpuid/Mock/simnowdata/%s/%s",
             repo_root,
             model,
             model);

    f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "FATAL: cannot open simnow data file: %s\n", path);
        exit(1);
    }

    {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            unsigned int req_eax, req_f1, req_ecx, req_f3;
            unsigned int eax, ebx, ecx, edx;
            int          matched = sscanf(line,
                                          " { 0x%x , 0x%x , 0x%x , 0x%x } : "
                                          "{ 0x%x , 0x%x , 0x%x , 0x%x }",
                                          &req_eax,
                                          &req_f1,
                                          &req_ecx,
                                          &req_f3,
                                          &eax,
                                          &ebx,
                                          &ecx,
                                          &edx);
            (void)req_f1;
            (void)req_f3;
            if (matched != 8)
                continue;
            if (t->count >= MOCK_MAX_ENTRIES) {
                fprintf(stderr, "FATAL: mock table overflow for %s\n", model);
                exit(1);
            }
            t->entries[t->count].eax      = (uint32_t)req_eax;
            t->entries[t->count].ecx      = (uint32_t)req_ecx;
            t->entries[t->count].regs.eax = (uint32_t)eax;
            t->entries[t->count].regs.ebx = (uint32_t)ebx;
            t->entries[t->count].regs.ecx = (uint32_t)ecx;
            t->entries[t->count].regs.edx = (uint32_t)edx;
            t->count++;
        }
    }
    fclose(f);

    if (t->count == 0) {
        fprintf(stderr, "FATAL: no entries parsed from %s\n", path);
        exit(1);
    }
}

/* ctx is void* (non-const) to match au_cpu_info_t::raw_fn's signature. */
static void
mock_raw_fn(uint32_t eax, uint32_t ecx, au_cpuid_regs_t* out, void* ctx)
{
    mock_table_t* t = (mock_table_t*)ctx;
    int           i;

    memset(out, 0, sizeof(*out));
    for (i = 0; i < t->count; i++) {
        if (t->entries[i].eax == eax && t->entries[i].ecx == ecx) {
            *out = t->entries[i].regs;
            return;
        }
    }
}

static void
init_mocked(au_cpu_info_t* c,
            mock_table_t*  t,
            const char*    repo_root,
            const char*    model)
{
    mock_table_load(t, repo_root, model);
    memset(c, 0, sizeof(*c));
    c->raw_fn  = mock_raw_fn;
    c->raw_ctx = t;
    au_cpuid_init(c, AU_CPUID_CURRENT_CPU, false);
}

/* ---------------------------------------------------------------------- *
 *  Model expectations (authoritative: Library/Tests/Cpuid/Mock/MockTest.hh)
 * ---------------------------------------------------------------------- */

static void
test_epyc_genoa_v1(const char* repo_root)
{
    au_cpu_info_t c;
    mock_table_t  t;

    init_mocked(&c, &t, repo_root, "EPYC-Genoa-v1");

    assert(au_cpuid_info_is_amd(&c));
    assert(!au_cpuid_info_is_intel(&c));
    assert(c.family == AU_FAMILY_19H);
    assert(c.uarch == AU_UARCH_ZEN4);
    assert(au_cpuid_info_is_x86_64v2(&c));
    assert(au_cpuid_info_is_x86_64v3(&c));
    assert(au_cpuid_info_is_x86_64v4(&c));
    assert(au_cpuid_info_is_zen_family(&c));

    /* Real flags from EPYC-Genoa-v1/FlagsT.txt / FlagsF.txt. */
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx2")));
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx512f")));
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("sha_ni")));
    assert(!au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("vmx")));

    printf("PASS: EPYC-Genoa-v1 (AMD, Family19h, Zen4, v2/v3/v4)\n");
}

static void
test_epyc_milan_v1(const char* repo_root)
{
    au_cpu_info_t c;
    mock_table_t  t;

    init_mocked(&c, &t, repo_root, "EPYC-Milan-v1");

    assert(au_cpuid_info_is_amd(&c));
    assert(!au_cpuid_info_is_intel(&c));
    assert(c.family == AU_FAMILY_19H);
    assert(c.uarch == AU_UARCH_ZEN3);
    assert(au_cpuid_info_is_x86_64v2(&c));
    assert(au_cpuid_info_is_x86_64v3(&c));
    assert(
        !au_cpuid_info_is_x86_64v4(&c)); /* the key Milan-vs-Genoa contrast */
    assert(au_cpuid_info_is_zen_family(&c));

    /* Real flags from EPYC-Milan-v1/FlagsT.txt / FlagsF.txt. */
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx2")));
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("sha_ni")));
    assert(!au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx512f")));
    assert(!au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("vmx")));

    printf("PASS: EPYC-Milan-v1 (AMD, Family19h, Zen3, v4 FALSE)\n");
}

static void
test_venice_v1(const char* repo_root)
{
    au_cpu_info_t c;
    mock_table_t  t;

    init_mocked(&c, &t, repo_root, "Venice-v1");

    assert(au_cpuid_info_is_amd(&c));
    assert(!au_cpuid_info_is_intel(&c));
    assert(c.family == AU_FAMILY_1AH);
    assert(c.uarch == AU_UARCH_ZEN6);
    assert(au_cpuid_info_is_x86_64v2(&c));
    assert(au_cpuid_info_is_x86_64v3(&c));
    assert(au_cpuid_info_is_x86_64v4(&c));
    assert(au_cpuid_info_is_zen_family(&c));

    /* Real flags from Venice-v1/FlagsT.txt / FlagsF.txt. */
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx2")));
    assert(au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("clzero")));
    assert(!au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("avx10")));
    assert(!au_cpuid_info_has_flag(&c, au_cpuid_flag_from_string("vmx")));

    printf("PASS: Venice-v1 (AMD, Family1Ah, Zen6, v2/v3/v4)\n");
}

/* Family 1Bh regression guard (no simnow fixture yet, drive resolver directly). */
static void
test_family_1bh_synthetic(void)
{
    au_cpu_info_t c;
    memset(&c, 0, sizeof(c));
    c.vendor = AU_VENDOR_AMD;
    c.family = au_cpuid_family(0x00C00F00); /* leaf-1 EAX encoding of family 0x1B */
    assert(c.family == 0x1B);
    c.model = 0x00;
    au_cpuid_setuarch(&c);
    assert(c.uarch == AU_UARCH_ZEN6);

    printf("PASS: synthetic Family1Bh -> Zen6\n");
}

int
main(int argc, char** argv)
{
    const char* repo_root = (argc > 1) ? argv[1] : PROJECT_SOURCE_DIR;

    test_epyc_genoa_v1(repo_root);
    test_epyc_milan_v1(repo_root);
    test_venice_v1(repo_root);
    test_family_1bh_synthetic();

    printf("ALL PASS: au_cpuid_header_only.h (C11)\n");
    return 0;
}
