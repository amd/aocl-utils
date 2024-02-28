# AOCL-Utils

## OVERVIEW

The library AOCL-Utils is intended to provide a uniform interface to all AOCL libraries to access CPU features, especially for AMD CPUs. The library provides the following features:

    * Core details
    * Flags available/usable
    * ISA available/usable
    * Topology about L1/L2/L3

AOCL-Utils is designed to be integrated into other AOCL libraries. Each project has their own mechanism to identify CPU and provide necessary features such as "dynamic dispatch". The main purpose of this library is to provide a centralized mechanism to update/validate and provide information to the users of this library.

## BUILDING

Please refer to BUILD.md file for build instructions

## Features
Currently supported:

* ISA available/usable     
* API to check following features:
  – SHA, AES, and VAES availability
  – RDSEED and RDRAND availability
  – AVX2 availability
  – AVX512 foundation and sub-feature flags
* APIs for cache topology:
  - Cache size and line size
  - Number of ways and sets
  - Number of logical processors sharing cache
  - Number of physical partitions
  - Fully associative
  - Self Initializing
  - Cache Inclusive/Exclusive

## Dependency

Need explicit linking of stdc++ library while using libaoclutils.a
Needs GCC/Clang compiler versions which supports C11 and C++17 standards.

## Important notes
Functions/members names may change in upcoming release of AOCL-Utils library.

## Contacts
AOCL-UTILS is developed and maintained by AMD. For any issues/suggestions you can contact us on the email-id toolchainsupport@amd.com. 
