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

# We use the GoogleTest module if it is available (only in CMake 3.9+)
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# CMake extensions include directory
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  # Add the default path
  ${CMAKE_MODULE_PATH}
  )

include(au_variables)
include(au_functions)
include(au_helpers)

# Export to CMakePresets.json, which is used by various IDE,
# easy integration on any IDE using LSP (Language Server Protocol)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Determine versions and its components
#
include(au_version)
file (STRINGS ${CMAKE_SOURCE_DIR}/version.txt AU_VERSION_FROM_FILE)
au_extract_version_details("AU_VERSION" "${AU_VERSION_FROM_FILE}")

set(AU_VERSION_STRING "${AU_VERSION_MAJOR}.${AU_VERSION_MINOR}.${AU_VERSION_PATCH}")
if(AU_VERSION_SUFFIX)
   set(AU_VERSION_STRING "${AU_VERSION_STRING}.${AU_VERSION_SUFFIX}${AU_VERSION_SUFFIX_EXTRA}")
endif()

set(PACKAGE           "libau")
set(PACKAGE_NAME      "au")
set(PACKAGE_STRING    "${PACKAGE_NAME} ${AU_VERSION_STRING}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${AU_VERSION_STRING}")
set(PACKAGE_BUGREPORT "aocl@amd.com")
set(PACKAGE_URL       "https://developer.amd.com/aocl/")
set(PACKAGE_VERSION   "${AU_VERSION_STRING}")

set(PROJECT_FULL_NAME "AOCL Utilities Library")
set(PROJECT_NAME "AOCL Library")
project(${PACKAGE_NAME} VERSION ${AU_VERSION_STRING} LANGUAGES CXX C ASM)

#
# Set language features
# Require C++-17
set(CMAKE_CXX_STANDARD ${AU_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release"  CACHE STRING
      "Default build type: Release" FORCE )
endif()

if(NOT CMAKE_CXX_CLANG_TIDY)
  find_program(CLANG_TIDY "clang-tidy")
  if(NOT ${CLANG_TIDY} MATCHES "CLANG_TIDY-NOTFOUND")
     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
  endif()
endif()

include(TestBigEndian)
test_big_endian(AU_TAGET_TEST_BIG_ENDIAN)
if(AU_TARGET_TEST_BIG_ENDIAN)
set(AU_TARGET_IS_BIG_ENDIAN TRUE)
set(AU_TARGET_IS_LITTLE_ENDIAN FALSE)
else()
set(AU_TARGET_IS_BIG_ENDIAN FALSE)
set(AU_TARGET_IS_LITTLE_ENDIAN TRUE)
endif()

include(au_options)
add_compile_definitions(PROJECT_BUILD_DIR="${CMAKE_BINARY_DIR}")
add_compile_definitions(PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if(MSVC)
  include(au_compiler_msvc)
else()
  include(au_compiler_unix)
endif()

# Fetch googletest if "testing" is enabled
if (AU_BUILD_TESTS)
  enable_testing()
  include(au_find_gtest)
  include(au_unit_tests)
endif()

option(INSTALL_GTEST OFF)

if (AU_BUILD_EXAMPLES)
  include(au_examples)
endif()

# Config.h generation
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Library/config.h.in" "${AU_CONFIG_OUTPUT_FILE}" @ONLY)
include_directories("${AU_CONFIG_DIR}")

set(AU_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/SDK/Include
  ${CMAKE_SOURCE_DIR}/Library/Private
  ${AU_CONFIG_DIR}
)

include(au_lib)

au_add_interface(sdk__include
  PUBLIC
)
set_target_properties(sdk__include
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/SDK/Include"
)


add_subdirectory(Library)

if (AU_BUILD_TESTS)
  add_subdirectory(Tests)
endif()
if (AU_BUILD_DOCS)
  add_subdirectory(Docs)
endif()

if (AU_BUILD_EXAMPLES)
    add_subdirectory(SDK)
endif()

#
# Print a banner if verbosity is enabled
#
if(AU_CMAKE_VERBOSE AND TRUE)
  message(STATUS "Project : ${PROJECT_FULL_NAME}")
  message(STATUS "  Description          : ${AU_DESCRIPTION}")
  message(STATUS "  Version              : ${AU_VERSION_STRING} (${AU_VERSION_FULL})")
  message(STATUS "  Build Type           : ${CMAKE_BUILD_TYPE}")
  if (AU_BUILD_EXAMPLES)
  message(STATUS "  Examples             : " "Enabled")
  else()
  message(STATUS "  Examples             : " "Off")
  endif()

  if (AU_BUILD_TESTS)
  message(STATUS "  Tests                : " "Enabled")
  else()
  message(STATUS "  Tests                : " "Off")
  endif()

  if (AU_BUILD_DOCS)
  message(STATUS "  Docs                 : " "Enabled")
  else()
  message(STATUS "  Docs                 : " "Off")
  endif()
  if (AU_WARN_DEPRECATED_APIS)
      message(STATUS "  Deprecated APIs Warning  : " "Enabled")
  else()
      message(STATUS "  Deprecated APIs Warning  : Disabled")
  endif()

  message(STATUS "  CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX})
  message(STATUS "  CMAKE_GENERATOR      : " ${CMAKE_GENERATOR})
  message(STATUS "  CMAKE_C_COMPILER_ID  : " ${CMAKE_C_COMPILER_ID})
  message(STATUS "  CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
endif()

include(au_install)
