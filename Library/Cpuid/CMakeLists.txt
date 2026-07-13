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
