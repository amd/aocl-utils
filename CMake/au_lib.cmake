#
# Copyright (C) 2022-2023, Advanced Micro Devices. All rights reserved.
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

macro(au_normalize_name name var)
  STRING(REPLACE "::" "__" ${var} ${name})
endmacro()


# au_add_interface(<name> PACKAGE <package>
#   [INCLUDES include...]
# )
#
function(au_add_interface NAME)
  set(fPrefix cciface)
  set(fOptions PUBLIC)
  set(fOneArg )
  set(fMultiArgs HEADERS)

  cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
  )
  if (NOT DEFINED AU_MODULE)
	  #message(FATAL_ERROR "af module name not defined")
	  set(AU_MODULE af)
  endif()

  au_normalize_name(${NAME} __new_name)
  add_library(${__new_name} INTERFACE)

  if(cciface_PUBLIC)
	  set(__target_name "au_${__new_name}")
  else()
	  set(__target_name "au_internal_${__new_name}")
  endif()

  target_include_directories(${__new_name} INTERFACE ${cciface_HEADERS})

  add_library(${AU_MODULE}::${NAME} ALIAS ${__new_name})
endfunction(au_add_interface)

# Produces a static library that can be linked into other test, binary or
# library targets.
#
# Arguments:
#   NAME      base name of the target. See below for target naming conventions.
#   SOURCES   list of source files, including headers.
#   DEPENDS   list of dependency libraries
#   PUBLIC    flag, intended for external use.
#
# If SRCS contains only headers, an INTERFACE rule is created. This rule carries
# include path and link library information, but is not directly buildable.
#
# The corresponding build target is named au_<MODULE>_<NAME> if PUBLIC is
# specified, or au_internal_<MODULE>_<NAME> otherwise. An alias is also defined
# for use in CMake scripts, in the form af::<MODULE>::<NAME> or
# af::internal::<MODULE>::<NAME>
#
function(au_cc_library NAME)
  set(fPrefix cclib)
  set(fOptions PUBLIC;TEST_ONLY)
  set(fOneArg )
  set(fMultiArgs SOURCES;HEADERS;DEPENDS;USING)

  if (NOT DEFINED AU_MODULE)
    message(FATAL_ERROR "af module name not defined")
  endif()

  cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
    )

    # Check if header-only library
    set(tmp_src_list ${${fPrefix}_SOURCES})
    list(FILTER tmp_src_list INCLUDE REGEX "\\.cc$")
    #message("sources: " ${tmp_src_list})

    if(cclib_PUBLIC)
      set(__target_name "au_${NAME}")
    else()
      set(__target_name "au_internal_${NAME}")
    endif()

    if (DEFINED tmp_src_list)
      add_library(${__target_name} STATIC "")
      target_sources(${__target_name}
	      PRIVATE
	      ${${fPrefix}_SOURCES}
      	      ${${fPrefix}_HEADERS})
      #target_include_directories(${__target_name}
      #	      PUBLIC ${AU_INCLUDE_DIRS})
      target_link_libraries(${__target_name}
	      PUBLIC ${cclib_DEPENDS}
      )
      set_target_properties(${__target_name}
	      PROPERTIES
	      CXX_STANDARD ${AU_CXX_STANDARD}
	      CXX_STANDARD_REQUIRED true
	      INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
      )
      #target_link_libraries(af::all PUBLIC ${__target_name})
      add_library(${__target_name}_shared SHARED "")
      target_sources(${__target_name}_shared
	      PRIVATE
	      ${${fPrefix}_SOURCES}
      	      ${${fPrefix}_HEADERS})
      #target_include_directories(${__target_name}
      #	      PUBLIC ${AU_INCLUDE_DIRS})
      target_link_libraries(${__target_name}_shared
	      PUBLIC ${cclib_DEPENDS}
      )
      set_target_properties(${__target_name}_shared
	      PROPERTIES
	      CXX_STANDARD ${AU_CXX_STANDARD}
	      CXX_STANDARD_REQUIRED true
	      INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
          OUTPUT_NAME ${__target_name}
      )
    else()
      add_library(${__target_name} INTERFACE)
      target_include_directories(${__target_name} INTERFACE ${AU_INCLUDE_DIRS})
      #target_link_libraries(${__target_name} INTERFACE ${cclib_DEPENDS})
    endif()
    if(UNIX)
      install(TARGETS ${__target_name} ${__target_name}_shared EXPORT ${AU_INSTALL_EXPORT_NAME}
              RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
              LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
              ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
      )
    else()
      install(TARGETS ${__target_name}  EXPORT ${AU_INSTALL_EXPORT_NAME}
              RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
              LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
              ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
      )
      install(TARGETS ${__target_name}_shared  EXPORT ${AU_INSTALL_EXPORT_NAME}
              RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
              LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
              ARCHIVE DESTINATION ${AU_INSTALL_BIN_DIR}
      )
    endif()
    add_library(au::${AU_MODULE} ALIAS ${__target_name})

endfunction(au_cc_library)
