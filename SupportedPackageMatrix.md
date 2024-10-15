# AOCL-Utils Supported Packages Matrix

One of the compilers and generators on the platform has to be installed to build the library, along with atleast the minimum version of cmake. The following table lists the supported compilers and generators for the library.

**Note** Use same compiler to build utils and the application that uses utils.

## Supported Compilers and Generators

**Note** Other versions may or may not work.

## Windows

| Package | Version        | Notes |
| ------- | ---------------| ----- |
| clang   | 12.0 - 18.0    | Pass -A x64 during configure|
| msvc    | 2019 and 2022  |       |
| cmake   | 3.22 - 3.30    |       |
| ninja   | 1.10           |       |
| visualstudio | 2019 and 2022 |       |

## Linux

| Package | Version                        | Notes  |
| ------- | ------------------------------ | -----  |
| clang   | 12.0 - 18.0                    |        |
| aocc    | 4.2(clang-16) and 5.0(clang-17)| Download from <https://www.amd.com/en/developer/aocc.html>|
| gcc     | 8.5 - 13.1                     | 8.5 requires "-lstdc++fs"  to be supplied on commandline.       |
| cmake   | 3.22 - 3.30                    |        |
| ninja   | 1.10                           |        |
| clang-tidy | 12.0 -17.0                  |        |

### Standard Libraries

The library uses the standard C++ libraries and does not have any external dependencies. The library is tested with the following standard libraries:

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

| Package  | Version  |   Notes |
| -------  | -------  |   ----- |
| python   | 3.9 -3.12|         |
| qemu-user|          |         |

**Note: One might have to install the required packages based on the platform.  and explicitly link the depency libraries.**

## Checklist

1. **While linking to the new combined binary**
   1. The combined binary is a combination of all the modules in utils. It is going to be larger in size.
       1. Will have to link explicitly to libstdc++ libpthread,  -ldl -lutil explicitly.[section 2 for more details]
       2. Make sure that headers used are corresponding to the compiler being used for compilation and uses the same glibc as the binary.
   2. Backward compatibility issues.
       1. Libraries linking with libaoclutils are good here, but if somebody moved to au_cpuid, the backward compatibility breaks.
       2. Use version.txt document to find the version and link to libaoclutils if version is 4.2 otherwise libau_cpuid

2. **Common issues and solutions.**

|     Sl no    |     Issue                                                                          |     Root cause                                                            |  Solution                                                                            |
|--------------|------------------------------------------------------------------------------------|---------------------------------------------------------------------------|--------------------------------------------------------------------------------------|
|     1        |       Could NOT find Python3 (missing:   Python3_INCLUDE_DIRS Python3_LIBRARIES    |     Not python3   or python3-devel package not present.                   |     Install   python3 and devel package.                                                                                                                                         |
|              |                                                                                    |     Older   cmake(3.22, 3.23 and 3.24) unable to find python3 library.    |     PYTHON_LIBRARIES     = path to the python library         PYTHON_INCLUDE_PATH  = path to where Python.h is found     PYTHON_EXECUTABLE   = path to the Python interpreter    |
|     2        |     failing cmake   --build command with python related issues.                    |     Older python not able to find stdlibs                                 |     Compile utils   with -DCMAKE_CXX_STANDARD_LIBRARIES="-ldl -lutil"                                                                                                            |
|     3        |     Missing   symbol   from aoclutils                                              |     Not able to   find the library(libaoclutils) in the std path          |     Update   LD_LIBRARY_PATH on linux update PATH on windows.                                                                                                                    |
|     4        |     Missing   symbols and errors related to types and type_traits                  |     unable to   link to libstdc++ automatically on older oses.            |     DCMAKE_CXX_STANDARD_LIBRARIES="-libstdc++"                                                                                                                                   |
|              |                                                                                    |     <   glibc2.34 has libpthread as a separate library.                   |     gcc 10 and   below or if the compiler is linking to glibc version < 2.34 explicitly   link to lpthread while compiling utils.                                                |
|   5          |    Missing  symbols related to pthreads                                           |     Not able to   link to libpthread automatically on older oses.          |   DCMAKE_CXX_STANDARD_LIBRARIES="-lpthread"                                                                                                                                   |

3. **Testing.**
   1. Testing requires qemu-x86_64 (qemu-user package) to be installed to run.  Make sure that this is installed before running the test. Do a clean build after installation.
   2. Cupid test depends on python too. Make sure that python3 and python-dev(el) package is installed.
