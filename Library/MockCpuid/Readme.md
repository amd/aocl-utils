<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# Mock-CPUID link shims

The Mock-CPUID facility is an opt-in test shim for AOCL-Utils. It presents a
named, older CPU profile to a consumer while preserving the safety rule that
the reported feature set is always a subset of the real host's features.

Phase 1 provides two link-time libraries:

- `libaoclutils_mock` contains the masked C API and the C++ `X86Cpu` surface.
- `libaoclutils_mock_c` contains the masked C API only and has no libstdc++
  dependency.

Both are off by default and are not installed.

```sh
cmake -S . -B build -DAU_BUILD_MOCKCPUID_SHIM=ON -DAU_BUILD_TESTS=ON
cmake --build build
```

Python 3 (Interpreter) is a build-time prerequisite when
`AU_BUILD_MOCKCPUID_SHIM=ON`, because the baked profile table is generated from
the CPUID mock data. When `AU_BUILD_TESTS=ON`, the CPUID test target additionally
requires the Python 3 Development components (headers and libraries).

## Use

The normal consumer source is unchanged. For a direct shared-library link,
put the mock before the real library so the mock supplies the CPUID symbols and
the real library supplies the rest of the AOCL-Utils surface:

```sh
cc -I SDK/Include -I build/generated \
    -L build/Library/MockCpuid -L build/Library \
    my_test.c -laoclutils_mock -laoclutils
```

For an already-built consumer, preload the mock library over the real one:

```sh
LD_LIBRARY_PATH=build/Library/MockCpuid:build/Library \
LD_PRELOAD=build/Library/MockCpuid/libaoclutils_mock.so \
    AU_CPUID_MOCK_PROFILE=rome ./my_test
```

The C-only variant is selected in the same way with `libaoclutils_mock_c` and
`libaoclutils_c`:

```sh
cc -I SDK/Include -I build/generated \
    -L build/Library/MockCpuid -L build/Library/Cpuid \
    my_test.c -laoclutils_mock_c -laoclutils_c
```

Profiles are baked from
`Library/Tests/Cpuid/Mock/simnowdata/<Model>/FlagsT.txt`. All 85 model
directory names are available, plus short aliases such as `milan`, `rome`,
`genoa`, `turin`, and `venice`:

```sh
AU_CPUID_MOCK_PROFILE=milan ./my_test
AU_CPUID_MOCK_PROFILE=list ./my_test
```

Phase 1 deliberately exposes profiles and reset only. The free-form feature
mask and override-table API are reserved for a later phase.

## Control API

The mock libraries export
`Capi/au/cpuid/mockcpuid.h`:

```c
au_mock_status_t au_mock_apply_profile(AU_CPUID_MOCK_ALL_CPUS, "rome");
au_mock_status_t au_mock_reset(AU_CPUID_MOCK_ALL_CPUS);
```

Only `AU_CPUID_MOCK_ALL_CPUS` is implemented. Any other CPU selector returns
`AU_MOCK_ERR_PERCPU_UNSUPPORTED` without changing policy. The host CPUID result
is cached once; the profile policy can be replaced at runtime, and the query
path takes no lock. Configure the mock before starting threads because setters
are not thread-safe with concurrent queries.

## Safety and strict mode

A profile can request features absent on the host or an identity newer than,
or from another vendor than, the host. Host-absent feature bits are warned
about and narrowed out; an unsafe identity is refused. In both cases the safe
feature subset is applied and the diagnostic is reported on stderr.

Strict mode is on by default. Set `AU_CPUID_MOCK_STRICT=0`, `false`, `no`, or
`off` to make an upward, cross-vendor, or unclassifiable identity, or an unknown
profile, warn and continue instead of aborting. An unknown profile returns
`AU_MOCK_ERR_NO_PROFILE` and leaves the current policy unchanged (initially,
the process reports the real host); it does not silently create a safe profile.
Any other strict-mode value remains strict, so a typo cannot silently make CI
permissive. Host-absent features in an ordinary named profile are normal
downward narrowing and are never fatal. There is no strict-mode setter in
Phase 1.

Pre-Zen profiles (the `Opteron_G*` and `phenom` entries) have no registry
microarchitecture, so identity mocking for them is unavailable: they narrow
features to the host but keep the host identity, aborting under strict mode and
warning under `AU_CPUID_MOCK_STRICT=0`.

The identity is masked at the raw CPUID level along with the features, so
`au_cpuid_arch_is_zenN`, `is_amd`, `is_intel`, x86-64 levels, and C++ `X86Cpu`
queries all decode one consistent result. A newer or cross-vendor identity is
left as the host identity and is reported as refused.

## Tests

```sh
ctest --test-dir build -R MockCpuid --output-on-failure --no-tests=error
```

The harness builds consumers twice from each source (real and mock), checks
the subset property, profile identity and reset behavior, exercises the
sentinel and strict-mode paths, verifies direct linking and the Linux
`LD_PRELOAD` cases, and checks C and C++ consumers. It runs a lower dispatch
kernel and compares the mock's `au_cpuid_*` / `alci_*` symbol set with the real
library when `nm` is available. Non-Linux runs skip the loader-specific cases,
and platforms without a suitable `nm` do not register the parity tests.
