<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# Building

```sh
cmake . -B build -DAUROOT=<Utils install path>
cmake --build build --config Release -j
```

Set `LD_LIBRARY_PATH=<Utils install path>/lib` when running against an
installation. `AUROOT` is the AOCL-Utils install directory.

# Mock-CPUID dispatch example (`cpuid_mockcpuid_example_c`)

Build the example and the opt-in link shim with:

```sh
cmake . -B build -DAU_BUILD_EXAMPLES=ON -DAU_BUILD_MOCKCPUID_SHIM=ON
cmake --build build -j
```

The source is an ordinary CPUID consumer. It selects a kernel from the
reported feature set and runs it. The example links the C++-surface mock first
and the real library second:

```sh
# host features: pick the best kernel available
./cpuid_mockcpuid_example_c

# present an older profile; the lower kernel is selected and executed
AU_CPUID_MOCK_PROFILE=Conroe-v1 AU_CPUID_MOCK_STRICT=0 \
    ./cpuid_mockcpuid_example_c

# an impossible profile is fatal by default; use strict=0 to observe the safe subset
AU_CPUID_MOCK_PROFILE=turin AU_CPUID_MOCK_STRICT=0 \
    ./cpuid_mockcpuid_example_c
```

The reported feature set is always a subset of the host's, so every path the
consumer can select is executable on the real machine. Phase 1 supports named
profiles only; arbitrary feature masks are a later phase.

See `Library/MockCpuid/Readme.md` for the complete Phase-1 API and delivery
rules.
