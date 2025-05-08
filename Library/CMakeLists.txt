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
#

#set(AU_MODULE "core")
au_module("core")

if (MSVC)
	add_compile_options(-W4  /Za /DLL)
else()
	add_compile_options(-Wall -Weffc++ -pedantic -fPIC)
endif()

SET(LOGGER_SRC_FILES "Core/Logger/LogWriter.cc"
                     "Core/Logger/Logger.cc"
                     "Core/Logger/LoggerManager.cc"
                     "Core/Logger/Message.cc"
                     "Core/Logger/Queue.cc"
                     # CAPIs
                     "Capi/logger.cc"
)

SET(THREAD_PINNING_SRC_FILES "Core/ThreadPinning.cc"
                             "Core/ThreadPinningImpl.cc"
                             # CAPIs
                             "Capi/threadpinning.cc"
)

set(BASE64_SRC_FILES
    Base64/Base64.cc
    Base64/Base64Encoder.cc
    Base64/Base64Decoder.cc
    # CAPIs
    #Capi/base64.cc
)

set(MEMORY_SRC_FILES
    Memory/BufferView.cc
)

set(MESSAGE_SRC_FILES
)

set(EVENT_SRC_FILES
)

set(RNG_SRC_FILES
    Core/Rng/SystemRng.cc
    Core/Rng/HardwareRng.cc
)

set_source_files_properties(
  Core/Rng/HardwareRng.cc
  PROPERTIES
    COMPILE_FLAGS "-mrdrnd"
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
  #Core/DynamicLoader.cc
  # CAPIS
  Capi/environ.cc
)

# Initialize empty source lists
set(UTILS_SRC_FILES "")

# Add core Status files
if(au_core_Status)
    list(APPEND UTILS_SRC_FILES
        Core/Status.cc
        Core/GenericError.cc
        Core/SourceLocation.cc
        Core/StatusOr.cc
    )
endif()

# Add ThreadPinning if enabled
if(au_core_ThreadPinning)
    list(APPEND UTILS_SRC_FILES
        ${THREAD_PINNING_SRC_FILES}
    )
endif()

# Add Logger if enabled
if(au_core_Logger)
    list(APPEND UTILS_SRC_FILES
        ${LOGGER_SRC_FILES}
    )
endif()

# Add RNG if enabled
if(au_core_RNG)
    list(APPEND UTILS_SRC_FILES
        ${RNG_SRC_FILES}
    )
endif()

# Always include these core files
list(APPEND UTILS_SRC_FILES
    Core/Environ.cc
    Core/BitMap.cc
    Capi/environ.cc
)

# Build core library
au_cc_library(core
    PRIVATE
    SOURCES
        ${UTILS_SRC_FILES}
    HEADERS
        Core/ThreadPinningImpl.hh
    USING
        au::sdk__include
)

# Build aoclutils library
au_module("aoclutils")
au_cc_library(aoclutils
    PUBLIC
    SOURCES
        ${UTILS_SRC_FILES}
    HEADERS
        Core/ThreadPinningImpl.hh
    USING
        au::sdk__include
)

# Only add enabled submodules
set(AU_SUBMODULE_DIRS "")

# Check if CPUID is enabled
if(au_cpuid_cpuid)
    list(APPEND AU_SUBMODULE_DIRS "Cpuid")
endif()

if(AU_BUILD_TESTS)
    add_subdirectory(Tests)
endif()

foreach(__moddir ${AU_SUBMODULE_DIRS})
    add_subdirectory(${__moddir})
endforeach()
