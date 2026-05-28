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

include(CMakeDependentOption)

option(AU_BUILD_TESTS "Enable the tests." OFF)
option(AU_BUILD_DOCS "Generate Docs during build" OFF)
option(AU_BUILD_EXAMPLES "Enable examples" OFF)
option(AU_ENABLE_SLOW_TESTS "Option to Enable SLOW tests" OFF)
option(AU_ENABLE_BROKEN_TESTS "Option to Enable BROKEN tests" OFF)
option(AU_ENABLE_ASSERTIONS "Enable asserts in the code" OFF)
option(AU_ENABLE_OLD_API "Enable OLD alci_* APIs" OFF)
option(AU_BUILD_WITH_ASAN "Enable ASAN Options on build" OFF)
option(AU_BUILD_WITH_TSAN "Enable TSAN Options on build" OFF)
option(AU_BUILD_WITH_MEMSAN "Enable MEMSAN Options on build" OFF)
option(AU_ENABLE_CODE_COVERAGE "Enable Code coverage on build" OFF)
# Default the stock CMake BUILD_SHARED_LIBS switch to ON so this project
# behaves canonically: callers using the standard CMake idiom get a shared
# library by default, while -DBUILD_SHARED_LIBS=OFF cleanly disables it.
# Declaring it via option() also types the cache entry (avoids the
# :UNINITIALIZED label) and is a no-op if a parent project or the user
# already set it on the command line.
option(BUILD_SHARED_LIBS "Build shared libraries by default" ON)

# AOCL policy: the static library is built by default. Users may still opt
# out explicitly with -DAU_BUILD_STATIC_LIBS=OFF.
option(AU_BUILD_STATIC_LIBS "Build static libraries (AOCL policy: default ON)" ON)

# AU_BUILD_SHARED_LIBS follows BUILD_SHARED_LIBS, so toggling the standard
# switch also toggles the shared library build here. AU_BUILD_SHARED_LIBS
# can still be set explicitly to override that default per build.
option(AU_BUILD_SHARED_LIBS "Build shared libraries" ${BUILD_SHARED_LIBS})

# Catch the contradictory combination explicitly. Without this check the
# value the user passed on -D wins silently (option() is a no-op when the
# cache entry already exists) and the build behaves opposite to what the
# canonical BUILD_SHARED_LIBS switch implied.
if(NOT BUILD_SHARED_LIBS AND AU_BUILD_SHARED_LIBS)
    message(FATAL_ERROR
        "Conflicting options: BUILD_SHARED_LIBS=OFF disables shared library "
        "builds, but AU_BUILD_SHARED_LIBS=ON requests one. Re-run cmake with "
        "-DBUILD_SHARED_LIBS=ON to build the shared library, or drop "
        "-DAU_BUILD_SHARED_LIBS=ON to honor BUILD_SHARED_LIBS=OFF.")
endif()

option(AU_CMAKE_VERBOSE "Set cmake verbosity" OFF)

# Sub options for docs
# Doxygen generate graphs and enable code browsing
option(AU_DOXYGEN_FULL "Generate Doxygen documentation with all features" OFF)

# Define the processor type
string(TOUPPER "${CMAKE_SYSTEM_PROCESSOR}" upper_CMAKE_SYSTEM_PROCESSOR)
cmake_dependent_option(AU_CPU_ARCH_X86 "" OFF "${upper_CMAKE_SYSTEM_PROCESSOR} MATCHES ^x86" ON)


# Lets make the build
string(TOUPPER "${CMAKE_BUILD_TYPE}" upper_CMAKE_BUILD_TYPE)
if (CMAKE_BUILD_TYPE AND
    NOT upper_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|DEVELOPER|RELWITHDEBINFO)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# Handle deprecated APIs
if(${AU_ENABLE_OLD_API})
    set(AU_WARN_DEPRECATION FALSE)
else()
    set(AU_WARN_DEPRECATION TRUE)
endif()

cmake_dependent_option(AU_BUILD_TYPE_RELEASE "" ON "upper_CMAKE_BUILD_TYPE STREQUAL RELEASE" OFF)
cmake_dependent_option(AU_BUILD_TYPE_DEBUG "" ON "upper_CMAKE_BUILD_TYPE STREQUAL DEBUG" OFF)
cmake_dependent_option(AU_BUILD_TYPE_DEVELOPER "" ON "upper_CMAKE_BUILD_TYPE STREQUAL DEVELOPER" OFF)
cmake_dependent_option(AU_BUILD_TYPE_RELWITHDEBINFO "" ON "upper_CMAKE_BUILD_TYPE STREQUAL RELWITHDEBINFO" OFF)

mark_as_advanced(AU_BUILD_TYPE_RELEASE
	AU_BUILD_TYPE_DEBUG
	AU_BUILD_TYPE_DEVELOPER
    AU_BUILD_TYPE_RELWITHDEBINFO
    AU_ENABLE_ASSERTIONS
    AU_WARN_DEPRECATION)

if (AU_CMAKE_VERBOSE AND FALSE)
message(
	"build type \n"
	"release:" ${AU_BUILD_TYPE_RELEASE} "\n"
	"debug:" ${AU_BUILD_TYPE_DEBUG} "\n"
	"developer:" ${AU_BUILD_TYPE_DEVELOPER} "\n"
    "relwithdebinfo:" ${AU_BUILD_TYPE_RELWITHDEBINFO} "\n"
	"assertions: " ${AU_ENABLE_ASSERTIONS})
endif()


if (AU_BUILD_TYPE_RELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/Release)
elseif(AU_BUILD_TYPE_DEVELOPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Developer)
elseif(AU_BUILD_TYPE_DEBUG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)
elseif(AU_BUILD_TYPE_RELWITHDEBINFO)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/RelWithDebInfo)
endif()
