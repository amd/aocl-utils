#
# Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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

function(au_module_exists arg var)
list(FIND "${arg}" AU_MODULES_LIST var)
if (${var} GREATER -1)
  set(var TRUE)
else()
  unset(var)
endif()
endfunction()

# au_add_component(<name> PACKAGE <package>
#   SOURCES source...
#   [INCLUDES include...]
#   [USING dependency...]
#   [SHARING dependency...]
#   [TESTS test...]
# )
#
function(au_add_component NAME)
  set(fPrefix COMP)
  set(fOptions )
  set(fOneArg PACKAGE)
  set(fMultiArgs "INCLUDES;SOURCES;TESTS;USING;SHARING")

  cmake_parse_arguments(
    ${fPrefix} 
    "${fOptions}" 
    "${fOneArg}"
    "${fMultiArgs}" 
    ${ARGN}
  )

  add_library(${NAME} ${COMP_SOURCES})
  add_library(${COMP_PACKAGE}::${NAME} ALIAS ${NAME})

  if(DEFINED COMP_INCLUDES)
    target_include_directories(${NAME} PUBLIC ${COMP_INCLUDES})
  endif()

  if(DEFINED COMP_SHARING)
    target_link_libraries(${NAME} PUBLIC ${COMP_SHARING})
  endif()

  if(DEFINED COMP_USING)
    target_link_libraries(${NAME} PRIVATE ${COMP_USING})
  endif()

  if(DEFINED COMP_TESTS)
    # add_executable(${NAME}-tests ${COMP_TESTS})
    # add_executable(tests::${NAME} ALIAS ${NAME}-tests)

    # add_custom_target(run-${NAME}-tests ALL COMMAND tests::${NAME})

    # target_link_libraries(${NAME}-tests
    #   PRIVATE
    #     ${COMP_PACKAGE}::${NAME}
    #     ${COMP_USING}
    #     #${AU_MODULE}::base
    #     gtest
    # )
    #target_include_directories(${NAME}-tests PRIVATE src)
  endif()

  #target_link_libraries(${NAME} PRIVATE )
endfunction(au_add_component)


