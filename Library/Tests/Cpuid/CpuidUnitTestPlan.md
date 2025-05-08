# The CPUID Module Unit Test Plan

## The CPUID Module

The CPUID module provides details about the underlying CPU architecture by reading and interpreting the `cpuid` register of the system.

## Objective

The test plan aims to provide comprehensive coverage for all components in this module.

## Classes of Test Cases

* **QEMU Tests**
  The QEMU CPU emulator is used to emulate CPUs to test both C and C++ APIs. Native tests corresponding to this are disabled by default because they do not make sense to run natively.

* **Mock Tests**
  Mock tests are used to test C++ APIs and utility functions by mocking the `cpuid` register call. The folder structure is organized as follows:

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

* **Native Tests**
  A few native test cases are written to check edge cases and negative scenarios. More tests will be added for full coverage.

### List of C++ APIs

* `isamd`
* `isintel`
* `isx86v2`
* `isx86v3`
* `isx86v4`
* `hasflag`
* `isuarch`
* `getuarch`
* `getvendorinfo`

### List of C APIs

* `au_cpuid_is_amd`
* `au_cupid_has_flag`
* `au_cpuid_get_vendor`
* `au_cpuid_arch_is_zen`
* `au_cpuid_arch_is_zenplus`
* `au_cpuid_arch_is_zen2`
* `au_cpuid_arch_is_zen3`
* `au_cpuid_arch_is_zen4`
* `au_cpuid_arch_is_zen5`

For all AMD CPU modules provided by QEMU and below Intel CPUs:

* `broadwell-v1`
* `denverton-v1`
* `conroe-v1`
* `skylake-server-v1`

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

### CPU Features

#### AMD CPUs

| cpu type/function name | epyc-genoa-v1 | epyc-milan-v1 | epyc-milan-v2 | epyc-rome-v1 | epyc-rome-v2 |
|------------------------|---------------|---------------|---------------|--------------|--------------|
|isamd                   |true           |true           |true           |true          |true          |
|isintel                 |false          |false          |false          |false         |false         |
|isx86_64v2              |true           |true           |true           |true          |true          |
|isx86_64v3              |true           |true           |true           |true          |true          |
|isx86_64v4              |true           |false          |false          |false         |false         |
|getuarch                |zen4           |zen3           |zen3           |zen2          |zen2          |

amd cpus (continued):

| cpu type/function name | epyc-rome-v3 | epyc-rome-v4 | epyc-v1 | epyc-v2 | epyc-v3 | epyc-v4 | opteron_g1-v1  |
|------------------------|--------------|--------------|---------|---------|---------|---------|----------------|
|isamd                   |true          |true          |true     |true     |true     |true     | true           |
|isintel                 |false         |false         |false    |false    |false    |false    | false          |
|isx86_64v2              |true          |true          |true     |true     |true     |true     | false          |
|isx86_64v3              |true          |true          |true     |true     |true     |true     | false          |
|isx86_64v4              |false         |false         |false    |false    |false    |false    | false          |
|getUarch                |zen2          |zen2          |zen      |zen      |zen      |zen      | unknown        |

amd cpus (continued):

| cpu type/function name | opteron_g2-v1 | opteron_g3-v1 | opteron_g4-v1 | opteron_g5-v1 | phenom-v1 |
|------------------------|---------------|---------------|---------------|---------------|-----------|
|isamd                   |true           |true           |true           |true           |true       |
|isintel                 |false          |false          |false          |false          |false      |
|isx86_64v2              |false          |false          |true           |true           |false      |
|isx86_64v3              |false          |false          |false          |false          |false      |
|isx86_64v4              |false          |false          |false          |false          |false      |
|getUarch                |unknown        |unknown        |unknown        |unknown        |unknown    |

#### Intel CPUs

| cpu type/function name | broadwell-v1 | denverton-v1 | conroe-v1 | skylake-server-v1 |
|------------------------|--------------|--------------|-----------|-------------------|
|isamd                   |false         |false         |false      |false              |
|isintel                 |true          |true          |true       |true               |
|isx86_64v2              |true          |true          |false      |true               |
|isx86_64v3              |true          |false         |false      |true               |
|isx86_64v4              |false         |false         |false      |true               |
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

`ctest` runs all the tests by default along with other tests. if running independently, call `aoclutils_CpuidTest`. it has to be done from the build folder as `./Release/aoclutils_CpuidTest`.

**Note to developer:**
the test creates the x86cpu test suites with all the unit test cases for functions listed in the scope of the test. these tests are disabled by default because it doesn't make sense to run them on the host whose architecture might be unknown. use the `--gtest_also_run_disabled_tests` flag if these need to be enabled on the host.

a python script is written to invoke tests in the x86cpu test suite to run on different qemu emulated cpus. the script is placed in the same folder as the test sources. the script can be run as follows:

```bash
# Assuming the PWD is the build directory
python ../Library/Tests/Cpuid/CpuidTest.py EPYC-Genoa-v1 X86Cpuid.DISABLED_isAMD

```

the qemutest suites run by invoking the above python script to run on different emulated cpus.
