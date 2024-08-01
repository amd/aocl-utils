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
    DIRECTORY ${PROJECT_SOURCE_DIR}/SDK/Include/
  DESTINATION include
  FILES_MATCHING REGEX ".*\\.(h|hh)")

# for backward compatibility.
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/SDK/Bcl/
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
    DIRECTORY ${PROJECT_SOURCE_DIR}/CMake/
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
message("Installing binaries... ")
if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    set(DEBUG_POSTFIX "-dbg")
endif()


#
# CPACK configuration
#
set(CPACK_PACKAGE_NAME                  "${PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION           "AOCL Foundations")
set(CPACK_PACKAGE_VENDOR                "AMD")
set(CPACK_PACKAGE_DESCRIPTION_FILE      "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_RESOURCE_FILE_README          "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_VERSION_MAJOR         ${AU_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${AU_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${AU_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION               ${AU_VERSION_STRING})

include(CPack)
