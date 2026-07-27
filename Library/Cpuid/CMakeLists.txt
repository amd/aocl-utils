#
# Copyright (C) 2023-2026, Advanced Micro Devices. All rights reserved.
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

set(AU_MODULE "cpuid")

set(CPUID_SRC_FILES
  CacheInfo.cc
  Cache.cc
  Cpuid.cc
  CpuidUtils.cc
  X86RawData.cc
  ../Capi/cpuid.cc
  X86Cpu.cc
  # X86Cpu::buildFromCore returns StatusOr<X86Cpu>, which references the
  # Status/Error implementation (StatusInvalidArgument, NoError,
  # InvalidArgumentError, ...). Compile those sources straight into the cpuid
  # library so it is self-contained.
  #
  # We deliberately do NOT do this via DEPENDS au::aoclutils: au::aoclutils is
  # the combined "umbrella" target that the cpuid sources are themselves folded
  # into (see Library/CMakeLists.txt), so linking cpuid against it makes cpuid
  # pull in a library that already contains cpuid's own objects -- a duplicate
  # symbol (e.g. Au::CpuInfo::CpuInfo) that lld-link rejects as a hard error
  # (GNU ld and MSVC link.exe silently tolerate it, which is why only the
  # clang-cl/lld Windows CI leg caught it). au::core cannot be used either: it
  # is the internal, non-exported target au_internal_core, so a public cpuid
  # depending on it breaks install(EXPORT). Folding the sources in sidesteps
  # both problems.
  ../Core/Status.cc
  ../Core/GenericError.cc
  ../Core/SourceLocation.cc
)
au_cc_library(cpuid
PUBLIC
SOURCES
    ${CPUID_SRC_FILES}

HEADERS
X86RawData.hh
)

#target_link_libraries(au_all PUBLIC af::cpuid)

# ---------------------------------------------------------------------------
# libaoclutils_c : C-only CPUID CAPI library (NO libstdc++)
#
# Pilot for the packaging split (PR #268). A single pure-C translation unit
# (Library/Capi/cpuid_c_export.c) includes the header-only CPUID core with the
# export attribute set, so it emits the SAME au_cpuid_* / alci_* symbols the
# combined libaoclutils exports -- but compiled as C, so the object drags in
# zero libstdc++. Built ALONGSIDE the combined library; the combined library is
# left untouched.
#
# Diamond linkage: shared consumers can interpose either library (symbol
# identity prevents conflict); static consumers must pick exactly one archive.
# ---------------------------------------------------------------------------
set(AOCLUTILS_C_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../Capi/cpuid_c_export.c")

# Force C compilation even though the file lives beside C++ sources.
set_source_files_properties(${AOCLUTILS_C_SRC} PROPERTIES LANGUAGE C)

if(AU_BUILD_SHARED_LIBS)
    add_library(aoclutils_c_shared SHARED ${AOCLUTILS_C_SRC})
    set_target_properties(aoclutils_c_shared PROPERTIES
        OUTPUT_NAME       aoclutils_c
        C_STANDARD        11
        C_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
        # Do NOT let CMake attach the C++ runtime: this must stay libstdc++-free.
        LINKER_LANGUAGE   C
        INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
    )
    install(TARGETS aoclutils_c_shared EXPORT ${AU_INSTALL_EXPORT_NAME}
        RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
        INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
    )
endif()

if(AU_BUILD_STATIC_LIBS)
    add_library(aoclutils_c_static STATIC ${AOCLUTILS_C_SRC})
    set_target_properties(aoclutils_c_static PROPERTIES
        OUTPUT_NAME       aoclutils_c
        C_STANDARD        11
        C_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
        LINKER_LANGUAGE   C
        INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
    )
    install(TARGETS aoclutils_c_static EXPORT ${AU_INSTALL_EXPORT_NAME}
        RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
        INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
    )
endif()
