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

# Declare the beginning of a new namespace.
#
# As a rule of thumb, every CMakeLists.txt should be a different module, named
# after the directory that contains it, and this function should appear at the
# top of each CMakeLists script. Multiple directories can be part of the same
# module as long as target names do not collide.
#

include(CMakeParseArguments)

# Declare a test using googletest.
#
# Parameters:
#   NAME           name of the test.
#   SOURCES        list of test source files, headers included.
#   HEADERS        list of headers to depend on
#   DEPENDS        list of dependencies
#   CONTENTS       list of non-code dependencies, such as test vectors.
#
# Attributes:
#   BROKEN            Known to fail
#   SKIP              Dont add to targets
#   HANGING           Test might hang(such as looking for Entropy)
#   WINDOWS_DISABLED  Dont run on Windows (Why ? you'll know)
#
# Tests added with this macro are automatically registered.
# Each test produces a build target named au_test_<MODULE>_<NAME>.
#
#
#   au_cc_test(
#     DIRECTORY test/
#       TEST BufferTest WINDOWS_DISABLED
#         SOURCES BufferTest.cc
#         HEADERS BufferTest.hh
#         CONTENTS data/
#   )

function(au_cc_test testName)

  if (NOT AU_BUILD_TESTS)
    return()
  endif()

  if (NOT DEFINED AU_MODULE)
    message(FATAL_ERROR "af module name not defined")
  endif()

  au_normalize_name(${AU_MODULE} _ESCAPED_AU_MODULE)

  set(fPrefix test)
  set(fOptions BROKEN;SKIP;WINDOWS_DISABLED)
  set(fOneArg CONTENTS;DIRECTORY)
  set(fMultiArgs SOURCES;HEADERS;DEPENDS;EXTLIBRARY)

  cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
    )

  set(_target_name "${_ESCAPED_AU_MODULE}_${testName}")

  if ( ${${fPrefix}_SKIP} OR ${${fPrefix}_BROKEN} )
    message("Test : " ${testName} "[SKIPPED]")
    return()
  endif()

  if ( ${${fPrefix}_WINDOWS_DISABLED} AND WIN32)
    message("Test : " ${testName} "[WIN32-DISABLED]")
    return()
  endif()

  if ( ${${fPrefix}_SLOW} )
    if (STREQUAL "${au_ENABLE_SLOW_TESTS}" "OFF" )
      message("Test : " ${testName} "[SKIPPED] SLOW")
      return()
    endif()
  endif()

  # message(
  #   " taget-"     ${_target_name}
  #   " SOURCES-"   ${${fPrefix}_SOURCES}
  #   " DIRECTORY-" ${${fPrefix}_DIRECTORY}
  #   " DEPENDS-"   ${${fPrefix}_DEPENDS}
  #   " CONTENTS-"  ${${fPrefix}_CONTENTS}
  # )

  if (${fPrefix}_DIRECTORY)
    add_executable(${_target_name}
            ${${fPrefix}_DIRECTORY}/${${fPrefix}_SOURCES})
  else()
      add_executable(${_target_name}  ${${fPrefix}_SOURCES})
  endif()

  target_include_directories(${_target_name}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "${AU_INCLUDE_DIRS}"
  )

  # Add gtest with main() as dependency
  target_link_libraries(${_target_name} PRIVATE gmock_main)

  #
  # Add additional dependencies,
  # au_base will be added towards end by default
  #
  list(APPEND ${fPrefix}_DEPENDS au::core)
  if(${fPrefix}_DEPENDS)
      target_link_libraries(${_target_name} PRIVATE ${${fPrefix}_DEPENDS})
  endif()
  if(${fPrefix}_EXTLIBRARY)
      find_package(${${fPrefix}_EXTLIBRARY} COMPONENTS Development REQUIRED)
      target_include_directories(${_target_name} PRIVATE ${${${fPrefix}_EXTLIBRARY}_INCLUDE_DIRS})
      target_link_libraries(${_target_name} PRIVATE ${${${fPrefix}_EXTLIBRARY}_LIBRARIES})
  endif()
  #message("cxx standard" ${AU_CXX_STANDARD})
  set_target_properties(
    ${_target_name}
      PROPERTIES
      CXX_STANDARD ${AU_CXX_STANDARD}
      CXX_STANDARD_REQUIRED true
      #TIMEOUT 120
    )

  #message("Adding Test: " ${_target_name})

  if (HAVE_CMAKE_GTEST)
    # If we have CMake's built-in gtest support use it to add each test
    # function as a separate test.
    gtest_add_tests(TARGET ${_target_name}
                    WORKING_DIRECTORY  "${CMAKE_BINARY_DIR}"
                    AUTO)
    set_tests_properties(${test_cases} PROPERTIES TIMEOUT 120)
  else()
    # Otherwise add each test executable as a single test.
    add_test(NAME ${_target_name}
            COMMAND ${_target_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
          )
          #set_tests_properties(
          #ctest_run_test_code
          #PROPERTIES DEPENDS ${_target_name})
  endif()

# TODO: Set the CONTENTS directory and copy its contents to ${CMAKE_BINARY_DIR}
endfunction(au_cc_test)
