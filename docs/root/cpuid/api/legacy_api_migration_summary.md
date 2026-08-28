# Legacy API Migration Guide

The `alci_*`/`alcpu_*` CPUID C API was deprecated in AOCL 5.0 and has been removed starting
with AOCL 6.0. The modern `au_cpuid_*` API is functionally equivalent and remains unchanged
by this removal. This guide maps every removed symbol to its replacement.

This guide is scoped to the 5.0→6.0 removal and will itself be removed/replaced with an
updated version as of AOCL 7.0.

## Functions

| Removed | Replacement | Notes |
|---|---|---|
| `alci_is_amd(cpu_num)` | `au_cpuid_is_amd(cpu_num)` | Same signature. |
| `alci_cpu_get_vendor(cpu_num, arr, size)` | `au_cpuid_get_vendor(cpu_num, arr, size)` | Return type changes `au_error_t` → `void`, since the old return was a hardcoded `1`, never a real error code. |
| `alci_arch_is_zen(cpu_num)` | `au_cpuid_arch_is_zen(cpu_num)` | Same signature. |
| `alci_arch_is_zen2(cpu_num)` | `au_cpuid_arch_is_zen2(cpu_num)` | Same signature. |
| `alci_arch_is_zen3(cpu_num)` | `au_cpuid_arch_is_zen3(cpu_num)` | Same signature. |
| `alci_arch_is_zen4(cpu_num)` | `au_cpuid_arch_is_zen4(cpu_num)` | Same signature. |
| `alci_arch_is_zen5(cpu_num)` | `au_cpuid_arch_is_zen5(cpu_num)` | Same signature. |
| `alci_cpu_has_flag(cpu_num, flag)` | `au_cpuid_has_flags(cpu_num, flags, 1)` (`const char* flags[] = { "..." }`) | **Do not use `au_cpuid_has_flag` (singular)** — it exists with a near-identical name but is itself deprecated in favor of `au_cpuid_has_flags`, and takes a `const char* const[]` + count, returning a heap-allocated `bool*` the caller must `free()`. The numeric flag ID (e.g. `ALC_E_FLAG_AVX`) also becomes a lowercase string name (e.g. `"avx"`) in a 1-element array, not the address of a scalar. `au_cpuid_has_flags` is the correct drop-in for a single boolean check: pass a 1-element array, get a `bool` back directly, no allocation. |

## Macros

All expand to the corresponding function above with the "current CPU" sentinel:

| Removed | Replacement | Notes |
|---|---|---|
| `alcpu_is_amd()` | `au_cpuid_is_amd(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_arch_is_zen()` | `au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_arch_is_zen2()` | `au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_arch_is_zen3()` | `au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_arch_is_zen4()` | `au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_arch_is_zen5()` | `au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM)` | Macro → explicit sentinel arg. |
| `alcpu_flag_is_available(flag)` | `au_cpuid_has_flags(AU_CURRENT_CPU_NUM, flags, 1)` (`const char* flags[] = { "..." }`) | Same has_flag(s) caveat as above applies; numeric flag → string name in a 1-element array. |

## Examples

**Vendor check** (return-type change):
```c
/* before */
au_error_t err = alci_cpu_get_vendor(0, buf, sizeof(buf));

/* after */
au_cpuid_get_vendor(0, buf, sizeof(buf)); /* void: no error code to check */
```

**Single flag check** (the has_flag trap):
```c
/* before */
if (alci_cpu_has_flag(0, ALC_E_FLAG_AVX)) { ... }

/* after */
const char* flags[] = { "avx" };
if (au_cpuid_has_flags(0, flags, 1)) { ... }
```

**"Current CPU" macro** (representative example):
```c
/* before */
if (alcpu_arch_is_zen4()) { ... }

/* after */
if (au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM)) { ... }
```
