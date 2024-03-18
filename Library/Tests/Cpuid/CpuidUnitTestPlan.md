# The cpuid module unit testplan
---------------------------------

## The cpuid module

The cpuid module provides details of underlying cpu architecture by reading and interpreting the cpuid register of the system.

## The objective

The testplan is to have a comprehensive test coverage for all components in the module.

## Classes of testcases

* **qemu tests**
    Qemu cpu emulator is used to emulate cpus to test both capis and cpp apis.
    Native tests corresponing to this is disabled by default as it doesn't make sense to run natively.
* **mock tests**
    Mock tests are used to test cpp apis and utility functions by mocking the cpuid register call.
    Mocked data is stored in simnow folder.
    The content of the folder is organised as follows.

```
simnowdata 
└───<folderperemulated cpuid>
    │   <cpuid>
    │   FlagsT.txt
    │   FlagsF.txt
 * The \<cpuid> file consists of cpuid register read corresponding to the particular cpuid for a set of inputs,
   in the form {request} : {response}
 * These files are shared by both qemu tests and mocktests.
 * FlagsT.txt and FlagsF.txt files are shared by all the hasflags tests.
 * Flags in FlagsT.txt marks the set of flags that should be present in the cpu
 * Flags in FlagsF.txt marks the set of flags that should not be present in the cpu.
```

* **native tests**
    Few native testcases are written to check edge cases and negative cases. More test are to be added for full coverage.
  
### List of cpp apis

* isamd
* isintel
* isx86v2
* isx86v3
* isx86v4
* hasflag
* isuarch
* getuarch
* getvendorinfo

### List of capis

* au_cpuid_is_amd
* au_cupid_has_flag
* au_cpuid_get_vendor
* au_cpuid_arch_is_zen
* au_cpuid_arch_is_zenplus
* au_cpuid_arch_is_zen2
* au_cpuid_arch_is_zen3
* au_cpuid_arch_is_zen4
* au_cpuid_arch_is_zen5

for all the amd cpu modules provided by qemu
and below intel cpus,

* broadwell-v1
* denverton-v1
* conroe-v1
* skylake-server-v1

The above c and cpp apis are tested using qemu testcases and mock tests.

Apart from that,
the utility functions that directly use the cpuid register read results to fill data \
in different datastructures used by above functions are tested with mocked up (cpuid register read) data.
Those functions are,

### List of utility functions

* getmfginfo
* getfamily
* getmodel
* getstepping
* hasflag
* updatecacheview
* updatecacheinfo


## The test matrix for c/cpp apis

### cpu features
#### amd cpus
| cpu type/function name  | epyc-genoa-v1 | epyc-milan-v1 | epyc-milan-v2 | epyc-rome-v1 |epyc-rome-v2 |
| ----------------------- | ------------- | ------------- | ------------- | ------------ |------------ |
| isamd                   | true          | true          | true          | true         |true         |
| isintel                 | false         | false         | false         | false        |false        |
| isx86_64v2              | true          | true          | true          | true         |true         |
| isx86_64v3              | true          | true          | true          | true         |true         |
| isx86_64v4              | true          | false         | false         | false        |false        |
| getuarch                | zen4          | zen3          | zen3          | zen2         |zen2         |

amd cpus(continued)

 | cpu type/function name  | epyc-rome-v3 | epyc-rome-v4 | epyc-v1 | epyc-v2 | epyc-v3 | epyc-v4 |opteron_g1-v1  |
 | ----------------------- | ------------ | ------------ | ------- | ------- | ------- | ------- |---------------|
 | isamd                   | true         | true         | true    | true    | true    | true    | true          |
 | isintel                 | false        | false        | false   | false   | false   | false   | false         |
 | isx86_64v2              | true         | true         | true    | true    | true    | true    | false         |
 | isx86_64v3              | true         | true         | true    | true    | true    | true    | false         |
 | isx86_64v4              | false        | false        | false   | false   | false   | false   | false         |
 | getUarch                | zen2         | zen2         | zen     | zen     | zen     | zen     | unknown       |

amd cpus(continued)

| cpu type/function name  |  opteron_g2-v1 | opteron_g3-v1 | opteron_g4-v1 | opteron_g5-v1 | phenom-v1 |
| ----------------------- |  ------------- | ------------- | ------------- | ------------- | --------- |
| isamd                   |  true          | true          | true          | true          | true      |
| isintel                 |  false         | false         | false         | false         | false     |
| isx86_64v2              |  false         | false         | true          | true          | false     |
| isx86_64v3              |  false         | false         | false         | false         | false     |
| isx86_64v4              |  false         | false         | false         | false         | false     |
| getUarch                |  unknown       | unknown       | unknown       | unknown       | unknown   |

#### intel cpus
| cpu type/function name  | broadwell-v1 | denverton-v1 | conroe-v1 | skylake-server-v1 |
| ----------------------- | ------------ | ------------ | --------- | ----------------- |
| isamd                   | false        | false        | false     | false             |
| isintel                 | true         | true         | true      | true              |
| isx86_64v2              | true         | true         | false     | true              |
| isx86_64v3              | true         | false        | false     | true              |
| isx86_64v4              | false        | false        | false     | true              |
| getUarch                |  unknown     | unknown      | unknown   | unknown           | 

