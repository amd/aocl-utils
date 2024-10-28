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

#set(AU_MODULE "core")
au_module("core")

if (MSVC)
	add_compile_options(-W4  /Za /DLL)
else()
	add_compile_options(-Wall -Weffc++ -pedantic -fPIC)
endif()

set(LOGGER_SRC_FILES
    Logger.cc
    LoggerFactory.cc
)

set(BASE64_SRC_FILES
    Base64/Base64.cc
    Base64/Base64Encoder.cc
    Base64/Base64Decoder.cc
)

set(MEMORY_SRC_FILES
    Memory/BufferView.cc
)

set(MESSAGE_SRC_FILES
)

set(EVENT_SRC_FILES
)

set(CAPI_SRC_FILES
    #Capi/base64.cc
    Capi/environ.cc
    Capi/threadpinning.cc
        )

set(AVX2_FILES
  Base64/transform_avx2.cc
)

set_source_files_properties(
  ${AVX2_FILES}
  PROPERTIES
    COMPILE_FLAGS "-mavx2"
)

set(CORE_SRC_FILES
  #Core/Buffer.cc
  Core/Status.cc
  Core/GenericError.cc
  Core/SourceLocation.cc
  Core/StatusOr.cc
  Core/Environ.cc
  Core/BitMap.cc
  Core/ThreadPinningImpl.cc
  Core/ThreadPinning.cc
  #Core/DynamicLoader.cc
  #${BASE64_SRC_FILES}
  #  ${LOGGER_SRC_FILES}
  #${MEMORY_SRC_FILES}
  #  ${MESSAGE_SRC_FILES}
  #  ${EVENT_SRC_FILES}
  ${CAPI_SRC_FILES}
  #${AVX2_FILES}
)

au_cc_library(core
  PRIVATE
  SOURCES
    ${CORE_SRC_FILES}
  HEADERS
    Core/ThreadPinningImpl.hh
  # FIXME: "USING" is not used in au_cc_library function.
  USING
    au::sdk__include
)


au_module("aoclutils")
au_cc_library(aoclutils
  PUBLIC
  SOURCES
      ${CORE_SRC_FILES}
  HEADERS
      Core/ThreadPinningImpl.hh
  # FIXME: "USING" is not used in au_cc_library function.
  USING
      au::sdk__include
)

if(AU_BUILD_TESTS)
    add_subdirectory(Tests)
endif()

set(AU_SUBMODULE_DIRS
  Cpuid
  #Base64
  #Provider
)

foreach(__moddir ${AU_SUBMODULE_DIRS})
  add_subdirectory(${__moddir})
endforeach()
