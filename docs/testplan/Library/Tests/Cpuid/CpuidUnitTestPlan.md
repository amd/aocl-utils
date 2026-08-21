<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# The CPUID Module Unit Test Plan

## The CPUID Module

The CPUID module provides details about the underlying CPU architecture by reading and interpreting the `cpuid` register of the system.

## Objective

The test plan aims to provide comprehensive coverage for all components in this module.

## Classes of Test Cases

* **QEMU Tests**
  The QEMU CPU emulator is used on Linux to emulate selected CPUs and test both
  C and C++ APIs. The test target remains registered on other platforms, but
  QEMU-backed cases are unavailable there.

* **Mock Tests**
  Raw-CPUID replay tests exercise C++ APIs and utility functions by mocking the
  `cpuid` register call. The folder structure is organized as follows:

```console
simnowdata
└───<folder-per-emulated-cpuid>
    └───<cpuid>
    └───FlagsT.txt
    └───FlagsF.txt
 * The \<cpuid> file consists of cpuid register read corresponding to the particular cpuid for a set of inputs,
   in the form {request} : {response}
 * These files are shared by both qemu tests and mocktests.
 * FlagsT.txt and FlagsF.txt files are shared by all the hasflags tests.
 * Flags in FlagsT.txt marks the set of flags that should be present in the cpu
 * Flags in FlagsF.txt marks the set of flags that should not be present in the cpu.
```

* **Mock-CPUID link-shim tests**
  The Phase-1 link shim builds ordinary C and C++ consumers twice, once against
  the real library and once against the mock. It checks the subset property,
  profile identity, reset, strict-mode behavior, direct linking, Linux
  `LD_PRELOAD`, dispatch-and-execute behavior, and symbol parity when `nm` is
  available. Run it with:

  ```console
  ctest --test-dir build -R MockCpuid --output-on-failure --no-tests=error
  ```

  The generated table contains 85 named profiles plus common aliases. The
  shim is opt-in and is not installed.

* **Native Tests**
  A few native test cases check edge cases and negative scenarios. More tests
  will be added for full coverage.

### List of C++ APIs

* `X86Cpu::isAMD`
* `X86Cpu::isIntel`
* `X86Cpu::isX86_64v2`
* `X86Cpu::isX86_64v3`
* `X86Cpu::isX86_64v4`
* `X86Cpu::hasFlag`
* `X86Cpu::hasFlags`
* `X86Cpu::hasAllFlags`
* `X86Cpu::isUarch`
* `X86Cpu::getUarch`
* `X86Cpu::isZenFamily`
* `X86Cpu::getVendorInfo`
* `X86Cpu::update`

### List of C APIs

* `au_cpuid_is_amd`
* `au_cpuid_get_vendor`
* `au_cpuid_has_flag` (deprecated)
* `au_cpuid_has_flags_all`
* `au_cpuid_has_flags_any`
* `au_cpuid_has_flags`
* `au_cpuid_arch_is_zen`
* `au_cpuid_arch_is_zenplus` (compatibility/cumulative alias)
* `au_cpuid_arch_is_zen2`
* `au_cpuid_arch_is_zen3`
* `au_cpuid_arch_is_zen4`
* `au_cpuid_arch_is_zen5`
* `au_cpuid_arch_is_zen6`
* `au_cpuid_arch_is_x86_64v2`
* `au_cpuid_arch_is_x86_64v3`
* `au_cpuid_arch_is_x86_64v4`
* `au_cpuid_arch_is_zen_family`
* `au_cpuid_is_hybrid`
* `au_cpuid_get_core_type`

Representative QEMU profiles currently exercised by the active suite include:

* `denverton-v1`
* `conroe-v1`
* `EPYC-Milan-v1` and `EPYC-Milan-v2`
* `EPYC-Rome-v1` through `EPYC-Rome-v4`
* `EPYC-v1` through `EPYC-v4`
* `Opteron_G1-v1` through `Opteron_G5-v1`

The Broadwell and Skylake-Server entries remain documented in the source as
future/disabled QEMU cases because the corresponding test entries are
commented out. Genoa is validated through raw-data mock tests rather than the
current QEMU model.

These C and C++ APIs are tested using QEMU test cases and mock tests. In addition, the utility functions that directly use the `cpuid` register to fill data in various data structures used by the above functions are tested with mocked `cpuid` data.

### List of Utility Functions

* `getmfginfo`
* `getfamily`
* `getmodel`
* `getstepping`
* `hasflag`
* `updatecacheview`
* `updatecacheinfo`

## The Test Matrix for C/C++ APIs

The raw replay matrix below retains representative legacy CPU profiles. The
current C++ mock data also covers Zen5 and Zen6 profiles such as Turin, Strix,
and Venice; see `Library/Tests/Cpuid/Mock/MockTest.hh` for the authoritative
profile list and expectations.

### CPU Features

#### AMD CPUs

