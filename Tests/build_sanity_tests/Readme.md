## Introduction

The build sanity test is a test suite that is run on the build artifacts to ensure that the build is not broken. It runs on different supported packages and checks if the build is successful. The test suite is run on the following packages:

All combinations of cmake 3.22 to 3.30, and gcc 14, 13, 12, 11, 10, 9 and clang 17, 16, 15, 14 along with clang-tidy validations
with make and ninja generators on ubuntu24.04. base docker image using robot framework.

## TODO

- Windows support.
- RHEL support.
- Aocc support.

## Steps to run the build sanity test

1. Install the required packages
   1. docker
   2. git
   3. python3
   4. robotframework
   5. pipx
   6. parobots
2. Clone this repository
3. pabot --testlevelsplit --processes 9 Tests/build_sanity_tests/docker/build_sanity_test.robot from project root directory.
    1. Run individual test cases:

        ```console
        python3 Tests/build_sanity_tests/run_build_sanity_test.py c_compiler cxx_compiler cmake_version
        ```
