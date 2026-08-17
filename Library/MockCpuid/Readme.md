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
cmake -B build -DAU_BUILD_MOCKCPUID_SHIM=ON -DAU_BUILD_TESTS=ON
cmake --build build
```

Python 3 (Interpreter) is a build-time prerequisite when
`AU_BUILD_MOCKCPUID_SHIM=ON`, because the baked profile table is generated from
the CPUID mock data, consistently with the Python-linked CPUID tests.

## Use

The normal consumer source is unchanged. For a direct shared-library link,
put the mock before the real library so the mock supplies the CPUID symbols and
the real library supplies the rest of the AOCL-Utils surface:

```sh
cc my_test.c -laoclutils_mock -laoclutils
```

For an already-built consumer, preload the mock library over the real one:

```sh
LD_PRELOAD=./libaoclutils_mock.so \
    AU_CPUID_MOCK_PROFILE=rome ./my_test
```

The C-only variant is selected in the same way with `libaoclutils_mock_c` and
`libaoclutils_c`:

```sh
cc my_test.c -laoclutils_mock_c -laoclutils_c
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
or from another vendor than, the host. Those requests are refused. The safe
feature subset still applies, but the refused portion is reported on stderr.

Strict mode is on by default. Set `AU_CPUID_MOCK_STRICT=0`, `false`, `no`, or
`off` to make an upward, cross-vendor, or unclassifiable identity, or an unknown
profile, warn and continue with the safe subset instead of aborting. Any other
value remains strict, so a typo cannot silently make CI permissive. Host-absent
features in an ordinary named profile are normal downward narrowing and are
never fatal. There is no strict-mode setter in Phase 1.

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
ctest --test-dir build -R -i MockCpuid --output-on-failure
```

The harness builds consumers twice from each source (real and mock), checks
the subset property, profile identity and reset behavior, exercises the
sentinel and strict-mode paths, verifies direct linking and `LD_PRELOAD`,
checks C and C++ consumers, runs a lower dispatch kernel, and compares the
mock's `au_cpuid_*` / `alci_*` symbol set with the real library.
