#
# Copyright (C) 2022-2025, Advanced Micro Devices. All rights reserved.
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

cmake_minimum_required(VERSION 3.20...3.27)
project(cpuid_example_cpp)

list(APPEND CMAKE_MODULE_PATH ${AUROOT}/lib/CMake)
list(APPEND AU_INCLUDE_DIRS ${AUROOT}/include)
link_directories(${AUROOT}/lib)
include(au_examples)

# In-tree the umbrella is a CMake target and resolves to the correct
# library file on every platform, so link it by target name -- the shared
# umbrella when present, else the static-only umbrella. Out-of-tree
# (standalone SDK example build) there is no target and we link by the
# shipped filename: only a real MSVC/clang-cl toolchain needs the full name
# (libaoclutils.lib); every GNU-like linker -- Unix and the Windows
# MinGW/MSYS2/Cygwin toolchains (where WIN32 is also TRUE) -- strips the lib
# prefix, so it must get the bare "aoclutils" (-laoclutils -> libaoclutils.*).
if(TARGET aoclutils)
    set(AU_LIBS aoclutils)
elseif(TARGET aoclutils_static)
    set(AU_LIBS aoclutils_static)
elseif(MSVC)
    set(AU_LIBS libaoclutils)
else()
    set(AU_LIBS aoclutils)
endif()

# Only build CPUID examples if feature is enabled
if(au_cpuid_cpuid)
    au_add_application(cpuid_example_cpp
                    SOURCES cpuid_example.cc
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})

    au_add_application(cpuid_deprecated_api_example_cpp
                    SOURCES cpuid_deprecated_api_example.cc
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})

    au_add_application(cpuid_all_flags_example_cpp
                    SOURCES cpuid_all_flags_example.cc
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
endif()

# Only build Logger examples if feature is enabled
if(au_core_Logger)
    au_add_application(logger_demo_cpp
                    SOURCES logger_demo.cc
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
endif()

# Only build ThreadPinning examples if feature is enabled
if(au_core_ThreadPinning)
    au_add_application(thread_pinning_example_cpp
                    SOURCES thread_pinning_example.cc
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
endif()

#if built inside the source tree, add dependencies.
if(AU_BUILD_EXAMPLES AND AU_BUILD_SHARED_LIBS)
    if(au_cpuid_cpuid)
        add_dependencies(cpuid_example_cpp aoclutils au_cpuid)
        add_dependencies(cpuid_deprecated_api_example_cpp aoclutils au_cpuid)
        add_dependencies(cpuid_all_flags_example_cpp aoclutils au_internal_core)
    endif()
    if(au_core_Logger)
        add_dependencies(logger_demo_cpp aoclutils au_internal_core)
    endif()
    if(au_core_ThreadPinning)
        add_dependencies(thread_pinning_example_cpp aoclutils au_internal_core)
    endif()
endif()

add_subdirectory(capi)
