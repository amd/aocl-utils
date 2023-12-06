#
# Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

set(AUROOT /opt/aocl CACHE PATH "AOCL Utils root")

if(NOT "${AUROOT}/CMake/au_examples.cmake")
  message("AUROOT is \" ${AUROOT} \" ")
  message("Please set AUROOT correctly to compile examples")
endif()

list(PREPEND CMAKE_MODULE_PATH 
  "${AUROOT}/lib/cmake" 
)
list(PREPEND CMAKE_PREFIX_PATH
  "${AUROOT}" 
)
#set(CMAKE_MODULE_PATH
#  "${AUROOT}/lib/cmake" 
#   ${CMAKE_MODULE_PATH}
#)

set(aocl-utils_DIR ${AUROOT}/lib/cmake)
find_package(aocl-utils)
get_property(importTargetsAfter DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
list(REMOVE_ITEM importTargetsAfter ${importTargets})

message("${importTargetsAfter}")

include(au_examples CONFIG)

project(aocl_utils_examples LANGUAGES CXX)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${AUROOT}/Include
    ${AUROOT}/include
    )

link_directories(
    ${AUROOT}/3rdparty
    ${AUROOT}/lib
    )

au_add_application(
	cpuid_example
	SOURCES cpuid_example.cc
    LIBS au::au_cpuid
)


