<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# C-API Behaviour Summary

## Behaviour of `au_cpuid_arch_is_*` API

| AOCL 5.3.2 | au_cpuid_arch_is_zen / alci_arch_is_zen | au_cpuid_arch_is_zenplus | au_cpuid_arch_is_zen2 / alci_arch_is_zen2 | au_cpuid_arch_is_zen3 / alci_arch_is_zen3 | au_cpuid_arch_is_zen4 / alci_arch_is_zen4 | au_cpuid_arch_is_zen5 / alci_arch_is_zen5 | au_cpuid_arch_is_zen6 | au_cpuid_arch_is_zen_family |
| :--------: | :-------------------------------------: | :----------------------: | :---------------------------------------: | :---------------------------------------: | :---------------------------------------: | :---------------------------------------: | :-------------------: | :-------------------------: |
|  Skylake   |                  False                  |          False           |                   False                   |                   False                   |                   False                   |                   False                   |         False         |            False            |
| Bulldozer  |                  False                  |          False           |                   False                   |                   False                   |                   False                   |                   False                   |         False         |            False            |
|    Zen1/1+ |                  True                   |           True           |                   False                   |                   False                   |                   False                   |                   False                   |         False         |            True             |
|    Zen2    |                  True                   |           True           |                   True                    |                   False                   |                   False                   |                   False                   |         False         |            True             |
|    Zen3    |                  True                   |           True           |                   True                    |                   True                    |                   False                   |                   False                   |         False         |            True             |
|    Zen4    |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   False                   |         False         |            True             |
|    Zen5    |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   True                    |         False         |            True             |
|    Zen6    |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   True                    |         True          |            True             |
|  Zen[X>6]  |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   True                    |         True          |            True             |

`au_cpuid_arch_is_zenplus` is retained as a compatibility/cumulative check.
It is currently equivalent to `au_cpuid_arch_is_zen`; the decoder does not
expose ZenPlus as a separately detected microarchitecture yet.

## Other current C APIs

The current C surface also includes the x86-64 level checks
`au_cpuid_arch_is_x86_64v2`, `au_cpuid_arch_is_x86_64v3`, and
`au_cpuid_arch_is_x86_64v4`. These are cumulative feature-level checks: a
processor that satisfies v4 also satisfies v2 and v3.

For heterogeneous x86 processors, `au_cpuid_is_hybrid()` reports whether the
package advertises hybrid execution, and `au_cpuid_get_core_type()` returns the
core-type byte for the current core. The latter returns zero when the platform
does not support the query.

## Behaviour of `alci_arch_is*` API for AOCL 4.2.

| AOCL 4.2  | alci_arch_is_zen | alci_arch_is_zen2 | alci_arch_is_zen3 | alci_arch_is_zen4 |
| :-------: | :--------------: | :---------------: | :---------------: | :---------------: |
|  Skylake  |      False       |       False       |       False       |       False       |
| Bulldozer |      False       |       False       |       False       |       False       |
|   Zen1/1+ |       True       |       False       |       False       |       False       |
|   Zen2    |      False       |       True        |       False       |       False       |
|   Zen3    |      False       |       False       |       True        |       False       |
|   Zen4    |      False       |       False       |       False       |       True        |
|   Zen5    |      False       |       False       |       False       |       True        |
| Zen[X>5]  |      False       |       False       |       False       |       True        |