### isa features
#### x86_64v2
| cpu type/flag | epyc-genoa-v1 | epyc-rome-v1 | opteron_g4-v1 | phenom-v1 | skylake-server-v1 | broadwell-v1 | denverton-v1 | conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| cx16          | true          | true         | true          | false     | true              | true         | true         | false     |
| lahf_lm       | true          | true         | true          | false     | true              | true         | true         | false     |
| popcnt        | true          | true         | true          | false     | true              | true         | true         | false     |
| sse4_1        | true          | true         | true          | false     | true              | true         | true         | false     |
| sse4_2        | true          | true         | true          | false     | true              | true         | true         | false     |
| ssse3         | true          | true         | true          | false     | true              | true         | true         | false     |

#### x86_64v3

| cpu type/flag | epyc-genoa-v1 | epyc-rome-v1 | opteron_g4-v1 | phenom-v1 | skylake-server-v1 | broadwell-v1 | denverton-v1 | conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| avx           | true          | true         | false         | false     | true              | true         | false        | false     |
| avx2          | true          | true         | false         | false     | true              | true         | false        | false     |
| bmi1          | true          | true         | false         | false     | true              | true         | false        | false     |
| bmi2          | true          | true         | false         | false     | true              | true         | false        | false     |
| f16c          | true          | true         | false         | false     | true              | true         | false        | false     |
| fma           | true          | true         | false         | false     | true              | true         | false        | false     |
| abm           | true          | true         | false         | false     | true              | true         | false        | false     |
| movbe         | true          | true         | false         | false     | true              | true         | false        | false     |
| xsave         | true          | true         | false         | false     | true              | true         | false        | false     |

#### x86_64v4
| cpu type/flag | epyc-genoa-v1 | epyc-rome-v1 | opteron_g4-v1 | phenom-v1 | skylake-server-v1 | broadwell-v1 | denverton-v1 | conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| avx512f       | true          | false        | false         | false     | true              | false        | false        | false     |
| avx512bw      | true          | false        | false         | false     | true              | false        | false        | false     |
| avx512cd      | true          | false        | false         | false     | true              | false        | false        | false     |
| avx512dq      | true          | false        | false         | false     | true              | false        | false        | false     |
| avx512vl      | true          | false        | false         | false     | true              | false        |              |           |

## Utility function test matrix
  | CpuType/VendorInfo| Vendor| Family | Model | Stepping   | Uarch   |   
  | ----------------- | ----- | -------| ----  | ---------- |-------- |
  | EPYC-Genoa-v1     |  Amd  | Zen4   | 0x11  |0x0         | zen4    |
  | EPYC-Milan-v1     |  Amd  | Zen4   | 0x01  |0x1         | zen3    |
  | EPYC-Milan-v2     |  Amd  | Zen4   | 0x01  |0x1         | zen3    |
  | EPYC-Rome-v1      |  Amd  | Zen2   | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v2      |  Amd  | Zen2   | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v3      |  Amd  | Zen2   | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v4      |  Amd  | Zen2   | 0x31  |0x0         | zen2    |
  | EPYC-v1           |  Amd  | Zen2   | 0x01  |0x2         | zen     |
  | EPYC-v2           |  Amd  | Zen2   | 0x01  |0x2         | zen     |
  | EPYC-v3           |  Amd  | Zen2   | 0x01  |0x2         | zen     |
  | EPYC-v4           |  Amd  | Zen2   | 0x01  |0x2         | zen     |
  | Opteron_G1-v1     |  Amd  | Unknown| 0x06  |0x1         | Unknown |
  | Opteron_G2-v1     |  Amd  | Unknown| 0x06  |0x1         | Unknown |
  | Opteron_G3-v1     |  Amd  | Unknown| 0x02  |0x3         | Unknown |
  | Opteron_G4-v1     |  Amd  | Unknown| 0x01  |0x2         | Unknown |
  | Opteron_G5-v1     |  Amd  | Unknown| 0x02  |0x0         | Unknown |
  | phenom-v1         |  Amd  | Unknown| 0x02  |0x3         | Unknown |
  | Broadwell-v1      |  Intel| Unknown| 0X3D  |0x2         | Unknown |
  | Denverton-v1      |  Intel| Unknown| 0x5F  |0x1         | Unknown |
  | Conroe-v1         |  Intel| Unknown| 0x0F  |0x3         | Unknown |
  | Skylake-Server-v1 |  Intel| Unknown| 0x55  |0x4         | Unknown |

## Integration tests <TBD>


## How to run the tests

`ctest` runs all the tests by default along with other tests. if running independently, call `core_cpuidtests`. it has to be done from the build folder as `./release/core_cpuidtest`.

**Note to developer:**
the test creates the x86cpu test suites with all the unit test cases for functions listed in the scope of the test. these tests are disabled by default because it doesn't make sense to run them on the host whose architecture might be unknown. use the `--gtest_also_run_disabled_tests` flag if these need to be enabled on the host.

a python script is written to invoke tests in the x86cpu test suite to run on different qemu emulated cpus. the script is placed in the same folder as the test sources. the script can be run as follows:

```bash
python ../library/tests/cpuid/cpuidtest.py epyc x86cpuid.disabled_isamd

```

the qemutest suites run by invoking the above python script to run on different emulated cpus.
