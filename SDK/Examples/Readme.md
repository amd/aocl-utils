<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# Building examples

## In-tree build

Build the examples from the repository root. This is the supported path for
the Mock-CPUID example and uses the source-tree CMake modules directly:

```sh
cmake -S . -B build -DAU_BUILD_EXAMPLES=ON
cmake --build build
```

The executables are written to `build/Release`. When running a shared-library
example, use the corresponding build-tree library directory:

```sh
LD_LIBRARY_PATH=build/Library:build/Library/MockCpuid \
    build/Release/cpuid_example_c
```

## Standalone installed examples

The standalone `SDK/Examples` project currently expects CMake modules under
`<AUROOT>/lib/CMake`, while the AOCL-Utils install layout places them under
`<install-prefix>/share/aocl-utils/cmake`. Its standalone module lookup must be
aligned before this form can be used reliably. The Mock-CPUID shim is
deliberately not installed, so it cannot be built by a standalone installed
example project.

# Mock-CPUID dispatch example (`cpuid_mockcpuid_example_c`)

Build the example and the opt-in link shim from the repository root:

```sh
cmake -S . -B build \
    -DAU_BUILD_EXAMPLES=ON \
    -DAU_BUILD_MOCKCPUID_SHIM=ON
cmake --build build
```

The source is an ordinary CPUID consumer. It selects a kernel from the
reported feature set and runs it. The example links the C++-surface mock first
and the real library second:

```sh
export LD_LIBRARY_PATH=build/Library/MockCpuid:build/Library

# host features: pick the best kernel available
build/Release/cpuid_mockcpuid_example_c

# mask to the Conroe feature set; strict-off keeps the host identity because
# Conroe is a cross-vendor profile on an AMD host
AU_CPUID_MOCK_PROFILE=Conroe-v1 AU_CPUID_MOCK_STRICT=0 \
    build/Release/cpuid_mockcpuid_example_c

# on a pre-Turin host, strict mode rejects this upward profile; strict-off
# continues with the safe host subset
AU_CPUID_MOCK_PROFILE=turin AU_CPUID_MOCK_STRICT=0 \
    build/Release/cpuid_mockcpuid_example_c
```

The reported feature set is always a subset of the host's, so every path the
consumer can select is executable on the real machine. Phase 1 supports named
profiles only; arbitrary feature masks are a later phase. This example's
ISA-targeted functions currently require GCC/Clang-style target attributes and
are not portable to MSVC.

See `Library/MockCpuid/Readme.md` for the complete Phase-1 API and delivery
rules.
