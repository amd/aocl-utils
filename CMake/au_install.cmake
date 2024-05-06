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

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/SDK/Include/
  DESTINATION include
  FILES_MATCHING REGEX ".*\\.(h|hh)")

#install(TARGETS ${AU_INSTALL_TARGETS}
#  EXPORT ${AU_INSTALL_EXPORT_NAME}
#  LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
#  ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
#  RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
#  PUBLIC_HEADER DESTINATION ${AU_INSTALL_INCLUDE_DIR}
#    COMPONENT runtime
#)

if (UNIX)
  install(CODE "execute_process(COMMAND ldconfig)"
          COMPONENT runtime
  )
endif()

install(EXPORT ${AU_INSTALL_EXPORT_NAME}
  DESTINATION ${AU_INSTALL_CMAKE_CONFIG_DIR}
  NAMESPACE au::
  FILE ${AU_INSTALL_CMAKE_CONFIG_NAME}
)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/CMake/
  DESTINATION ${AU_INSTALL_LIB_DIR}/cmake
  FILES_MATCHING PATTERN "*.cmake"
)

message("Installing... " ${AU_CONFIG_OUTPUT_FILE})
install(
  FILES ${AU_CONFIG_OUTPUT_FILE}
  DESTINATION ${AU_INSTALL_INCLUDE_DIR}/Au
)

message("Installing Version.txt... ")
install(
  FILES version.txt
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
#To ensure backward compatibility with older versions of AOCL UTILS
message("Installing headers... ")
if(EXISTS ${CMAKE_SOURCE_DIR}/SDK/Include/Capi/au/cpuid/cpuid.h)
    install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci)
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Capi/au/cpuid/cpuid.h \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/arch.h \
    )")
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Capi/au/cpuid/cpuid.h \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/alci_c.h \
    )")
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Capi/au/au.h \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/alci.h \
    )")
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Capi/au/enum.h \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/enum.h \
    )")
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Capi/au/macros.h \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/macros.h \
    )")
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/SDK/Include/Au/Cpuid/X86Cpu.hh)
    install(DIRECTORY DESTINATION
        ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/cxx)
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Au/Cpuid/Enum.hh \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/cxx/enum.h \
    )")
    install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/Au/Cpuid/Enum.hh \
    ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_INCLUDE_DIR}/alci/cxx/cpu.hh \
    )")
endif()
message("Installing binaries... ")
if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    set(DEBUG_POSTFIX "-dbg")
endif()

if (UNIX)
    if (EXISTS ${CMAKE_BINARY_DIR}/Library/Cpuid)
        install(CODE "execute_process( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libau_cpuid${DEBUG_POSTFIX}.so \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libaoclutils.so \
        )")
        install(CODE "execute_process( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libau_cpuid${DEBUG_POSTFIX}.a \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libaoclutils.a \
        )")
    endif()
else()
    if (EXISTS ${CMAKE_BINARY_DIR}/Library/Cpuid)
        install(CODE "file(COPY_FILE \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/au_cpuid${DEBUG_POSTFIX}.lib \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libaoclutils_static.lib \
        )")
        install(CODE "file( COPY_FILE \
          ${CMAKE_INSTALL_PREFIX}/bin/au_cpuid${DEBUG_POSTFIX}.lib \
          ${CMAKE_INSTALL_PREFIX}/${AU_INSTALL_LIB_DIR}/libaoclutils.lib \
        )")
    endif()
endif(UNIX)

#install(TARGETS console
#  RUNTIME COMPONENT runtime
#)

#
# CPACK configuration
#
set(CPACK_PACKAGE_NAME                  "${PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION           "AOCL Foundations")
set(CPACK_PACKAGE_VENDOR                "AMD")
set(CPACK_PACKAGE_DESCRIPTION_FILE      "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     "/usr/local")
set(CPACK_PACKAGE_VERSION_MAJOR         ${AU_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${AU_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${AU_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION               ${AU_VERSION_STRING})

include(CPack)
