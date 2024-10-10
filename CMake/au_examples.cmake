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

# au_add_application(<name> PACKAGE <package>
#   SOURCES source...
#   HEADERS headers...
#   [LIBS component...]
# )
#
if(NOT DEFINED AU_CXX_STANDARD)
  set(AU_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD ${AU_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(au_add_application NAME)
  set(fPrefix APP)
  set(fOptions )
  set(fOneArg )
  set(fMultiArgs "INCLUDES;SOURCES;HEADERS;LIBS")

  cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
  )

  #cmake_parse_arguments(APP "" "PACKAGE" "SOURCES;COMPONENTS" ${ARGN})

  add_executable(${NAME} ${APP_SOURCES})
  add_executable(${APP_PACKAGE}::${NAME} ALIAS ${NAME})

  if(DEFINED APP_LIBS)
    target_link_directories(${NAME} PRIVATE ${CMAKE_BINARY_DIR}/Library)
    target_link_libraries(${NAME}
      PRIVATE
        ${APP_LIBS}
    )
  endif()
  if(DEFINED APP_INCLUDES)
      target_include_directories(${NAME} PRIVATE ${APP_INCLUDES})
  endif()

endfunction(au_add_application)
