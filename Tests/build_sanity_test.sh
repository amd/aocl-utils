#!/bin/bash
#
# Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# This script is used to build the sanity test for the project
# The sanity test is a simple test that checks if the project builds correctly

# check if the cmake is installed
WORKDIR=/usr/src/app
CMAKE_VERSION=$1
C_COMPILER=$2
CXX_COMPILER=$3

# get the clang version for C_COMPILER
#
# clang-10 -> 10
# clang-11 -> 11
# clang-12 -> 12

# check if the C_COMPILER is clang-xx
# if it is, then get the version of clang
if [[ "$C_COMPILER" == *"clang-"* ]]; then
    echo "C_COMPILER is clang"
    clang_version=$(echo $C_COMPILER | cut -d'-' -f2)
    # install clang-tidy for the clang version
    apt-get update && apt-get install -y clang-tidy-$clang_version
    # make it the default clang-tidy
    update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-$clang_version 100
fi

mkdir -p Tests/sanity_test/cmake-$CMAKE_VERSION/$C_COMPILER
log_folder=Tests/sanity_test/cmake-$CMAKE_VERSION/$C_COMPILER

cd Tests/sanity_test
if [ ! -d "cmake-$CMAKE_VERSION.0-linux-x86_64" ]; then
    wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
    tar -xf cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
fi
cd -

# update the environment.log
echo "************C_COMPILER: --version***************************" >$log_folder/environment.log
$C_COMPILER --version >>$log_folder/environment.log
echo "************CXX_COMPILER: --version*************************" >>$log_folder/environment.log
$CXX_COMPILER --version >>$log_folder/environment.log
echo "************CMAKE_VERSION: cmake --version******************" >>$log_folder/environment.log
$CMAKE_VERSION --version >>$log_folder/environment.log
echo "************CLANG-TIDY VERSION: clang-tidy --version********" >>$log_folder/environment.log
clang-tidy --version >>$log_folder/environment.log

Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --version >>$log_folder/environment.log

set -e
# configure the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake -DAU_ENABLE_OLD_API=TRUE -DAU_BUILD_TESTS=TRUE -DAU_BUILD_EXAMPLES=TRUE -DCMAKE_BUILD_TYPE=RELEASE \
    -B Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make -DCMAKE_C_COMPILER=/usr/bin/$C_COMPILER -DCMAKE_CXX_COMPILER=/usr/bin/$CXX_COMPILER 2>&1 >$log_folder/configure_make.log

Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake -DAU_ENABLE_OLD_API=TRUE -DAU_BUILD_TESTS=TRUE -DAU_BUILD_EXAMPLES=TRUE -DCMAKE_BUILD_TYPE=RELEASE \
    -B Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_ninja -DCMAKE_C_COMPILER=/usr/bin/$C_COMPILER -DCMAKE_CXX_COMPILER=/usr/bin/$CXX_COMPILER 2>&1 >$log_folder/configure_ninja.log
# build the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --build \
    Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make --config Release -j 2>&1 >$log_folder/build_make.log

Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --build \
    Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_ninja --config Release -j 2>&1 >$log_folder/build_ninja.log
# test the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/ctest -C release \
    --test-dir Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make 2>&1 >$log_folder/test_make.log
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/ctest -C release \
    --test-dir Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_ninja 2>&1 >$log_folder/test_ninja.log

# if failure log existst copy LastLog.txt to the host
if [ -f Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log ]; then
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTest.log $log_folder/
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log $log_folder/
fi

# clean up
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
