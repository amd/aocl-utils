# AOCL-UTILS

  An effort to factor out common functionality across libraries.

# Project structure

The project is structured as follows:

* `Library`: This directory contains the source code for the project.
* `SDK`: The release folder.
* `Tests`: This directory contains the necessary unit tests for the project.
* `Tools`: The necessary tools to work with the project.
# BUILD AND INSTALL ON LINUX

## Dependencies

The project depends on the following libraries:

* CMake 3.22
* Gcc-9 and above [Appropriate libstdc++-dev version needs to be installed]
*  Clang-12 and above
*  Clang-tidy 12 and above
* GoogleTest

## Getting started

### Build

To build the project, run the following command:

```
    mkdir build
    cd build
    cmake ..
    cmake build .
```
### Install

To install the library, run the following command:

```
    cmake  -DCMAKE_INSTALL_PREFIX=<Install Path> ..
    make
    make install
```
This command creates

1. The necessary header files in the <Install Path>/include
folder
2.  static and dynamic library files corresponding to modules core(au_core) and cpuid(au_cpuid)
    Link with these libraries based on the functionality required.

Note: This command creates lib/lib64 directory for the binaries.
To have custom library path, use CMAKE_INSTALL_LIBDIR
### Testing
Note: To build with tests enabled use

```
    cmake -DAU_BUILD_TESTS=TRUE -DAU_CMAKE_VERBOSE=TRUE ..
```

To run the unit tests, run the following command:

```
    ctest
```
# BUILD AND INSTALL ON WINDOWS

## Dependencies

The project depends on the following libraries:

- MS Visual Studio (2019 or greater)
- Clang 15.0 or above
- Python 3.7 or greater
- Cmake 3.22 or greater
- Git

## Environment Setup:

1. Install visual Studio with workload: *Desktop development with c++*
	- Enable Clang/cl tools(required) & Address Santizer(if require)
2. If using LLVM/Clang as external toolset:
	- Install LLVM
	- Install plugin: *llvm2019.vsix* :https://marketplace.visualstudio.com/items?itemName=MarekAniola.mangh-llvm2019
	- Install VS19 version 16.10

## Windows Build with LLVM/Clang:

Using Powershell:

1. Checkout the latest code.
2. Open the powershell.exe (as administrator)
3. Set path to current working directory/project_source_directory

## Build

`Run from source directory`
```
PS > cmake -A [platform: x86/x64] -B [build_directory] [Enable features] -DCMAKE_BUILD_TYPE=[RELEASE] -G "[generator: Visual Studio 17 2022]" -T [toolset:ClangCl/LLVM]
```

`Powershell`
```
* 1. cmake -A x64 -DCMAKE_BUILD_TYPE=RELEASE -B build -T ClangCl
		`-Build binaries will be written to project_source_directory/build`
* 2. cmake --build .\build --config=release -j
```

## Install

```Powershell
* cmake --install .\build --config=release

```
This command creates

1. The necessary header files in the <Install Path>/include
folder
2.  static and dynamic library files corresponding to modules core(au_core) and cpuid(au_cpuid)
    Link with these libraries based on the functionality required.

Note: This command creates lib/lib64 directory for the binaries.
To have custom library path, use CMAKE_INSTALL_LIBDIR


## List of build options
```
// Generate Docs during build
AU_BUILD_DOCS:BOOL=OFF
--
// Enable examples
AU_BUILD_EXAMPLES:BOOL=FALSE
--
// Build shared libraries
AU_BUILD_SHARED_LIBS:BOOL=TRUE
--
// Enable the tests.
AU_BUILD_TESTS:BOOL=OFF
--
// Set cmake verbosity
AU_CMAKE_VERBOSE:BOOL=ON
--
// Option to Enable BROKEN tests
AU_ENABLE_BROKEN_TESTS:BOOL=OFF
--
// Enable OLD alci_* APIs
AU_ENABLE_OLD_API:BOOL=OFF
--
// Option to Enable SLOW tests
AU_ENABLE_SLOW_TESTS:BOOL=OFF
--
```

## List of functionalities provided by each utils modules

### au_core

* thread pinning

### au_cpuid

* cpu flag detection

* cpu arichitecture detection.

Note: Refer to API Documentation and Examples in Example folder to understand
how to link and use the modules.
