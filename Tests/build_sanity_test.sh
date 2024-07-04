#!/bin/bash

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
if [ ! -d "cmake-$CMAKE_VERSION.0-linux-x86_64" ]; then \
        wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz; \
        tar -xf cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz; \
    fi;
cd -

# configure the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake -DAU_BUILD_TESTS=TRUE -DAU_BUILD_EXAMPLES=TRUE -DCMAKE_BUILD_TYPE=RELEASE \
 -B Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER} -DCMAKE_C_COMPILER=/usr/bin/$C_COMPILER -DCMAKE_CXX_COMPILER=/usr/bin/$CXX_COMPILER 2> $log_folder/configure.log; \
#build the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/cmake --build \
Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER} --config Release -j 2 > $log_folder/build.log; \
# test the project
Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64/bin/ctest -C release \
--test-dir Tests/sanity_test/cmake-$CMAKE_VERSION/build-${C_COMPILER}; \
# if failure log existst copy LastLog.txt to the host
if [ -f Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log ]; then \
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTest.log $log_folder/
    cp Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER/Testing/Temporary/LastTestsFailed.log $log_folder/
fi;
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION/build-$C_COMPILER
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64
rm -rf Tests/sanity_test/cmake-$CMAKE_VERSION.0-linux-x86_64.tar.gz