| cpu type/function name | epyc-genoa-v1 | epyc-milan-v1 | epyc-milan-v2 | epyc-rome-v1 | epyc-rome-v2 |
|------------------------|---------------|---------------|---------------|--------------|--------------|
|isAMD                   |true           |true           |true           |true          |true          |
|isIntel                 |false          |false          |false          |false         |false         |
|isX86_64v2              |true           |true           |true           |true          |true          |
|isX86_64v3              |true           |true           |true           |true          |true          |
|isX86_64v4              |true           |false          |false          |false         |false         |
|getUarch                |zen4           |zen3           |zen3           |zen2          |zen2          |

amd cpus (continued):

| cpu type/function name | epyc-rome-v3 | epyc-rome-v4 | epyc-v1 | epyc-v2 | epyc-v3 | epyc-v4 | opteron_g1-v1  |
|------------------------|--------------|--------------|---------|---------|---------|---------|----------------|
|isAMD                   |true          |true          |true     |true     |true     |true     | true           |
|isIntel                 |false         |false         |false    |false    |false    |false    | false          |
|isX86_64v2              |true          |true          |true     |true     |true     |true     | false          |
|isX86_64v3              |true          |true          |true     |true     |true     |true     | false          |
|isX86_64v4              |false         |false         |false    |false    |false    |false    | false          |
|getUarch                |zen2          |zen2          |zen      |zen      |zen      |zen      | unknown        |

amd cpus (continued):

| cpu type/function name | opteron_g2-v1 | opteron_g3-v1 | opteron_g4-v1 | opteron_g5-v1 | phenom-v1 |
|------------------------|---------------|---------------|---------------|---------------|-----------|
|isAMD                   |true           |true           |true           |true           |true       |
|isIntel                 |false          |false          |false          |false          |false      |
|isX86_64v2              |false          |false          |true           |true           |false      |
|isX86_64v3              |false          |false          |false          |false          |false      |
|isX86_64v4              |false          |false          |false          |false          |false      |
|getUarch                |unknown        |unknown        |unknown        |unknown        |unknown    |

#### Intel CPUs

| cpu type/function name | broadwell-v1 | denverton-v1 | conroe-v1 | skylake-server-v1 |
|------------------------|--------------|--------------|-----------|-------------------|
|isAMD                   |false         |false         |false      |false              |
|isIntel                 |true          |true          |true       |true               |
|isX86_64v2              |true          |true          |false      |true               |
|isX86_64v3              |true          |false         |false      |true               |
|isX86_64v4              |false         |false         |false      |true               |
|getUarch                |unknown       |unknown       |unknown    |unknown            |

### ISA Features

#### x86_64v2

| cpu type/flag | epyc-genoa-v1 | epyc-rome-v1 | opteron_g4-v1 | phenom-v1 | skylake-server-v1 | broadwell-v1 | denverton-v1 | conroe-v1 |
|---------------|---------------|--------------|---------------|-----------|-------------------|--------------|--------------|-----------|
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
  | EPYC-Genoa-v1     |  Amd  | Family19h | 0x11  |0x0         | zen4    |
  | EPYC-Milan-v1     |  Amd  | Family19h | 0x01  |0x1         | zen3    |
  | EPYC-Milan-v2     |  Amd  | Family19h | 0x01  |0x1         | zen3    |
  | EPYC-Rome-v1      |  Amd  | Family17h | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v2      |  Amd  | Family17h | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v3      |  Amd  | Family17h | 0x31  |0x0         | zen2    |
  | EPYC-Rome-v4      |  Amd  | Family17h | 0x31  |0x0         | zen2    |
  | EPYC-v1           |  Amd  | Family17h | 0x01  |0x2         | zen     |
  | EPYC-v2           |  Amd  | Family17h | 0x01  |0x2         | zen     |
  | EPYC-v3           |  Amd  | Family17h | 0x01  |0x2         | zen     |
  | EPYC-v4           |  Amd  | Family17h | 0x01  |0x2         | zen     |
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

`ctest` runs all the tests by default along with other tests. if running independently, call `aoclutils_CpuidTest`. it has to be done from the build folder as `./Release/aoclutils_CpuidTest`.

**Note to developer:**
the test creates the x86cpu test suites with all the unit test cases for functions listed in the scope of the test. these tests are disabled by default because it doesn't make sense to run them on the host whose architecture might be unknown. use the `--gtest_also_run_disabled_tests` flag if these need to be enabled on the host.

a python script is written to invoke tests in the x86cpu test suite to run on different qemu emulated cpus. the script is placed in the same folder as the test sources. the script can be run as follows:

```bash
# Assuming the PWD is the build directory
python ../Library/Tests/Cpuid/CpuidTest.py EPYC-Genoa-v1 X86Cpuid.DISABLED_isAMD

```

the qemutest suites run by invoking the above python script to run on different emulated cpus.
