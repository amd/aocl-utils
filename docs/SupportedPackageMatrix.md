# AOCL-Utils Supported Packages Matrix

One of the compilers and generators on the platform must be installed to build the library, along with at least the minimum version of CMake. The table below lists the supported compilers and generators for the library.

**Note:** Use the same compiler to build both AOCL-Utils and any application that relies on it.

## Supported Compilers and Generators

**Note:** Other versions may or may not work.

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

| Package  | Version   |  Notes |
| -------  | --------  |  ----- |
| python   | 3.9 -3.13 |        |
| qemu-user|           |        |

**Note:** Installation of required packages may vary by platform. In some cases, one might need to explicitly link dependency libraries such as -lstdc++ or -lpthread, especially on older distributions that do not link them automatically.

## Checklist

1. **While linking to the new combined binary**
   1. The combined binary is a combination of all the modules in utils. It is going to be larger in size.
       1. Will have to link explicitly to libstdc++ libpthread,  -ldl -lutil explicitly.[section 2 for more details]
       2. Make sure that headers used are corresponding to the compiler being used for compilation and uses the same glibc as the binary.
   2. Backward compatibility issues.
       1. Libraries linking with libaoclutils are good here, but if somebody moved to au_cpuid, the backward compatibility breaks.
       2. Use version.txt document to find the version and link to libaoclutils if version is 4.2 otherwise libau_cpuid

    2. **Common issues and solutions.**

    | Sl no | Issue                                                                         | Root cause                                                            | Solution                                                                                           |
    |-------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------|----------------------------------------------------------------------------------------------------|
    | 1     | Could NOT find Python3 (missing: Python3_INCLUDE_DIRS Python3_LIBRARIES)      | Not python3 or python3-devel package not present.                     | Install python3 and devel package.                                                                 |
    |       |                                                                               | Older cmake(3.22, 3.23 and 3.24) unable to find python3 library.       | PYTHON_LIBRARIES = path to the python library PYTHON_INCLUDE_PATH = path to where Python.h is found PYTHON_EXECUTABLE = path to the Python interpreter |
    | 2     | failing cmake --build command with python related issues.                     | Older python not able to find stdlibs                                  | Compile utils with -DCMAKE_CXX_STANDARD_LIBRARIES="-ldl -lutil"                                    |
    | 3     | Missing symbol from aoclutils                                                 | Not able to find the library(libaoclutils) in the std path             | Update LD_LIBRARY_PATH on linux update PATH on windows.                                            |
    | 4     | Missing symbols and errors related to types and type_traits                   | unable to link to libstdc++ automatically on older oses.               | DCMAKE_CXX_STANDARD_LIBRARIES="-libstdc++"                                                         |
    |       |                                                                               | < glibc2.34 has libpthread as a separate library.                      | gcc 10 and below or if the compiler is linking to glibc version < 2.34 explicitly link to lpthread while compiling utils. |
    | 5     | Missing symbols related to pthreads                                           | Not able to link to libpthread automatically on older oses.            | DCMAKE_CXX_STANDARD_LIBRARIES="-lpthread"                                                          |

3. **Testing.**
   1. Testing requires qemu-x86_64 (qemu-user package) to be installed to run.  Make sure that this is installed before running the test. Do a clean build after installation.
   2. Cupid test depends on python too. Make sure that python3 and python-dev(el) package is installed.
