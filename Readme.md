# AOCL-UTILS

AOCL-Utils is an effort to extract common functionalities across
libraries. Its main features include:

- Au_cpuid
  - Core details
  - Available flags
  - Usable ISA

- Au_core_module
  - Thread pinning
  - Status
  - Logger
  - RNG

**NOTE:**
This library detects only AMD "Zen" CPUs. There are no plans to support other x86
implementations. Some utilities may fail or behave unexpectedly on older AMD
architectures.

Core module is internal to AOCL-Utils. To use its features, link to libaoclutils, which
combines all available utility modules.

## Table of Contents

- [AOCL-UTILS](#aocl-utils)
  - [Table of Contents](#table-of-contents)
  - [Project structure](#project-structure)
  - [BUILD AND INSTALL](#build-and-install)
    - [Dependencies](#dependencies)
    - [Getting started](#getting-started)
      - [Checkout the latest code](#checkout-the-latest-code)
      - [Configure](#configure)
      - [Build](#build)
      - [Install](#install)
  - [Testing](#testing)
  - [Examples](#examples)
  - [List of build options](#list-of-build-options)
  - [List of functionalities provided by each utils modules](#list-of-functionalities-provided-by-each-utils-modules)
    - [au\_core (internal)](#au_core-internal)
      - [Current API Stack(Core)](#current-api-stack-core)
    - [au\_cpuid](#au_cpuid)
      - [Current API Stack(Cpuid)](#current-api-stack-cpuid)
    - [aoclutils](#aoclutils)
  - [Integration with other projects](#integration-with-other-projects)
    - [CMAKE](#cmake)
    - [Make](#make)

## Project structure

The project is structured as follows:

- `Library`: This directory contains the source code for the project.

- `SDK`: The release folder.

- `Tests`: This directory contains the necessary unit tests for the project.

- `Tools`: The necessary tools to work with the project.

- `scripts`: Utility scripts to work with the project.

## BUILD AND INSTALL

### Dependencies

Refer [supported package matrix document](https://docs.amd.com/r/en-US/63866-AOCL-utils/Supported-package-Matrix)
(supported_package_matrix.md file)

### Getting started

The same commands apply to both Linux and Windows. Only the environment setup differs.
The default compiler and generator are the platform defaults.

For a specific compiler and generator:
```console
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -G "Unix Makefiles" ..
```
**Note:** Replace clang with the chosen compiler and "Unix Makefiles" with the chosen generator.

#### Checkout the latest code

```console
    git clone
    cd aocl-utils
```

#### Configure

```console
    cmake -B  default -DCMAKE_INSTALL_PREFIX=install_dir
```

#### Build

```console
    cmake --build default --config release -j
```

#### Install

```console
    cmake --install default --config release
```
This command places:

1. Header files in `<Install Path>/include`
2. Static and dynamic libraries for au_core and au_cpuid

**Note:**
   1. This command creates lib/lib64 directory for the binaries. To have custom library path, use CMAKE_INSTALL_LIBDIR.
   2. Rightly update the include path and library path in the project to link with the installed libraries. or use LD_LIBRARY_PATH to point to the installed library path.(PATH environment variable in windows)
   3. Refer to the [API documentation](https://docs.amd.com/r/en-US/63866-AOCL-utils) and examples in the Example folder to understand how to link and use the modules.

**Important:**
1. Most CPUID APIs (and their headers) introduced in 4.2 are deprecated; they will be
   removed in a future release. See the API documentation for the newer APIs.
2. Old APIs can be enabled with `AU_ENABLE_OLD_API=ON` during the build, otherwise deprecated
   warnings will be shown.
3. The aoclutils module combines au_core and au_cpuid.
4. The aoclutils module is the default module for all functionalities.

## Testing

Build with `AU_BUILD_TESTS=ON` to enable tests:
```console
ctest -C Release
```
QEMU (`qemu-x86_64`) is required for running tests on Linux distributions. On Windows, tests
are disabled because QEMU-user is unavailable there.

```console
    sudo apt-get install qemu-user # For Ubuntu
    sudo dnf install qemu-user # For Fedora/RHEL/CentOS
    # qemu tests are disable on windows as qemu-user is not available on windows
```

## Examples

Build with `AU_BUILD_EXAMPLES=ON` to enable examples:
```console
cmake -B build -DAU_BUILD_EXAMPLES=ON -G Ninja
cmake --build build --config Release
```

The binaries are in the default/release folder. Refer to the SDK/Examples folder Readme.md for details on out of tree compilation.

## List of build options

```console
Build Flags                              Description                  Default   Alternate
----------------------------------------------------------------------------------------
AU_BUILD_DOCS                            Generate Docs during build   OFF       ON
AU_BUILD_EXAMPLES                        Build examples               OFF       ON
AU_BUILD_TESTS                           Build tests                  OFF       ON
AU_BUILD_TYPE                            Build type                   Release   Debug, Developer
AU_ENABLE_OLD_API                        Enable OLD alci_* APIs       OFF       ON
AU_BUILD_SHARED_LIBS                     Build shared libraries       ON        OFF
AU_BUILD_STATIC_LIBS                     Build static libraries       ON        OFF
AU_BUILD_WITH_ASAN                       Enable ASAN options          OFF       ON
AU_BUILD_WITH_TSAN                       Enable TSAN options          OFF       ON
AU_BUILD_WITH_MEMSAN                     Enable MEMSAN options        OFF       ON
```

## List of functionalities provided by each utils modules

### au_core (internal)

| Functionality  | Headerfiles(C)            | Headerfiles(C++)       |
| -------------- | ------------------------- | ---------------------- |
| thread pinning | Capi/au/threadpinning.h   | Au/ThreadPinning.hh    |
| status         |                           | Au/Status.hh           |
| logger         | Capi/au/logger.h          | Au/Logger/Logger.hh    |

#### Current API Stack(Core)

1. ThreadPinning

<img src="TP_API_STACK.png" alt="Current ThreadPinning API stack" width="400"/>

### au_cpuid

| Functionality                | Headerfiles(C)        | Headerfiles(C++)         |
|------------------------------|-----------------------|--------------------------|
| cpu architecture detection   | Capi/au/cpuid.h       | Au/Cpuid/X86Cpu.hh       |
| cpuid feature flag detection | Capi/au/cpuid.h       | Au/Cpuid/X86Cpu.hh       |
| Deprecated APIs              | Bcl/alci/arch.h       | Bcl/alci/cxx/cpu.hh      |

#### Current API Stack(Cpuid)

<img src="CPUID_API_STACK.png" alt="Current Cpuid API stack" width="500"/>

**Note**

1. The APIs in the grey box are deprecated and will be removed in the future release.
2. X86Cpu.hh(CPP)/cpuid.h(C) is the new header file that contains the new APIs.

### aoclutils

- All features of au_cpuid and au_core combined.

The C headers are in the \<installpath\>/include/Capi folder and the C++ headers are in the include/Au folder.
Deprecated APIs are in the include/alci folder.
**Note: Refer to [API documentation](https://docs.amd.com/r/en-US/63866-AOCL-utils/Cpuid-API-Reference) and Examples in Examples folder to understand how to link and use the modules.**

## Integration with other projects

Following are the build systems to integrate in library/application with AOCL-Utils:

### CMAKE

In the CMake file, use the following:

```console
TARGET_INCLUDE_DIRECTORIES() – path of libaoclutils include directory

TARGET_LINK_LIBRARIES() – path to link libaoclutils binaries
```

### Make

In the compiler flags of Make file, use the following:

```console
“–I” - path of libaoclutils include directory

“-l, -L” - path to link libaoclutils binaries
```
