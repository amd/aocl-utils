# AOCL-UTILS

  An effort to factor out common functionality across libraries.

# Project structure

The project is structured as follows:

* `Library`: This directory contains the source code for the project.
* `SDK`: The release folder.
* `Tests`: This directory contains the necessary unit tests for the project.
* `Tools`: The necessary tools to work with the project.

# Dependencies

The project depends on the following libraries:

* CMake 3.25
* Gcc-13 or Clang-15
* GoogleTest

# Getting started

## Build

To build the project, run the following command:

```
    mkdir build
    cd build
    cmake ..
    cmake build .
```
## Install

To install the library, run the following command:

```
    cmake  -DCMAKE_INSTALL_PREFIX=<Install Path> ..
    make
    make install
```
## Testing
Note: To build with tests enabled use

```
    cmake -DAU_BUILD_TESTS=TRUE -DAU_CMAKE_VERBOSE=TRUE ..
```

To run the unit tests, run the following command:

```
    ctest
```
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
