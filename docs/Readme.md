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

## Project structure

The project is structured as follows:

- `Library`: This directory contains the source code for the project.

- `SDK`: The release folder.

- `Tests`: This directory contains the necessary unit tests for the project.

- `Tools`: The necessary tools to work with the project.

- `scripts`: Utility scripts to work with the project.

**Important:**
1. Most CPUID APIs (and their headers) introduced in 4.2 are deprecated; they will be
   removed in a future release. See the API documentation for the newer APIs.
2. Old APIs can be enabled with `AU_ENABLE_OLD_API=ON` during the build, otherwise deprecated
   warnings will be shown.
3. The aoclutils module combines au_core and au_cpuid.
4. The aoclutils module is the default module for all functionalities.
