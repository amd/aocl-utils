#
# Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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
cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

# CMake extensions include directory
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  # Add the default path
  ${CMAKE_MODULE_PATH}
)

# Define AU variables before we do anything else
include(au_variables)

# Parse and define features
include(au_features)

# Export to CMakePresets.json, which is used by various IDE,
# easy integration on any IDE using LSP (Language Server Protocol)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Determine versions and its components
#
include(au_version)

# Read only well-formed version lines so blank lines, trailing newlines, and
# comment lines in version.txt do not break parsing. The regex matches the
# same MAJOR.MINOR.PATCH-[suffix] format au_extract_version_details expects,
# so loose strings (e.g. 1.2.3.4 or "1.2.3 foo") are rejected here rather
# than silently leaving AU_VERSION_* unset later.
file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" AU_VERSION_LINES
      REGEX "^[0-9]+\\.[0-9]+\\.[0-9]+-")
if (NOT AU_VERSION_LINES)
    message(FATAL_ERROR "No valid version line found in version.txt")
endif()
list (GET AU_VERSION_LINES 0 AU_VERSION_FROM_FILE)
string (STRIP "${AU_VERSION_FROM_FILE}" AU_VERSION_FROM_FILE)
au_extract_version_details("AU_VERSION" "${AU_VERSION_FROM_FILE}")
if (NOT DEFINED AU_VERSION_MAJOR)
    message(FATAL_ERROR "Failed to parse version from version.txt: '${AU_VERSION_FROM_FILE}'")
endif()

set(AU_VERSION_STRING "${AU_VERSION_MAJOR}.${AU_VERSION_MINOR}.${AU_VERSION_PATCH}")
if(AU_VERSION_SUFFIX)
   set(AU_VERSION_STRING "${AU_VERSION_STRING}.${AU_VERSION_SUFFIX}${AU_VERSION_SUFFIX_EXTRA}")
endif()

set(PACKAGE            "libau")
set(PACKAGE_NAME       "au")
set(PACKAGE_STRING     "${PACKAGE_NAME} ${AU_VERSION_STRING}")
set(PACKAGE_TARNAME    "${PACKAGE_NAME}-${AU_VERSION_STRING}")
set(PACKAGE_BUGREPORT  ${AU_VENDOR_CONTACT})
set(PACKAGE_URL        ${AU_VENDOR_URL})
set(PACKAGE_VERSION    "${AU_VERSION_STRING}")

set(PROJECT_FULL_NAME "AOCL Utilities Library")
set(PROJECT_NAME      ${AU_PROJECT_NAME})
message("version " ${AU_VERSION_STRING})
project(${PACKAGE_NAME} VERSION ${AU_VERSION_STRING} LANGUAGES CXX C ASM)

# Declare project dependant variables
include(au_project)

#
# Set language features
# Require C++-20
set(CMAKE_CXX_STANDARD ${AU_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(AU_COMPILER_IS_GNU 0)
set(AU_COMPILER_IS_CLANG 0)
set(AU_COMPILER_IS_MSVC 0)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        message(FATAL_ERROR "GCC version must be at least 8!")
    else()
        set(AU_COMPILER_IS_GNU 1)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 12
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12.0)
        message(FATAL_ERROR "Clang version must be at least 12.0!")
    else()
        set(AU_COMPILER_IS_CLANG 1)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("MSVC version: " ${CMAKE_CXX_COMPILER_VERSION})
    # require at least MSVC 19.29
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29)
        message(FATAL_ERROR "MSVC version must be at least 19.29!")
    else()
        set(AU_COMPILER_IS_MSVC 1)
    endif()

else()
    message(WARNING "Unsupported compiler! Compilation has
    only been tested with above versions.")
endif()

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release"  CACHE STRING
      "Default build type: Release" FORCE )
endif()

if(NOT CMAKE_CXX_CLANG_TIDY)
  find_program(CLANG_TIDY "clang-tidy")
  # NOTFOUND is CMake-falsy; avoids MATCHES on an unquoted path (can have
  # spaces on Windows, e.g. "Program Files").
  if(CLANG_TIDY)
    if (MSVC)
      # clang-tidy runs as Clang even over a cl.exe command line, so on
      # clang-tidy <= 18 it needs -mrdrnd itself to see _rdrand16_step
      # (cl.exe doesn't; Library/CMakeLists.txt withholds it there).
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};--extra-arg=-EHsc")
      # -mrdrnd is x86-only; only add it for clang-tidy on an x86/x64 target.
      string(TOUPPER "${CMAKE_SYSTEM_PROCESSOR}" _AU_CLANG_TIDY_ARCH)
      if (_AU_CLANG_TIDY_ARCH MATCHES "^(X86|I[3-6]86|X86_64|AMD64)$")
        list(APPEND CMAKE_CXX_CLANG_TIDY "--extra-arg=-mrdrnd")
      endif()
      unset(_AU_CLANG_TIDY_ARCH)
    else()
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
    endif()
  endif()
endif()

