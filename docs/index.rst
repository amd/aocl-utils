.. AOCL-UTILS documentation master file, created by
   sphinx-quickstart on Thu Mar 14 16:16:42 2024.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.


AOCL-Utils
===========

.. toctree::
    :maxdepth: 2
    :numbered:
    :caption: Table of Contents:

    introduction
    root/cpuid/api/index
    root/logger/api/index
    root/status/api/index
    root/threadpinning/api/index

.. _introduction:

**Introduction**

AOCL-Utils is an effort to extract common functionalities across
libraries. Its main features include:

- Au Cpuid
  - Core details
  - Available flags
  - Usable ISA

- Au Core
  - Thread pinning
  - Status
  - Logger
  - RNG

*NOTE:*
This library detects only AMD "Zen" CPUs. There are no plans to support other x86
implementations. Some utilities may fail or behave unexpectedly on older AMD
architectures.

Core module is internal to AOCL-Utils. To use its features, link to libaoclutils, which
combines all available utility modules.

**Project Structure**

The project is structured as follows:

- `Library`: This directory contains the source code for the project.

- `SDK`: The release folder.

- `Tests`: This directory contains the necessary unit tests for the project.

- `Tools`: The necessary tools to work with the project.

- `scripts`: Utility scripts to work with the project.

**Important Notes**

1. Most CPUID APIs (and their headers) introduced in 4.2 are deprecated; they will be
   removed in a future release. See the API documentation for the newer APIs.

2. Old APIs can be enabled with ``AU_ENABLE_OLD_API=ON`` during the build, otherwise deprecated
   warnings will be shown.

3. The aoclutils module combines au_core and au_cpuid.

4. The aoclutils module is the default module for all functionalities.
