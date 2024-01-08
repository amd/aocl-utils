# The Cpuid Module unit testplan

## The Cpuid Module

The Cpuid module provides details of underlying cpu architecture by reading and interpreting the cpuid register of the system
The objective of the testplan is to have a comprehensive test coverage for all components in the module
The result of reading different cpu’s cpuids yield different results based on the cpu, So testing the cpuid module requires an emulator of some kind so that
different usecases can be emulated and tested,
We chose to go with the qemu emulator as it provides most of the facilities that we needed for testing.

## The Scope of the tests

Qemu emulator is used to test the different interfaces provided by the cpuid module
such as

* isAMD
* isIntel
* isX86v2
* isX86v3
* isX86V4
* hasflag

for All the AMD Cpu modules provided by qemu
and below intel CPUs,

* Broadwell-v1
* Denverton-v1
* Conroe-v1
* Skylake-Server-v1

Apart from that,
The utility functions that directly use the cpuid register read results to fill data in different datastructures used by above functions are
tested with mocked up (cpuid register read) data
Those functions are

* __update_vendor_info
* __update_mfg_info
* __has_flag
* update_cache_info
* update_cache_view

So the test matrix as of now is going to be.

AMD Cpus
| CPU TYPE/function name | EPYC-Genoa-v1 | EPYC-Milan-v1 | EPYC-Milan-v2 | EPYC-Rome-v1 | EPYC-Rome-v2 | EPYC-Rome-v3 | EPYC-Rome-v4 | EPYC-v1 | EPYC-v2 | EPYC-v3 | EPYC-v4 |
| ----------------------- | ------------- | ------------- | ------------- | ------------ | ------------ | ------------ | ------------ | ------- | ------- | ------- | ------- |
| isAMD                   | True          | True          | True          | True         | True         | True         | True         | True    | True    | True    | True    |
| IsIntel                 | False         | False         | False         | False        | False        | False        | False        | False   | False   | False   | False   |
| isX86_64V2              | True          | True          | True          | True         | True         | True         | True         | True    | True    | True    | True    |
| isX86_64V3              | True          | True          | True          | True         | True         | True         | True         | True    | True    | True    | True    |
| isX86_64V4              | False         | False         | False         | False        | False        | False        | False        | False   | False   | False   | False   |

AMD Cpus(Continued)
| CPU TYPE/function name | Opteron_G1-v1 | Opteron_G2-v1 | Opteron_G3-v1 | Opteron_G4-v1 | Opteron_G5-v1 | phenom-v1 |
| ----------------------- | ------------- | ------------- | ------------- | ------------- | ------------- | --------- |
| isAMD                   | True          | True          | True          | True          | True          | True      |
| IsIntel                 | False         | False         | False         | False         | False         | False     |
| isX86_64V2              | False         | False         | False         | True          | True          | False     |
| isX86_64V3              | False         | False         | False         | False         | False         | False     |
| isX86_64V4              | False         | False         | False         | False         | False         | False     |

Intel Cpus
| CPU TYPE/function name | Broadwell-v1 | Denverton-v1 | Conroe-v1 | Skylake-Server-v1 |
| ----------------------- | ------------ | ------------ | --------- | ----------------- |
| isAMD                   | False        | False        | False     | False             |
| IsIntel                 | True         | True         | True      | True              |
| isX86_64V2              | True         | True         | False     | True              |
| isX86_64V3              | True         | False        | False     | True              |
| isX86_64V4              | False        | False        | False     | True              |

X86_64v2
| CPU TYPE/FLAG | EPYC-Genoa-v1 | EPYC-Rome-v1 | Opteron_G4-v1 | phenom-v1 | Skylake-Server-v1 | Broadwell-v1 | Denverton-v1 | Conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| cx16          | True          | True         | True          | False     | True              | True         | True         | False     |
| lahf_lm       | True          | True         | True          | False     | True              | True         | True         | False     |
| popcnt        | True          | True         | True          | False     | True              | True         | True         | False     |
| sse4_1        | True          | True         | True          | False     | True              | True         | True         | False     |
| sse4_2        | True          | True         | True          | False     | True              | True         | True         | False     |
| ssse3         | True          | True         | True          | False     | True              | True         | True         | False     |

X86_64v3

| CPU TYPE/FLAG | EPYC-Genoa-v1 | EPYC-Rome-v1 | Opteron_G4-v1 | phenom-v1 | Skylake-Server-v1 | Broadwell-v1 | Denverton-v1 | Conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| avx           | True          | True         | False         | False     | True              | True         | False        | False     |
| avx2          | True          | True         | False         | False     | True              | True         | False        | False     |
| bmi1          | True          | True         | False         | False     | True              | True         | False        | False     |
| bmi2          | True          | True         | False         | False     | True              | True         | False        | False     |
| f16c          | True          | True         | False         | False     | True              | True         | False        | False     |
| fma           | True          | True         | False         | False     | True              | True         | False        | False     |
| abm           | True          | True         | False         | False     | True              | True         | False        | False     |
| movbe         | True          | True         | False         | False     | True              | True         | False        | False     |
| xsave         | True          | True         | False         | False     | True              | True         | False        | False     |

X86_64v4
| CPU TYPE/FLAG | EPYC-Genoa-v1 | EPYC-Rome-v1 | Opteron_G4-v1 | phenom-v1 | Skylake-Server-v1 | Broadwell-v1 | Denverton-v1 | Conroe-v1 |
| ------------- | ------------- | ------------ | ------------- | --------- | ----------------- | ------------ | ------------ | --------- |
| avx512f       | True          | False        | False         | False     | True              | False        | False        | False     |
| avx512bw      | True          | False        | False         | False     | True              | False        | False        | False     |
| avx512cd      | True          | False        | False         | False     | True              | False        | False        | False     |
| avx512dq      | True          | False        | False         | False     | True              | False        | False        | False     |
| avx512vl      | True          | False        | False         | False     | True              | False

### Utility Function Test Matrix TBD

### The C Interfaces TBD

## How to Run the Tests

`ctest` runs all the tests by default along with other tests. If running independently, call `core_CpuidTests`. It has to be done from the build folder as `./Release/core_CpuidTest`.

**Note to Developer:**
The test creates the X86Cpu test suites with all the unit test cases for functions listed in the scope of the test. These tests are disabled by default because it doesn't make sense to run them on the host whose architecture might be unknown. Use the `--gtest_also_run_disabled_tests` flag if these need to be enabled on the host.

A Python script is written to invoke tests from the X86Cpu test suite to run on different QEMU emulated CPUs. The script is placed in the same folder as the test sources. The script can be run as follows:

```bash
python ../Library/Tests/Cpuid/CpuidTest.py EPYC X86Cpuid.DISABLED_isAMD

```

Then there is QemuTest suite, that uses the python script, run_cmd function results, to write tests for different emulated cpus listed above.