include(TestBigEndian)
test_big_endian(AU_TARGET_TEST_BIG_ENDIAN)
if(AU_TARGET_TEST_BIG_ENDIAN)
set(AU_TARGET_IS_BIG_ENDIAN TRUE)
set(AU_TARGET_IS_LITTLE_ENDIAN FALSE)
else()
set(AU_TARGET_IS_BIG_ENDIAN FALSE)
set(AU_TARGET_IS_LITTLE_ENDIAN TRUE)
endif()

include(au_options)
add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
add_compile_definitions(PROJECT_BUILD_DIR="${PROJECT_BINARY_DIR}")

if(MSVC)
  include(au_compiler_msvc)
else()
  include(au_compiler_unix)
endif()
if(AU_ENABLE_CODE_COVERAGE)
    add_compile_options(-O0 -fprofile-arcs -ftest-coverage)
    add_link_options(-lgcov --coverage)
endif()
if (AU_BUILD_WITH_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()
if (AU_BUILD_WITH_TSAN)
  if (NOT AU_BUILD_WITH_ASAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
  else()
    message(WARNING "TSAN cannot be enabled with ASAN. Disabling TSAN.")
    set(AU_BUILD_WITH_TSAN OFF)
  endif()
endif()
if (AU_BUILD_WITH_MEMSAN)
  if (AU_COMPILER_IS_CLANG)
    add_compile_options(-fsanitize=memory)
    add_link_options(-fsanitize=memory)
  else()
    message(WARNING "MEMSAN can only be enabled with Clang. Disabling MEMSAN.")
    set(AU_BUILD_WITH_MEMSAN OFF)
  endif()
endif()
# Fetch googletest if "testing" is enabled
if (AU_BUILD_TESTS)
  enable_testing()
  include(au_find_gtest)
  include(au_unit_tests)
  # AU_BUILD_TESTS is deliberately NOT added here with add_compile_definitions:
  # that is directory-scoped and would bake the macro into the production
  # libraries (both the combined libaoclutils and the C-only libaoclutils_c),
  # shipping any test-only seam guarded by it. It is instead defined PRIVATE on
  # each test executable in au_cc_test(), so it reaches test binaries only.
endif()

option(INSTALL_GTEST OFF)

if (AU_BUILD_EXAMPLES)
  include(au_examples)
endif()

include(au_git_hash)
include(au_functions)
include(au_helpers)

if(NOT AU_BUILD_SHARED_LIBS AND NOT AU_BUILD_STATIC_LIBS)
    # nothing to build raise warning and exit
    message(WARNING "Nothing to build. Please set AU_BUILD_SHARED_LIBS and/or AU_BUILD_STATIC_LIBS to ON")
    return()
endif()
# Config.h generation
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Library/config.h.in" "${AU_CONFIG_OUTPUT_FILE}" @ONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Library/version.cc.in" "${AU_VERSION_OUTPUT_FILE}" @ONLY)

include(au_lib)

au_add_interface(sdk__include
  PUBLIC
)
set_target_properties(sdk__include
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/SDK/Include"
)

add_subdirectory(Library)

if (AU_BUILD_TESTS)
  add_subdirectory(Tests)
endif()
if (AU_BUILD_DOCS)
  add_subdirectory(docs)
endif()

if (AU_BUILD_EXAMPLES)
  add_subdirectory(SDK/Examples)
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/aocl-utils.pc.in
  ${PROJECT_BINARY_DIR}/aocl-utils.pc
  @ONLY
)
#
# Print a banner if verbosity is enabled
#
if(AU_CMAKE_VERBOSE AND TRUE)
  message(STATUS "Project                : ${PROJECT_FULL_NAME}")
  message(STATUS "  Description          : ${AU_PROJECT_DESCRIPTION}")
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

  if (AU_BUILD_WITH_ASAN)
  message(STATUS "  ASAN                 : " "Enabled")
  else()
  message(STATUS "  ASAN                 : " "Disabled")
  endif()

  if (AU_BUILD_WITH_TSAN)
  message(STATUS "  TSAN                 : " "Enabled")
  else()
  message(STATUS "  TSAN                 : " "Disabled")
  endif()

  if (AU_BUILD_WITH_MEMSAN)
  message(STATUS "  MEMSAN               : " "Enabled")
  else()
  message(STATUS "  MEMSAN               : " "Disabled")
  endif()

  if (AU_BUILD_DOCS)
  message(STATUS "  Docs                 : " "Enabled")
  else()
  message(STATUS "  Docs                 : " "Off")
  endif()
  if (AU_ENABLE_ASSERTIONS)
  message(STATUS "  Assertions           : " "Enabled")
  else()
  message(STATUS "  Assertions           : " "Disabled")
  endif()
  if (AU_ENABLE_OLD_API)
  message(STATUS "  Deprecated APIs Warning  : " "Disabled")
  else()
  message(STATUS "  Deprecated APIs Warning  : Enabled")
  endif()

  message(STATUS "  CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX})
  message(STATUS "  CMAKE_GENERATOR      : " ${CMAKE_GENERATOR})
  message(STATUS "  CMAKE_C_COMPILER_ID  : " ${CMAKE_C_COMPILER_ID})
  message(STATUS "  CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
endif()

include(au_install)
