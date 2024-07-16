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

macro(au_module NAME)
  set(AU_MODULE ${NAME})
  list(FIND AU_MODULES_LIST ${NAME} EXISTS__)
  #message("Called from " ${CMAKE_CURRENT_LIST_FILE} " With name: " ${NAME}
  #  " EXISTS: " ${EXISTS__})
  if (${EXISTS__} GREATER -1)
    message(SEND_ERROR "Module ${NAME} already exists")
  endif()
  list(APPEND AU_MODULES_LIST ${NAME})
  unset(EXISTS__)
endmacro()

#
# au_install_with_dir(
#   DESTINATION <dest>
#   [FILES files...])
function (au_install_with_dir NAME)
  set(fPrefix INST)
  set(fOptions )
  set(fOneArg DESITNATION)
  set(fMultiArgs FILES)

  cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
   )

   foreach(FILE ${INST_FILES})
     get_filename_component(DIR ${FILE} DIRECTORY)
     install(FILES ${FILE} DESTINATION ${INST_DESTINATION}/${DIR})
   endforeach()

endfunction(au_install_with_dir)

if (UNIX)
  if(APPLE)
    set(AU_TARGET_OS_IS_MACOS TRUE)
  elseif(${CMAKE_HOST_UNIX})
    set(AU_TARGET_OS_IS_LINUX TRUE) # Linux/FreeBSD/Solaris
  endif()
elseif(WIN32)
  set(AU_TARGET_OS_IS_WINDOWS TRUE)
endif()
