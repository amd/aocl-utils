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

set(BASE_TEST_FILES
    #DynamicLoaderTest.cc
    ErrorTest.cc
    EnvTest.cc
    StatusTest.cc
    StatusOrTest.cc
    #BitOpsTest.cc
    BitMapTest.cc
)

set(MEMORY_TEST_FILES
    #Memory/LocalBufferTest.cc
    #Memory/BufferViewTest.cc
)

set(CPUID_TEST_FILES
    Cpuid/CpuidTest.cc
    Cpuid/CapiTest.cc
    Cpuid/Mock/CpuidUtilsTest.cc
    Cpuid/Mock/X86CpuTest.cc
    Cpuid/CpuidQemuTest.cc
)

set(OTHER_TEST_FILES
)

set(THREAD_PINNING_TEST_FILES
    ThreadPinning/ThreadPinningTest.cc
    ThreadPinning/ThreadPinningCapiTest.cc
    ThreadPinning/ThreadPinningMockTest.cc
)
set(ALL_TEST_FILES
    ${MEMORY_TEST_FILES}
    ${BASE_TEST_FILES}
    ${OTHER_TEST_FILES}
    ${THREAD_PINNING_TEST_FILES}
)

# Single test Files gets added in a Loop, more complex ones are below
# message(" Total Test Files: " ${ALL_TEST_FILES})
foreach(testFile IN LISTS ALL_TEST_FILES)
    get_filename_component(__TestName ${testFile} NAME_WE)
    get_filename_component(__TestFile ${testFile} NAME)

    #cmake_path(GET testFile ROOT_NAME __path_root_name)
    #message("path_root_name - " ${__path_root_name})
    #cmake_path(GET testFile ROOT_DIRECTORY __path_root_dir)
    #message("path_root_dir - " ${__path_root_dir})
    #cmake_path(GET testFile ROOT_PATH __path_root_path)
    #message("path_root_path - " ${__path_root_path})
    #cmake_path(GET testFile FILENAME __path_filename)
    #message("path_filename - " ${__path_filename})
    #cmake_path(GET testFile STEM LAST_ONLY __path_stem)
    #message("path_stem - " ${__path_stem})
    #cmake_path(GET testFile RELATIVE_PART __path_relative)
    #message("path_relative - " ${__path_relative})
    #cmake_path(GET testFile PARENT_PATH __path_parent)
    #message("path_parent - " ${__path_parent})

    cmake_path(GET testFile PARENT_PATH __parent_path)
    #message("parent_path - " ${__parent_path})
    cmake_path(
        RELATIVE_PATH __parent_path
        BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE __TestDir
    )
    # message("Test-> " ${__TestName}
    #        " file-> " ${__TestFile}
    #        " dir -> " ${__TestDir}
    # )
    au_cc_test(${__TestName}
        SOURCES  "${__TestFile}"
        DIRECTORY ${__parent_path}
        DEPENDS
        au::aoclutils
        #au_provider
    )
endforeach()

##########
# Provider Tests
##########

#list(FIND "provider" AU_MODULES_LIST EXISTS__)
#if (${EXISTS__} GREATER -1)
au_module_exists("provider" EXISTS__)
# message("Exists " ${EXISTS__})
if (${EXISTS__})
    set(PROVIDER_TEST_FILES
        ProviderTest.cc
    )
    set(PROVIDER_DIR "Provider")
    set(PRVDR_FILES ${PROVIDER_TEST_FILES})
    list(TRANSFORM PRVDR_FILES PREPEND "${PROVIDER_DIR}/")
    set_source_files_properties(
        ${PRVDR_FILES}
        PROPERTIES
        #DIRECTORY "Provider"
        INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Library"
    )

    au_cc_test(ProviderTest
        DIRECTORY
        ${PROVIDER_DIR}
        SOURCES
        ${PROVIDER_TEST_FILES}
        DEPENDS
        au::provider
    )
endif()

#au_cc_test(Base64Test
#    DIRECTORY
#        .
#    SOURCES
#        Base64Test.cc
#    DEPENDS
#        au::base64
#)

au_cc_test(CpuidTest
  SOURCES
  ${CPUID_TEST_FILES}
  HEADERS Mock/MockTest.hh QemuTest.hh
  DEPENDS
  au::cpuid
  EXTLIBRARY
  Python3
)
##########
# Dynamic Loader Tests
##########

# au_cc_test(DynLoadTest
#         #WINDOWS_DISABLED
#         DIRECTORY   tests/
#         SOURCES     tests/DynamicLoaderTest.cc
#         #HEADERS DynamicLoader.hh
#         DEPENDS     _module_lib
# )

add_subdirectory(Status)
