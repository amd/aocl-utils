<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# AOCL-Utils Supported Packages Matrix

One of the compilers and generators on the platform must be installed to build the library, along with at least the minimum version of CMake. The table below lists the supported compilers and generators for the library.

**Note:** Use the same compiler to build both AOCL-Utils and any application that relies on it.

## Supported Compilers and Generators

The enforced minimums on `amd-main` are:

| Toolchain | Minimum | Active build-sanity matrix |
|-----------|---------|-----------------------------|
| CMake | 3.26 | 3.26–3.31 |
| GCC/G++ | 8 | 9–14 |
| Clang/Clang++ | 12 | 14–17 |
| MSVC | 19.29 | Windows direct-build validation |

Other versions may work but are not covered by these project gates.

### Standard Libraries

The library uses the standard C++ libraries and the platform threads
implementation. Tests additionally require GoogleTest, Python 3, and
`qemu-x86_64` for Linux emulation cases.

#### au_cpuid

| Package    |Notes |
| -----------|----- |
| libstdc++  |      |
| libc++     |      |

#### au_core

| Package    |Notes |
| -----------|----- |
| libstdc++  |      |
| libc++     |      |
| libpthread |      |

#### tests

| Package   | Requirement | Notes |
| --------- | ----------- | ----- |
| Python 3  | Interpreter + Development components | Required by CPUID tests and Mock-CPUID profile generation |
| qemu-user | `qemu-x86_64` | Required for Linux emulation cases |

**Note:** Installation of required packages may vary by platform. In some cases, one might need to explicitly link dependency libraries such as -lstdc++ or -lpthread, especially on older distributions that do not link them automatically.

### C++17 filesystem on GCC 8.x

When `AU_BUILD_TESTS=ON` on Linux, the CPUID QEMU test uses C++17
`std::filesystem`. GCC/G++ 8.x provides its filesystem implementation in the
separate `libstdc++fs` library; it moved into the main `libstdc++` in GCC 9.
The same applies to Clang/Clang++ (including AOCC) when the compiler selects a
GCC 8.x toolchain. Add the library to `CMAKE_CXX_STANDARD_LIBRARIES` when
configuring the test build:

```console
cmake -S . -B default -DCMAKE_BUILD_TYPE=Release \
    -DAU_BUILD_TESTS=ON \
    -DCMAKE_CXX_STANDARD_LIBRARIES="-lstdc++fs"
```

Append `-lstdc++fs` to any existing value, such as `-lpthread`, `-ldl`, or
`-lutil`. Modern libstdc++ versions generally do not need this flag; if a
version still ships a compatibility `libstdc++fs` archive, passing it is
harmless. A libc++-only toolchain does not provide `libstdc++fs`, so passing
this flag there fails.

## Checklist

1. **While linking to the new combined binary**
   1. The combined binary is a combination of all the modules in utils. It is going to be larger in size.
       1. Will have to link explicitly to libstdc++ libpthread,  -ldl -lutil explicitly.[section 2 for more details]
       2. Make sure that headers used are corresponding to the compiler being used for compilation and uses the same glibc as the binary.
   2. Backward compatibility.
       1. Prefer the installed CMake package targets and the umbrella
          `AoclUtils::aoclutils` target for new consumers.
       2. Keep headers and libraries from the same AOCL-Utils installation;
          do not select a library solely from the version string.

    2. **Common issues and solutions.**

    | Sl no | Issue                                                                         | Root cause                                                            | Solution                                                                                           |
    |-------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------|----------------------------------------------------------------------------------------------------|
    | 1     | Could NOT find Python3 (missing: Python3_INCLUDE_DIRS Python3_LIBRARIES)      | Not python3 or python3-devel package not present.                     | Install python3 and devel package.                                                                 |
    |       |                                                                               | CMake must meet the project minimum of 3.26.                       | Provide a Python 3 interpreter, headers, and development library so `find_package(Python3 COMPONENTS Interpreter Development REQUIRED)` succeeds. |
    | 2     | failing cmake --build command with python related issues.                     | Older python not able to find stdlibs                                  | Compile utils with -DCMAKE_CXX_STANDARD_LIBRARIES="-ldl -lutil"                                    |
    | 3     | Missing symbol from aoclutils                                                 | Not able to find the library(libaoclutils) in the std path             | Update LD_LIBRARY_PATH on linux update PATH on windows.                                            |
    | 4     | Missing symbols and errors related to types and type_traits                   | unable to link to libstdc++ automatically on older oses.               | `-DCMAKE_CXX_STANDARD_LIBRARIES="-lstdc++"`                                                         |
    |       |                                                                               | < glibc2.34 has libpthread as a separate library.                      | gcc 10 and below or if the compiler is linking to glibc version < 2.34 explicitly link to lpthread while compiling utils. |
    | 5     | Missing symbols related to pthreads                                           | Not able to link to libpthread automatically on older oses.            | DCMAKE_CXX_STANDARD_LIBRARIES="-lpthread"                                                          |

3. **Testing.**
   1. Testing requires qemu-x86_64 (qemu-user package) to be installed to run.  Make sure that this is installed before running the test. Do a clean build after installation.
   2. CPUID tests depend on Python 3 development headers and libraries. Make
      sure the interpreter and development package are installed.
