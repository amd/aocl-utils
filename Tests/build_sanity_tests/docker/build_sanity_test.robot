
*** Settings ***
Library           Process
Library           OperatingSystem
Test Template    Run Compiler And CMake Test

*** Keywords ***
# TODO Add teardown that deletes all docker images created.
# run build_sanity_test.py
Run Compiler And CMake Test
    [Arguments]                ${compiler}     ${cxx_compiler}    ${cmake_version}

    ${logfolder}=              Set Variable    ./Tests/sanity_test/cmake-${cmake_version}/${compiler}

    ${result}=                 Run Process     python   Tests/build_sanity_tests/build_sanity_test.py   ${compiler}    ${cxx_compiler}    ${cmake_version}    stdout=/tmp/stdout.txt    stderr=/tmp/stderr.txt

    Create File		       ${logfolder}/environment.log
    Create File		       ${logfolder}/configure_make.log
    Create File		       ${logfolder}/build_make.log
    Create File		       ${logfolder}/configure_ninja.log
    Create File		       ${logfolder}/build_ninja.log
    Create File		       ${logfolder}/test_make.log

    ${rc} =                    Set Variable    ${result.rc}

    ${stdout} =                Get File        /tmp/stdout.txt
    Log    ${stdout}
    ${stderr} =                Get File        /tmp/stderr.txt
    Log    ${stderr}
    ${stdout} =                Get File        /tmp/stdout.txt
    Log    ${stdout}
    ${stderr} =                Get File        /tmp/stderr.txt
    Log    ${stderr}
    ${environment.log} =       Get File        ${logfolder}/environment.log
    Log    ${environment.log}
    ${configure_make.log} =    Get File        ${logfolder}/configure_make.log
    Log    ${configure_make.log}
    ${build_make.log} =        Get File        ${logfolder}/build_make.log
    Log    ${build_make.log}
    ${configure_ninja.log} =   Get File        ${logfolder}/configure_ninja.log
    Log    ${configure_ninja.log}
    ${build_ninja.log} =       Get File        ${logfolder}/build_ninja.log
    Log    ${build_ninja.log}
    ${make_last.log} =         Get File        ${logfolder}/../build-${compiler}_make/Testing/Temporary/LastTest.log
    Log   ${make_last.log}
    #${ninja_last.log} =        Get File        ${logfolder}/../build-${compiler}_ninja/Testing/Temporary/LastTest.log
    #Log   ${ninja_last.log}
    ${test_make.log} =         Get File        ${logfolder}/test_make.log
    Log    ${test_make.log}
    Should Be Equal As Numbers    ${rc}   0
    #${test_ninja.log} =        Get File        ${logfolder}/test_ninja.log
    #Log    ${test_ninja.log}



***Test Cases***

Gcc-9 CMake-3.26
    [Tags]    gcc-9    cmake-3.26
    gcc-9    g++-9    3.26

Gcc-9 CMake-3.27
    [Tags]    gcc-9    cmake-3.27
    gcc-9    g++-9    3.27

Gcc-9 CMake-3.28
    [Tags]    gcc-9    cmake-3.28
    gcc-9    g++-9    3.28

Gcc-9 CMake-3.29
    [Tags]    gcc-9    cmake-3.29
    gcc-9    g++-9    3.29

Gcc-9 CMake-3.30
    [Tags]    gcc-9    cmake-3.30
    gcc-9    g++-9    3.30

Gcc-9 CMake-3.31
    [Tags]    gcc-9    cmake-3.31
    gcc-9    g++-9    3.31

Gcc-10 CMake-3.26
    [Tags]    gcc-10    cmake-3.26
    gcc-10    g++-10    3.26

Gcc-10 CMake-3.27
    [Tags]    gcc-10    cmake-3.27
    gcc-10    g++-10    3.27

Gcc-10 CMake-3.28
    [Tags]    gcc-10    cmake-3.28
    gcc-10    g++-10    3.28

Gcc-10 CMake-3.29
    [Tags]    gcc-10    cmake-3.29
    gcc-10    g++-10    3.29

Gcc-10 CMake-3.30
    [Tags]    gcc-10    cmake-3.30
    gcc-10    g++-10    3.30

Gcc-10 CMake-3.31
    [Tags]    gcc-10    cmake-3.31
    gcc-10    g++-10    3.31

Gcc-11 CMake-3.26
    [Tags]    gcc-11    cmake-3.26
    gcc-11    g++-11    3.26

Gcc-11 CMake-3.27
    [Tags]    gcc-11    cmake-3.27
    gcc-11    g++-11    3.27

Gcc-11 CMake-3.28
    [Tags]    gcc-11    cmake-3.28
    gcc-11    g++-11    3.28

Gcc-11 CMake-3.29
    [Tags]    gcc-11    cmake-3.29
    gcc-11    g++-11    3.29

Gcc-11 CMake-3.30
    [Tags]    gcc-11    cmake-3.30
    gcc-11    g++-11    3.30

Gcc-11 CMake-3.31
    [Tags]    gcc-11    cmake-3.31
    gcc-11    g++-11    3.31

Gcc-12 CMake-3.26
    [Tags]    gcc-12    cmake-3.26
    gcc-12    g++-12    3.26

