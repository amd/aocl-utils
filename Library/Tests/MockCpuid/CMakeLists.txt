#
# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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
#    without specific prior written permission.
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

# Every consumer source is built twice from identical text: once against the
# real library and once against the corresponding mock library. The driver
# then exercises link substitution, LD_PRELOAD, profiles, reset, and dispatch.

if(NOT Python3_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
endif()

if(NOT TARGET aoclutils_mock_shared OR NOT TARGET aoclutils_mock_c_shared)
    message(STATUS
        "MockCpuid tests skipped: shared mock targets not built "
        "(AU_BUILD_SHARED_LIBS=OFF)")
    return()
endif()

function(au_add_mockcpuid_consumer __name __source)
    add_executable(${__name} ${__source})
    get_filename_component(__extension "${__source}" EXT)
    if(__extension STREQUAL ".cc" OR __extension STREQUAL ".cpp")
        set_target_properties(${__name} PROPERTIES
            CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    else()
        set_target_properties(${__name} PROPERTIES
            C_STANDARD 11 C_STANDARD_REQUIRED ON)
    endif()
    target_include_directories(${__name} PRIVATE ${AU_INCLUDE_DIRS})
    target_link_libraries(${__name} PRIVATE ${ARGN})
endfunction()

set(AU_MOCK_DISPATCH_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/MockDispatch")
set(AU_MOCK_DISPATCH_MAIN
    "${AU_MOCK_DISPATCH_DIR}/dispatch_main.c")
set(AU_MOCK_DISPATCH_KERNEL_SOURCES
    "${AU_MOCK_DISPATCH_DIR}/kernel_scalar.c"
    "${AU_MOCK_DISPATCH_DIR}/kernel_avx2.c"
    "${AU_MOCK_DISPATCH_DIR}/kernel_avx512.c")

# Keep dispatch_main.c and kernel_scalar.c at baseline ISA. Only the vector
# kernel translation units receive elevated ISA flags.
if(AU_COMPILER_IS_MSVC)
    set_source_files_properties(
        "${AU_MOCK_DISPATCH_DIR}/kernel_avx2.c"
        PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
    set_source_files_properties(
        "${AU_MOCK_DISPATCH_DIR}/kernel_avx512.c"
        PROPERTIES COMPILE_OPTIONS "/arch:AVX512")
else()
    set_source_files_properties(
        "${AU_MOCK_DISPATCH_DIR}/kernel_avx2.c"
        PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma")
    set_source_files_properties(
        "${AU_MOCK_DISPATCH_DIR}/kernel_avx512.c"
        PROPERTIES COMPILE_OPTIONS "-mavx512f")
endif()

au_add_mockcpuid_consumer(MockCpuidSafetyReal
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidSafetyTest.c" aoclutils)
au_add_mockcpuid_consumer(MockCpuidSafetyShim
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidSafetyTest.c"
    aoclutils_mock_c_shared aoclutils_c_shared)
au_add_mockcpuid_consumer(MockCpuidDispatchReal
    "${AU_MOCK_DISPATCH_MAIN}" aoclutils)
target_sources(MockCpuidDispatchReal PRIVATE ${AU_MOCK_DISPATCH_KERNEL_SOURCES})
au_add_mockcpuid_consumer(MockCpuidDispatchShim
    "${AU_MOCK_DISPATCH_MAIN}"
    aoclutils_mock_c_shared aoclutils_c_shared)
target_sources(MockCpuidDispatchShim PRIVATE ${AU_MOCK_DISPATCH_KERNEL_SOURCES})

au_add_mockcpuid_consumer(MockCpuidCppReal
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidCppTest.cc" aoclutils)
au_add_mockcpuid_consumer(MockCpuidCppShim
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidCppTest.cc"
    aoclutils_mock_shared aoclutils)

au_add_mockcpuid_consumer(MockCpuidApi
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidApiTest.c"
    aoclutils_mock_c_shared)
au_add_mockcpuid_consumer(MockCpuidProgrammaticStrict
    "${CMAKE_CURRENT_SOURCE_DIR}/MockCpuidProgrammaticStrictTest.c"
    aoclutils_mock_c_shared)

add_test(NAME MockCpuidBehaviourTest
    COMMAND ${Python3_EXECUTABLE}
            "${CMAKE_CURRENT_SOURCE_DIR}/run_mockcpuid_tests.py"
            --safety-real   $<TARGET_FILE:MockCpuidSafetyReal>
            --safety-shim   $<TARGET_FILE:MockCpuidSafetyShim>
            --dispatch-real $<TARGET_FILE:MockCpuidDispatchReal>
            --dispatch-shim $<TARGET_FILE:MockCpuidDispatchShim>
            --cpp-real      $<TARGET_FILE:MockCpuidCppReal>
            --cpp-shim      $<TARGET_FILE:MockCpuidCppShim>
            --api           $<TARGET_FILE:MockCpuidApi>
            --programmatic $<TARGET_FILE:MockCpuidProgrammaticStrict>
            --mock-c        $<TARGET_FILE:aoclutils_mock_c_shared>)

add_test(NAME MockCpuidSelfTest
    COMMAND ${Python3_EXECUTABLE}
            "${CMAKE_CURRENT_SOURCE_DIR}/run_mockcpuid_tests.py"
            --self-test
            --safety-real   $<TARGET_FILE:MockCpuidSafetyReal>
            --safety-shim   $<TARGET_FILE:MockCpuidSafetyShim>
            --dispatch-real $<TARGET_FILE:MockCpuidDispatchReal>
            --dispatch-shim $<TARGET_FILE:MockCpuidDispatchShim>
            --cpp-real      $<TARGET_FILE:MockCpuidCppReal>
            --cpp-shim      $<TARGET_FILE:MockCpuidCppShim>
            --api           $<TARGET_FILE:MockCpuidApi>
            --programmatic $<TARGET_FILE:MockCpuidProgrammaticStrict>
            --mock-c        $<TARGET_FILE:aoclutils_mock_c_shared>)

find_program(AU_NM_EXECUTABLE nm)
if(AU_NM_EXECUTABLE AND TARGET aoclutils)
    add_test(NAME MockCpuidParityTest
        COMMAND ${Python3_EXECUTABLE}
                "${CMAKE_CURRENT_SOURCE_DIR}/check_symbol_parity.py"
                --real $<TARGET_FILE:aoclutils>
                --shim $<TARGET_FILE:aoclutils_mock_shared>
                --nm   ${AU_NM_EXECUTABLE})
    add_test(NAME MockCpuidCParityTest
        COMMAND ${Python3_EXECUTABLE}
                "${CMAKE_CURRENT_SOURCE_DIR}/check_symbol_parity.py"
                --real $<TARGET_FILE:aoclutils>
                --shim $<TARGET_FILE:aoclutils_mock_c_shared>
                --nm   ${AU_NM_EXECUTABLE})
else()
    message(STATUS "MockCpuid symbol-parity tests skipped: nm not found")
endif()
