# 1. Fix minimum compiler version

Date: 2024-05-20

## Context

There are so many combinations of compiler and tools that might have interdependcy and other issues.
Need to pin down on the list of compiler versions and tools utils support

## Decision

Minimum supported versions are as below
clang      --    12 and above
clang-tidy --    12 and above
gcc        --    9 and above
cmake      --    3.22 and above

Note: Other versions may work.(if not documented otherwise but this is the
tested version)
## Consequences

Issues with other versions wont' be fixed.

## Status

Accepted
