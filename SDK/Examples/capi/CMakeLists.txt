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

project(cpuid_example_c)

# Only build CPUID examples if feature is enabled
if(au_cpuid_cpuid)
    au_add_application(cpuid_example_c
                    SOURCES cpuid_example.c
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})

    au_add_application(cpuid_deprecated_api_example_c
                    SOURCES cpuid_deprecated_api_example.c
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
endif()

# Only build Logger examples if feature is enabled
if(au_core_Logger)
    set_source_files_properties(logger_demo.c PROPERTIES LANGUAGE CXX)
    au_add_application(logger_demo_c
                    SOURCES logger_demo.c
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
endif()

# Only build ThreadPinning examples if feature is enabled
if(au_core_ThreadPinning)
    if (NOT WIN32)
        au_add_application(thread_pinning_example_c
                    SOURCES thread_pinning_example_linux.c
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
    else()
        au_add_application(thread_pinning_example_c
                    SOURCES thread_pinning_example_windows.c
                    LIBS ${AU_LIBS}
                    INCLUDES ${AU_INCLUDE_DIRS})
    endif()
endif()

if(AU_BUILD_EXAMPLES AND AU_BUILD_SHARED_LIBS)
    if(WIN32)
        if(au_cpuid_cpuid)
            add_dependencies(cpuid_example_c libaoclutils_shared au_cpuid_shared)
            add_dependencies(cpuid_deprecated_api_example_c libaoclutils_shared au_cpuid_shared)
        endif()
        if(au_core_Logger)
            add_dependencies(logger_demo_c libaoclutils_shared)
        endif()
        if(au_core_ThreadPinning)
            add_dependencies(thread_pinning_example_c libaoclutils_shared)
        endif()
    else()
        if(au_cpuid_cpuid)
            add_dependencies(cpuid_example_c aoclutils_shared au_cpuid_shared)
            add_dependencies(cpuid_deprecated_api_example_c aoclutils_shared au_cpuid_shared)
        endif()
        if(au_core_Logger)
            add_dependencies(logger_demo_c aoclutils_shared)
        endif()
        if(au_core_ThreadPinning)
            add_dependencies(thread_pinning_example_c aoclutils_shared)
        endif()
    endif()
endif()
