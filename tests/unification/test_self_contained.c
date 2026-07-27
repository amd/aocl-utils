/*
 * test_self_contained.c - Verify the unified header is truly self-contained.
 *
 * This TU includes ONLY the unified header and exercises the API. It must
 * compile with ONLY the directory containing the unified header on the
 * include path (NO SDK/Include, NO build/generated), and must link with nothing
 * (no libstdc++, no libaoclutils).
 */

#include "au_cpuid_unified.h"

#include <stdio.h>

int main(void)
{
    /* Query current CPU */
    bool is_amd = au_cpuid_is_amd(AU_CURRENT_CPU_NUM);
    printf("CPU is AMD: %s\n", is_amd ? "true" : "false");

    /* Query uarch */
    bool is_zen_family = au_cpuid_arch_is_zen_family(AU_CURRENT_CPU_NUM);
    printf("CPU is Zen family: %s\n", is_zen_family ? "true" : "false");

    /* Query specific Zen versions */
    bool is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    bool is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    bool is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    printf("Zen4: %s, Zen5: %s, Zen6: %s\n",
           is_zen4 ? "yes" : "no",
           is_zen5 ? "yes" : "no",
           is_zen6 ? "yes" : "no");

    /* Query x86-64 levels */
    bool v2 = au_cpuid_arch_is_x86_64v2(AU_CURRENT_CPU_NUM);
    bool v3 = au_cpuid_arch_is_x86_64v3(AU_CURRENT_CPU_NUM);
    bool v4 = au_cpuid_arch_is_x86_64v4(AU_CURRENT_CPU_NUM);
    printf("x86-64-v2: %s, v3: %s, v4: %s\n",
           v2 ? "yes" : "no",
           v3 ? "yes" : "no",
           v4 ? "yes" : "no");

    /* Query feature flags */
    const char* flags[] = { "avx2", "avx512f", "vaes" };
    bool has_all = au_cpuid_has_flags_all(AU_CURRENT_CPU_NUM, flags, 3);
    bool has_any = au_cpuid_has_flags_any(AU_CURRENT_CPU_NUM, flags, 3);
    printf("Has all [avx2, avx512f, vaes]: %s\n", has_all ? "yes" : "no");
    printf("Has any [avx2, avx512f, vaes]: %s\n", has_any ? "yes" : "no");

    /* Get vendor info */
    char vendor_info[256];
    au_cpuid_get_vendor(AU_CURRENT_CPU_NUM, vendor_info, sizeof(vendor_info));
    printf("Vendor info (newline-separated):\n%s\n", vendor_info);

    printf("\n=== Self-contained unified header test PASSED ===\n");
    return 0;
}
