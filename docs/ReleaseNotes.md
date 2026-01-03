Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.

# AOCL-Utils 5.0.1 Release Notes:

AOCL-Utils provides a uniform interface to all the AOCL libraries to access the CPU features for AMD CPUs. This library provides the following features:

* Core details
* Flags available/usable
* ISA available/usable
* Topology about L1/L2/L3 caches

AOCL-Utils is designed for integration with the other AOCL libraries. Each project has it’s own mechanism to identify CPU and provide necessary features such as Dynamic Dispatch. The main purpose of this library is to provide a centralized mechanism to update/validate and provide information to the users.

# Highlights of AOCL-Utils 5.0.1:

* CMake: Added Linux Presets
* Use of MultiThreaded Library in Windows.
* Implement Hardware and System Rng.
* Implement extendable and flexible thread safe logger.
* Support for isZenFamily.
* Implement logger support.
* CMake: Upgrade version to 3.26.
* Few bug fixes.


Please refer AOCL User Guide for supported Operating Systems and Compilers.

The package contains AOCL-Utils Library binaries which includes optimizations for the AMD EPYC and AMD Ryzen processor families, header files and examples.
