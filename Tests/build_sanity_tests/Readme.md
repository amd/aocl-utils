<!-- Copyright (C) 2026, Advanced Micro Devices. All rights reserved. -->

## Introduction

The build sanity test is a test suite that is run on the build artifacts to ensure that the build is not broken. It runs on different supported packages and checks if the build is successful. The test suite is run on the following packages:

The active matrix covers CMake 3.26 through 3.31 with GCC 9 through 14 and
Clang 14 through 17, including clang-tidy validation, with Make and Ninja
generators on an Ubuntu 24.04 base image using Robot Framework.

## TODO

- Windows support.
- RHEL support.
- AOCC support.

## Steps to run the build sanity test

1. Install the required packages
   1. docker
   2. git
   3. python3
   4. robotframework
   5. pipx
   6. pabot (from robotframework-pabot)
2. Clone this repository
3. From the project root, run:
   `pabot --testlevelsplit --processes 9 Tests/build_sanity_tests/docker/build_sanity_test.robot`
    1. Run individual test cases:

        ```console
        python3 Tests/build_sanity_tests/build_sanity_test.py c_compiler cxx_compiler cmake_version
        ```
