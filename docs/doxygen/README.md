@mainpage
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
* APIs for SHA,AES and VAES availability
* APIs for RDSEED and RDRND availability
* APIs for AVX2 availability
* APIs for AVX512 foundation and sub-feature flags
* API for cache topology

## Dependency

Need explicit linking of stdc++ library while using libaoclutils.a
Needs GCC/Clang compiler versions which supports C11 and C++17 standards.

## Important notes
Functions/members names may change in upcoming release of AOCL-Utils library.