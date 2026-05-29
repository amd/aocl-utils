#
# Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
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

set(AU_CXX_FLAGS_COMMON "-W4 -Wextra" "-fno-rtti" "-fno-exceptions")
set(AU_CXX_FLAGS_DEBUG
        "-Wsuggest-final-types"
        "-Wsuggest-final-methods"
        "-Wsuggest-override"
        "-g")
set(AU_CXX_FLAGS_RELEASE "-O3" "-Wno-unused")

list(APPEND CXX_FLAGS_DEBUG "${AU_CXX_FLAGS_COMMON} ${AU_CXX_FLAGS_DEBUG}")
list(APPEND CXX_FLAGS_RELEASE "${AU_CXX_FLAGS_COMMON} ${AU_CXX_FLAGS_RELEASE}")

# Project-wide MSVC CRT selection, keyed off the canonical BUILD_SHARED_LIBS
# switch (which au_options.cmake declares with a default of ON). Setting this
# globally ensures the library targets AND every test/example consumer get
# the same CRT, avoiding the "RuntimeLibrary mismatch" link failures the CI
# caught when this was set per-target while tests inherited the CMake
# default of /MD. With BUILD_SHARED_LIBS=ON everything uses /MD (so consumers
# like aocl-crypto built with /MD can link cleanly). With BUILD_SHARED_LIBS
# =OFF, AU_BUILD_SHARED_LIBS is forced OFF too (only the static lib is built)
# and everything uses /MT for a self-contained build.
if(BUILD_SHARED_LIBS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Note that CMAKE_REQUIRED_FLAGS must be a string, not a list
#set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=${CXX_STD}")
