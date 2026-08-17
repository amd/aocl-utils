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

# ---------------------------------------------------------------------------
# Mock-CPUID link shims.
#
# The C variant emits the complete au_cpuid_* / alci_* surface and contains no
# C++ source or C++ runtime dependency. The C++ variant re-emits that same C
# surface and X86Cpu over the redirected core, then uses the real umbrella
# library for the non-CPUID C++ surface.
# ---------------------------------------------------------------------------

if(NOT Python3_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
endif()

set(AU_MOCKCPUID_PROFILES_H
    "${CMAKE_CURRENT_BINARY_DIR}/generated/mockcpuid_profiles.h")

file(GLOB AU_MOCKCPUID_FLAG_DATA
     "${PROJECT_SOURCE_DIR}/Library/Tests/Cpuid/Mock/simnowdata/*/FlagsT.txt")

add_custom_command(
    OUTPUT  ${AU_MOCKCPUID_PROFILES_H}
    COMMAND ${Python3_EXECUTABLE}
            "${PROJECT_SOURCE_DIR}/scripts/tools/gen_mock_profiles.py"
            --flags-header
            "${PROJECT_SOURCE_DIR}/SDK/Include/Capi/au/cpuid/cpuid_flags.h"
            --simnow-dir
            "${PROJECT_SOURCE_DIR}/Library/Tests/Cpuid/Mock/simnowdata"
            --output ${AU_MOCKCPUID_PROFILES_H}
    DEPENDS "${PROJECT_SOURCE_DIR}/scripts/tools/gen_mock_profiles.py"
            "${PROJECT_SOURCE_DIR}/SDK/Include/Capi/au/cpuid/cpuid_flags.h"
            ${AU_MOCKCPUID_FLAG_DATA}
    COMMENT "Generating Mock-CPUID profiles from simnowdata"
    VERBATIM)

add_custom_target(mockcpuid_profiles DEPENDS ${AU_MOCKCPUID_PROFILES_H})

set(AU_MOCKCPUID_C_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/mockcpuid_export.c")
set(AU_MOCKCPUID_CXX_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/mockcpuid_export.cc")
set_source_files_properties(${AU_MOCKCPUID_C_SRC} PROPERTIES LANGUAGE C)

function(au_configure_mockcpuid_target __name __kind __output __language)
    add_library(${__name} ${__kind} ${ARGN} ${AU_MOCKCPUID_PROFILES_H})
    add_dependencies(${__name} mockcpuid_profiles)
    set_target_properties(${__name} PROPERTIES
        OUTPUT_NAME               ${__output}
        POSITION_INDEPENDENT_CODE ON
        LINKER_LANGUAGE           ${__language}
    )
    if(__language STREQUAL "C")
        set_target_properties(${__name} PROPERTIES
            C_STANDARD        11
            C_STANDARD_REQUIRED ON
        )
    else()
        set_target_properties(${__name} PROPERTIES
            CXX_STANDARD        17
            CXX_STANDARD_REQUIRED ON
        )
    endif()
    target_include_directories(${__name} PRIVATE
        ${AU_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
    )
endfunction()

# Shared and static import libraries occupy the same .lib namespace on
# Windows. Match the real C-only target's convention so building both mock
# variants does not overwrite an import library.
if(UNIX)
    set(AU_MOCKCPUID_C_STATIC_OUTPUT aoclutils_mock_c)
    set(AU_MOCKCPUID_STATIC_OUTPUT   aoclutils_mock)
else()
    set(AU_MOCKCPUID_C_STATIC_OUTPUT aoclutils_mock_c_static)
    set(AU_MOCKCPUID_STATIC_OUTPUT   aoclutils_mock_static)
endif()

if(AU_BUILD_SHARED_LIBS)
    au_configure_mockcpuid_target(aoclutils_mock_c_shared SHARED
        aoclutils_mock_c C ${AU_MOCKCPUID_C_SRC})
    au_configure_mockcpuid_target(aoclutils_mock_shared SHARED
        aoclutils_mock CXX ${AU_MOCKCPUID_CXX_SRC})
    # X86Cpu's base/status/non-CPUID references are forwarded by the C++ shim
    # to the real umbrella library. Keeping this edge makes the shared shim a
    # direct drop-in and gives LD_PRELOAD the normal real-library surface.
    target_link_libraries(aoclutils_mock_shared PRIVATE aoclutils)
endif()

if(AU_BUILD_STATIC_LIBS)
    au_configure_mockcpuid_target(aoclutils_mock_c_static STATIC
        ${AU_MOCKCPUID_C_STATIC_OUTPUT} C ${AU_MOCKCPUID_C_SRC})
    au_configure_mockcpuid_target(aoclutils_mock_static STATIC
        ${AU_MOCKCPUID_STATIC_OUTPUT} CXX ${AU_MOCKCPUID_CXX_SRC})
    # Archives have no DT_NEEDED edge. Propagate the real archive so a consumer
    # can use the documented mock-first + real-library link order.
    target_link_libraries(aoclutils_mock_static INTERFACE aoclutils_static)
endif()

# Deliberately NOT install()ed. This is a test and bring-up tool; shipping it
# beside the real library invites accidental production use.
