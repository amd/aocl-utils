SupportedPackageMatrix.md

One of the compilers and generators on the platform has to be installed to build the library, along with atleast the minimum version of cmake. The following table lists the supported compilers and generators for the library.

**Note** Use same compiler to build utils and the application that uses utils.

## Supported Compilers and Generators

## Windows

| Package | Version | Supported | Notes |
| ------- | ------- | --------- | ----- |
| clang   | 11.0    | No        |       |
| clang   | 12.0    | Yes       | Minimum clang version supported.      |
| clang   | 13.0    | Yes       |       |
| clang   | 14.0    | Yes       |       |
| clang   | 15.0    | Yes       |       |
| clang   | 16.0    | Yes       |       |
| clang   | 17.0    | Yes       |       |
| clang   | 18.0    | Yes       |       |
| aocc    | 4.2     | Yes       | Download from|
| aocc    | 5.0     | Yes       | Download from|
| msvc    | 2019    | Yes       |       |
| msvc    | 2022    | Yes       |       |
| cmake   | 3.22    | Yes       | Minimum version supported.      |
| cmake   | 3.23    | Yes       |       |
| cmake   | 3.24    | Yes       |       |
| cmake   | 3.25    | Yes       |       |
| cmake   | 3.26    | Yes       |       |
| cmake   | 3.27    | Yes       |       |
| cmake   | 3.28    | Yes       |       |
| cmake   | 3.29    | Yes       |       |
| cmake   | 3.30    | Yes       |       |
| ninja   | 1.10    | Yes       | Minimum version supported.      |
| visualstudio | 2019 | Yes       |       |
| visualstudio | 2022 | Yes       |       |

TODO: Add download links for aocc.
verify clang-tidy is working

## Linux

| Package | Version | Supported | Notes |
| ------- | ------- | --------- | ----- |
| clang   | 11.0    | No        |       |
| clang   | 12.0    | Yes       | Minimum clang version supported.      |
| clang   | 13.0    | Yes       |       |
| clang   | 14.0    | Yes       |       |
| clang   | 15.0    | Yes       |       |
| clang   | 16.0    | Yes       |       |
| clang   | 17.0    | Yes       |       |
| clang   | 18.0    | Yes       |       |
| aocc    | 4.2     | Yes       | Download from|
| aocc    | 5.0     | Yes       | Download from|
| gcc     | 8.5     | Yes       |       |
| gcc     | 9       | Yes       |       |
| gcc     | 10      | Yes       |       |
| gcc     | 11      | Yes       |       |
| gcc     | 12      | Yes       |       |
| gcc     | 13      | Yes       |       |
| gcc     | 14      | Yes       |       |
| cmake   | 3.22    | Yes       | Minimum version supported.      |
| cmake   | 3.23    | Yes       |       |
| cmake   | 3.24    | Yes       |       |
| cmake   | 3.25    | Yes       |       |
| cmake   | 3.26    | Yes       |       |
| cmake   | 3.27    | Yes       |       |
| cmake   | 3.28    | Yes       |       |
| cmake   | 3.29    | Yes       |       |
| cmake   | 3.30    | Yes       |       |
| ninja   | 1.10    | Yes       | Minimum version supported.      |
| clang-tidy | 12.0  | Yes       | Minimum version supported.      |
| clang-tidy | 13.0  | Yes       |       |
| clang-tidy | 14.0  | Yes       |       |
| clang-tidy | 15.0  | Yes       |       |
| clang-tidy | 16.0  | Yes       |       |
| clang-tidy | 17.0  | Yes       |       |
| clang-tidy | 18.0  | Yes       |       |

TODO: Steps to disable clang tidy.

### Standard Libraries

The library uses the standard C++ libraries and does not have any external dependencies. The library is tested with the following standard libraries:

#### cpuid

| Package | Version | Supported | Notes |
| ------- | ------- | --------- | ----- |
| libstdc++ | 8.5   | Yes       |       |
| libstdc++ | 9     | Yes       |       |
| libstdc++ | 10    | Yes       |       |
| libstdc++ | 11    | Yes       |       |
| libc++    | 8.5   | Yes       |       |
| libc++    | 9     | Yes       |       |
| libc++    | 10    | Yes       |       |
| libc++    | 11    | Yes       |       |
| libc++    | 12    | Yes       |       |
| libc++    | 13    | Yes       |       |
| libc++    | 14    | Yes       |       |

#### core

| Package | Version | Supported | Notes |
| ------- | ------- | --------- | ----- |
| libstdc++ | 8.5   | Yes       |       |
| libstdc++ | 9     | Yes       |       |
| libstdc++ | 10    | Yes       |       |
| libstdc++ | 11    | Yes       |       |
| libc++    | 8.5   | Yes       |       |
| libc++    | 9     | Yes       |       |
| libc++    | 10    | Yes       |       |
| libc++    | 11    | Yes       |       |
| libc++    | 12    | Yes       |       |
| libc++    | 13    | Yes       |       |
| libc++    | 14    | Yes       |       |
|libpthread | 8.5    | Yes       |       |
|libpthread | 9      | Yes       |       |
|libpthread | 10     | Yes       |       |
|libpthread | 11     | Yes       |       |
|libpthread | 12     | Yes       |       |
|libpthread | 13     | Yes       |       |
|libpthread | 14     | Yes       |       |

#### tests

| Package  | Version | Supported | Notes |
| -------  | ------- | --------- | ----- |
| python   | 3.9     | Yes       | Minimum version supported.      |
| python   | 3.10    | Yes       |       |
| python   | 3.11    | Yes       |       |
| python   | 3.12    | Yes       |       |
| qemu-user|         | Yes       |       |

**Note: One might have to install the required packages based on the platform.  and explicitly link the depency libraries.**
