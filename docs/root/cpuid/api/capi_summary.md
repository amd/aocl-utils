# C-API Behaviour Summary

## Behaviour of `au_cpuid_arch_is_*` API

| AOCL 5.1   | au_cpuid_arch_is_zen / alci_arch_is_zen | au_cpuid_arch_is_zenplus | au_cpuid_arch_is_zen2 / alci_arch_is_zen2 | au_cpuid_arch_is_zen3 / alci_arch_is_zen3 | au_cpuid_arch_is_zen4 / alci_arch_is_zen4 | au_cpuid_arch_is_zen5 / alci_arch_is_zen5 | au_cpuid_arch_is_zen_family |
| :--------: | :-------------------------------------: | :----------------------: | :---------------------------------------: | :---------------------------------------: | :---------------------------------------: | :---------------------------------------: | :-------------------------: |
|  Skylake   |                  False                  |          False           |                   False                   |                   False                   |                   False                   |                   False                   |                 False                   |
| Bulldozer  |                  False                  |          False           |                   False                   |                   False                   |                   False                   |                   False                   |                 False                   |
|    Zen1    |                  True                   |          False           |                   False                   |                   False                   |                   False                   |                   False                   |                 True                    |
|   Zen1+    |                  True                   |           True           |                   False                   |                   False                   |                   False                   |                   False                   |                 True                    |
|    Zen2    |                  True                   |           True           |                   True                    |                   False                   |                   False                   |                   False                   |                 True                    |
|    Zen3    |                  True                   |           True           |                   True                    |                   True                    |                   False                   |                   False                   |                 True                    |
|    Zen4    |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   False                   |                 True                    |
|    Zen5    |                  True                   |           True           |                   True                    |                   True                    |                   True                    |                   True                    |                 True                    |
|  Zen[X>5]  |                  False                  |          False           |                   False                   |                   False                   |                   False                   |                   False                   |                 True                    |

## Behaviour of `alci_arch_is*` API for AOCL 4.2.

| AOCL 4.2  | alci_arch_is_zen | alci_arch_is_zen2 | alci_arch_is_zen3 | alci_arch_is_zen4 |
| :-------: | :--------------: | :---------------: | :---------------: | :---------------: |
|  Skylake  |      False       |       False       |       False       |       False       |
| Bulldozer |      False       |       False       |       False       |       False       |
|   Zen1    |       True       |       False       |       False       |       False       |
|   Zen1+   |       True       |       False       |       False       |       False       |
|   Zen2    |      False       |       True        |       False       |       False       |
|   Zen3    |      False       |       False       |       True        |       False       |
|   Zen4    |      False       |       False       |       False       |       True        |
|   Zen5    |      False       |       False       |       False       |       True        |
| Zen[X>5]  |      False       |       False       |       False       |       True        |