Gcc-12 CMake-3.27
    [Tags]    gcc-12    cmake-3.27
    gcc-12    g++-12    3.27

Gcc-12 CMake-3.28
    [Tags]    gcc-12    cmake-3.28
    gcc-12    g++-12    3.28

Gcc-12 CMake-3.29
    [Tags]    gcc-12    cmake-3.29
    gcc-12    g++-12    3.29

Gcc-12 CMake-3.30
    [Tags]    gcc-12    cmake-3.30
    gcc-12    g++-12    3.30

Gcc-12 CMake-3.31
    [Tags]    gcc-12    cmake-3.31
    gcc-12    g++-12    3.31

Gcc-13 CMake-3.26
    [Tags]    gcc-13    cmake-3.26
    gcc-13    g++-13    3.26

Gcc-13 CMake-3.27
    [Tags]    gcc-13    cmake-3.27
    gcc-13    g++-13    3.27

Gcc-13 CMake-3.28
    [Tags]    gcc-13    cmake-3.28
    gcc-13    g++-13    3.28

Gcc-13 CMake-3.29
    [Tags]    gcc-13    cmake-3.29
    gcc-13    g++-13    3.29

Gcc-13 CMake-3.30
    [Tags]    gcc-13    cmake-3.30
    gcc-13    g++-13    3.30

Gcc-13 CMake-3.31
    [Tags]    gcc-13    cmake-3.31
    gcc-13    g++-13    3.31

Gcc-14 CMake-3.26
    [Tags]    gcc-14    cmake-3.26
    gcc-14    g++-14    3.26

Gcc-14 CMake-3.27
    [Tags]    gcc-14    cmake-3.27
    gcc-14    g++-14    3.27

Gcc-14 CMake-3.28
    [Tags]    gcc-14    cmake-3.28
    gcc-14    g++-14    3.28

Gcc-14 CMake-3.29
    [Tags]    gcc-14    cmake-3.29
    gcc-14    g++-14    3.29

Gcc-14 CMake-3.30
    [Tags]    gcc-14    cmake-3.30
    gcc-14    g++-14    3.30

Gcc-14 CMake-3.31
    [Tags]    gcc-14    cmake-3.31
    gcc-14    g++-14    3.31

Clang-14 CMake-3.26
    [Tags]    clang-14    cmake-3.26
    clang-14    clang++-14    3.26

Clang-14 CMake-3.27
    [Tags]    clang-14    cmake-3.27
    clang-14    clang++-14    3.27

Clang-14 CMake-3.28
    [Tags]    clang-14    cmake-3.28
    clang-14    clang++-14    3.28

Clang-14 CMake-3.29
    [Tags]    clang-14    cmake-3.29
    clang-14    clang++-14    3.29

Clang-14 CMake-3.30
    [Tags]    clang-14    cmake-3.30
    clang-14    clang++-14    3.30

Clang-14 CMake-3.31
    [Tags]    clang-14    cmake-3.31
    clang-14    clang++-14    3.31

Clang-15 CMake-3.26
    [Tags]    clang-15    cmake-3.26
    clang-15    clang++-15    3.26

Clang-15 CMake-3.27
    [Tags]    clang-15    cmake-3.27
    clang-15    clang++-15    3.27

Clang-15 CMake-3.28
    [Tags]    clang-15    cmake-3.28
    clang-15    clang++-15    3.28

Clang-15 CMake-3.29
    [Tags]    clang-15    cmake-3.29
    clang-15    clang++-15    3.29

Clang-15 CMake-3.30
    [Tags]    clang-15    cmake-3.30
    clang-15    clang++-15    3.30

Clang-15 CMake-3.31
    [Tags]    clang-15    cmake-3.31
    clang-15    clang++-15    3.31

Clang-16 CMake-3.26
    [Tags]    clang-16    cmake-3.26
    clang-16    clang++-16    3.26

Clang-16 CMake-3.27
    [Tags]    clang-16    cmake-3.27
    clang-16    clang++-16    3.27

Clang-16 CMake-3.28
    [Tags]    clang-16    cmake-3.28
    clang-16    clang++-16    3.28

Clang-16 CMake-3.29
    [Tags]    clang-16    cmake-3.29
    clang-16    clang++-16    3.29

Clang-16 CMake-3.30
    [Tags]    clang-16    cmake-3.30
    clang-16    clang++-16    3.30

Clang-16 CMake-3.31
    [Tags]    clang-16    cmake-3.31
    clang-16    clang++-16    3.31

Clang-17 CMake-3.26
    [Tags]    clang-17    cmake-3.26
    clang-17    clang++-17    3.26

Clang-17 CMake-3.27
    [Tags]    clang-17    cmake-3.27
    clang-17    clang++-17    3.27

Clang-17 CMake-3.28
    [Tags]    clang-17    cmake-3.28
    clang-17    clang++-17    3.28

Clang-17 CMake-3.29
    [Tags]    clang-17    cmake-3.29
    clang-17    clang++-17    3.29

Clang-17 CMake-3.30
    [Tags]    clang-17    cmake-3.30
    clang-17    clang++-17    3.30

Clang-17 CMake-3.31
    [Tags]    clang-17    cmake-3.31
    clang-17    clang++-17    3.31