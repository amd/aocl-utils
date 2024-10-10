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

mkdir -p Tests/sanity_test/cmake-$CMAKE_VERSION/$C_COMPILER
log_folder=Tests/sanity_test/cmake-$CMAKE_VERSION/$C_COMPILER

cd Tests/sanity_test
if [ ! -d "cmake-$CMAKE_VERSION.0-linux-x86_64" ]; then
    wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
    tar -xf cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
fi
cd -

# install llvm and clang-tidy of the same version as the compiler if the compiler is clang

# extract the compiler name
compiler_name=$(echo $C_COMPILER | sed -e 's/[^clang]//g')
echo "compiler_name: $compiler_name"

# extract the compiler version
compiler_version=$(echo $C_COMPILER | sed -e 's/[^0-9]//g')

## FIXME: The clang-tidy version same as the compiler version is not available in the repo
if [ "$compiler_name" = "clang" ]; then
    # install clang-tidy and llvm of compiler_version and make it the default clang-tidy from repo
    apt-get install -y clang-tidy-$compiler_version
    # make the installed clang-tidy the default clang-tidy
    update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-$compiler_version 100
fi

touch environment.log
# update the environment.log
echo "************C_COMPILER: --version***************************" >$log_folder/environment.log
$C_COMPILER --version >>$log_folder/environment.log
echo "************CXX_COMPILER: --version*************************" >>$log_folder/environment.log
$CXX_COMPILER --version >>$log_folder/environment.log
echo "************CMAKE_VERSION: cmake --version******************" >>$log_folder/environment.log
$CMAKE_VERSION --version >>$log_folder/environment.log
echo "************CLANG-TIDY VERSION: clang-tidy --version********" >>$log_folder/environment.log
clang-tidy --version >>$log_folder/environment.log
echo "************NINJA VERSION ninja --version******************" >>$log_folder/environment.log
ninja --version >>$log_folder/environment.log
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --version >>$log_folder/environment.log

set -e
# configure the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake -DAU_ENABLE_OLD_API=TRUE -DAU_BUILD_TESTS=TRUE -DAU_BUILD_EXAMPLES=TRUE -DCMAKE_BUILD_TYPE=RELEASE \
    -B Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make -DCMAKE_C_COMPILER=/usr/bin/$C_COMPILER -DCMAKE_CXX_COMPILER=/usr/bin/$CXX_COMPILER 2>&1 >$log_folder/configure_make.log

Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake -G Ninja -DAU_ENABLE_OLD_API=TRUE -DAU_BUILD_TESTS=TRUE -DAU_BUILD_EXAMPLES=TRUE -DCMAKE_BUILD_TYPE=RELEASE \
    -B Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_ninja -DCMAKE_C_COMPILER=/usr/bin/$C_COMPILER -DCMAKE_CXX_COMPILER=/usr/bin/$CXX_COMPILER 2>&1 >$log_folder/configure_ninja.log

# build the project
touch build_make.log
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --build \
    Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make --config Release -j 2>&1 >$log_folder/build_make.log

touch build_ninja.log
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --build \
    Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_ninja --config Release -j 2>&1 >$log_folder/build_ninja.log

# test the project
touch test_make.log
export PYTHONPYCACHEPREFIX=Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make; Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/ctest -C release \
    --test-dir Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}_make 2>&1 >$log_folder/test_make.log
#touch test_ninja.log
#ninja test

# if failure log existst copy LastLog.txt to the host
if [ -f Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log ]; then
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTest.log $log_folder/
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log $log_folder/
fi
