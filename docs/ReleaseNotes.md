(C) 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

# AOCL Utils - Release Notes

## Updates after 5.0.1

### Build System

#### CMake
- Removed absolute path from binaries on Linux builds
- Updated scripts for improved find_package handling
- Enabled sanitizers (TSAN, MEMSAN) and RelWithDebInfo build type
- Added support for pkg-config configuration
- Upgraded minimum CMake version to 3.26 (tested up to 3.31)
- Improved workflow with Linux presets
- Added support for MultiThreaded library in MSVC

### Library

#### Logger
- Implemented extendable and flexible thread-safe logger
- Added TLS support from std::threads
- Introduced "<<" operator override for LogManager
- Added CAPI and CPP API macros
- Added Windows platform support
- Implemented singleton pattern for LogWriter
- Used direct initialization for logger objects

#### Cpuid
- Introduced new CPU detection for Zen-family processors
- Added isZenFamily API (family >= 0x17)
- Added CAPI for generic CPU detection (v2, v3, v4)
- Provided has_any() and has_all() CPUID checks
- Fixed CPUID instruction implementation
- Corrected request and response EFlags handling
- Fixed has_flags API implementation

#### Rng
- Implemented Hardware and System Random Number Generator
- Added flexible framework for RNG implementations

#### Status
- Added isError API for iError interface
- Improved Status.ok() implementation
- Enhanced error handling capabilities

#### ThreadPinning
- Added security checks for CacheMap
- Improved thread vector validations

#### Misc
- Fixed enabling assertions

### Tests
- Added comprehensive unit tests for Status module
- Added unit tests for Calculator components
- Improved Docker-based build sanity checks
- Fixed race conditions in parallel test execution
- Corrected expected architecture data in CPUID tests

### SDK
- Removed unused header files
- Reorganized headers (moved from Private)
- Enhanced C examples compatibility
- Added cpuid_legacy to architecture support
- Added demonstration code for all flags
- Added logger demo code in examples

### Documentation
- Enhanced logger usage documentation
- Added comprehensive API summary
- Updated X86_64{V2,V3,V4} API tables
- Added Zen-family support documentation
- Added ROCm 0.32.0 support information
- Synced documentation with gh-pages branch

### Licensing & Maintenance
- Updated copyright year handling
- Updated license information
- Updated NOTICES file
- Fixed multiple compiler warnings (including GCC-14.2)
- Standardized build configurations
