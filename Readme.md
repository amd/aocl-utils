# AOCL-UTILS

  An effort to factor out common functionality across libraries.

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
      - [List of build options](#list-of-build-options)
      - [List of functionalities provided by each utils modules](#list-of-functionalities-provided-by-each-utils-modules)
        - [au\_core](#au_core)
        - [au\_cpuid](#au_cpuid)

## Project structure

The project is structured as follows:

- `Library`: This directory contains the source code for the project.

- `SDK`: The release folder.

- `Tests`: This directory contains the necessary unit tests for the project.

- `Tools`: The necessary tools to work with the project.

### BUILD AND INSTALL

#### Dependencies

Refer [supported package matrix document](./supported_package_matrix.md)

#### Getting started

Same commands can be used on both Linux and Windows. The only difference is the environment setup. The default compiler and generator used will be  the platform defaults.

For specific compiler and generator, use the following command:

```console
    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -G "Unix Makefiles" ..
```

**Note: Replace the compiler(clang) and generator(Unix Makefiles) with the required one.**

Refer [supported package matrix document](./supported_package_matrix.md) for the supported compiler and generator.

##### Checkout the latest code

```console
    git clone
    cd aocl-utils
```

##### Configure

```console
    cmake -B  default -DCMAKE_INSTALL_PREFIX=install_dir
```

##### Build

```console
    cmake --build default --config release -j
```

##### Install

```console
    cmake --install default --config release
```

This command creates

1. The necessary header files in the \<Install Path\>/includefolder
2. static and dynamic library files corresponding to modules core(au_core) and cpuid(au_cpuid)
    Link with these libraries based on the functionality required.
**Note:**
   1. This command creates lib/lib64 directory for the binaries. To have custom library path, use CMAKE_INSTALL_LIBDIR.
   2. Rightly update the include path and library path in the project to link with the installed libraries. or use LD_LIBRARY_PATH to point to the installed library path.(PATH environment variable in windows)
   3. Refer to the [API documentation](https://github.amd.com/pages/AOCL/aocl-utils/index.html) and examples in the Example folder to understand how to link and use the modules.


**Important:**

```console
    a binary named aoclutils is present in the lib directory, to facilitate backward compatibility with the 4.2
    version of the library. This binary is a wrapper around the cpuid module. It is recommended to use the cpuid module if you are using only the cpuid functionality.
    If not incase of copying the binaries to a non installed path, copy the cpuid binary to the same path as the aoclutils binary.
```

##### Testing

```console
    ctest -C release
```

build with AU_BUILD_TESTS=ON to run the tests. Below are other available build options.

qemu-x86_64 is a dependency for running tests. Install it using the following command:

```console
    sudo apt-get install qemu-user # For Ubuntu
    sudo dnf install qemu-user # For Fedora/RHEL/CentOS
    # qemu tests are disable on windows as qemu-user is not available on windows
```

#### List of build options

```console
Build FLags                             Description                  Default         Alternate Values
--------------------------------------------------------------------------------------------------
AU_BUILD_DOCS                           Generate Docs during build   OFF            ON
AU_BUILD_EXAMPLES                       Build examples               OFF            ON
AU_BUILD_TESTS                          Build tests                  OFF            ON
AU_BUILD_TYPE                           Build type                   Release        Debug, Developer
AU_ENABLE_OLD_API                       Enable OLD alci_* APIs       OFF            ON                  Use to avoid deprecation warnings
```

#### List of functionalities provided by each utils modules

##### au_core

- thread pinning

##### au_cpuid

- cpu flag detection

- cpu architecture detection.

**Note: Refer to [API documentation](https://github.amd.com/pages/AOCL/aocl-utils/index.html) and Examples in [Example](SDK/Examples/) folder to understand how to link and use the modules.**
