<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

# 1. Fix minimum compiler version

Date: 2024-05-20

## Context

There are so many combinations of compiler and tools that might have interdependcy and other issues.
Need to pin down on the list of compiler versions and tools utils support

## Decision

The original 2024 minimums are superseded by the current build gates:

| Tool | Current enforced minimum |
|------|---------------------------|
| Clang | 12 |
| clang-tidy | 12 |
| GCC | 8 |
| CMake | 3.26 |
| MSVC | 19.29 |

The active build-sanity matrix covers GCC 9–14, Clang 14–17, and CMake
3.26–3.31. See `docs/SupportedPackageMatrix.md` for the current support
matrix.

Other versions may work, but are not covered by the project gates.
## Consequences

Issues with other versions wont' be fixed.

## Status

Superseded
