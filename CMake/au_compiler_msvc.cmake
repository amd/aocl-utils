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

# Project-wide MSVC CRT baseline: /MT (static CRT). This is the default for
# EVERY target -- the static library, unit tests, examples, and fetched
# googletest -- so they form one consistent /MT graph and link cleanly against
# libaoclutils_static.lib (which the au:: alias resolves to in a both-libs
# build). The shared library is the ONLY exception: au_lib.cmake overrides the
# shared target (and its _shared dependency edges) to /MD via a per-target
# MSVC_RUNTIME_LIBRARY property, because a DLL must share the CRT/heap with its
# /MD consumers.
#
# History: keying this global off BUILD_SHARED_LIBS (the previous approach)
# painted the static archive /MD in the default both-libs build (BUILD_SHARED_LIBS
# defaults ON while AU_BUILD_STATIC_LIBS is also ON), which broke every /MT
# downstream consumer at link time (e.g. aocl-sparse). A bare per-target attempt
# that dropped this global broke the other way: tests fell back to CMake's /MD
# default and could not link the /MT static lib. /MT-default-here +
# /MD-override-on-shared is the only shape that satisfies both at once.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Note that CMAKE_REQUIRED_FLAGS must be a string, not a list
#set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=${CXX_STD}")
