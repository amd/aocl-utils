#
# Copyright (C) 2022-2026, Advanced Micro Devices. All rights reserved.
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

# Resolve a DEPENDS list to the shared-linkage variant of any in-project au::
# module. The shared library must link the _shared build of its au:: deps, not
# the static target the au::<module> alias points at in a both-libs build --
# otherwise a /MD shared DLL drags in /MT static-target objects and the MSVC
# CRT (RuntimeLibrary) mismatch resurfaces internally. External/system deps and
# header-only INTERFACE deps are passed through unchanged.
#
#   in_deps : the raw DEPENDS list (e.g. au::aoclutils;SomeExternal::lib)
#   out_var : name of the variable to receive the shared-resolved list
function(au_resolve_shared_deps in_deps out_var)
  set(_resolved "")
  foreach(_dep IN LISTS in_deps)
    if(_dep MATCHES "^au::(.+)$")
      set(_mod "${CMAKE_MATCH_1}")
      # The shared build target for module <mod> is <static-target>_shared.
      # Reuse setlibname so the static/internal naming stays the single source
      # of truth (libaoclutils, au_<mod>, au_internal_<mod>).
      setlibname(${_mod} TRUE _dep_target)
      if(TARGET ${_dep_target}_shared)
        list(APPEND _resolved ${_dep_target}_shared)
      else()
        # No shared variant built for this dep (e.g. static-only submodule):
        # fall back to the alias so the link still resolves.
        list(APPEND _resolved ${_dep})
      endif()
    else()
      list(APPEND _resolved ${_dep})
    endif()
  endforeach()
  set(${out_var} "${_resolved}" PARENT_SCOPE)
endfunction()

macro(setlibname NAME isPublic __target_name)
  if(UNIX)
    # set the target name if it is public or the name not equal to aoclutils
    if(${isPublic})
      if(${NAME} STREQUAL "aoclutils")
          set(__target_name "aoclutils")
      else()
          set(__target_name "au_${NAME}")
      endif()
    else()
      if(${NAME} STREQUAL "aoclutils")
          set(__target_name "aoclutils")
      else()
          set(__target_name "au_internal_${NAME}")
      endif()
    endif()
  else()
    if(${isPublic})
      if(${NAME} STREQUAL "aoclutils")
          set(__target_name "libaoclutils")
      else()
          set(__target_name "au_${NAME}")
      endif()
    else()
      if(${NAME} STREQUAL "aoclutils")
          set(__target_name "libaoclutils")
      else()
          set(__target_name "au_internal_${NAME}")
      endif()
    endif()
  endif()
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
	  #message(FATAL_ERROR "au module name not defined")
	  set(AU_MODULE au)
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
# for use in CMake scripts, in the form au::<MODULE>::<NAME> or
# au::internal::<MODULE>::<NAME>
#
function(au_cc_library NAME)
  set(fPrefix cclib)
  set(fOptions PUBLIC;TEST_ONLY)
  set(fOneArg )
  set(fMultiArgs SOURCES;HEADERS;DEPENDS;USING)

  if (NOT DEFINED AU_MODULE)
    message(FATAL_ERROR "au module name not defined")
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

  if(UNIX)
    set(libaoclutils "aoclutils")
  else()
    set(libaoclutils "libaoclutils")
  endif()

  if(${AU_BUILD_STATIC_LIBS})
    if (TARGET ${libaoclutils})
        target_sources(${libaoclutils} PRIVATE ${${fPrefix}_SOURCES} ${${fPrefix}_HEADERS})
    endif()
  endif()
  if(${AU_BUILD_SHARED_LIBS})
    if (TARGET ${libaoclutils}_shared)
        target_sources(${libaoclutils}_shared  PRIVATE ${${fPrefix}_SOURCES} ${${fPrefix}_HEADERS})
    endif()
  endif()

  if (DEFINED tmp_src_list)
    set(isPublic ${cclib_PUBLIC})
    setlibname(${NAME} ${isPublic} __target_name)

    if(UNIX)
      set(output_name ${__target_name})
    else()
      set(output_name ${__target_name}_static)
    endif()
    if(${AU_BUILD_STATIC_LIBS})
        add_library(${__target_name} STATIC "")
        target_sources(${__target_name}
	        PRIVATE
	        ${${fPrefix}_SOURCES}
    	    ${${fPrefix}_HEADERS})

        target_link_libraries(${__target_name}
	        PUBLIC ${cclib_DEPENDS}
        )

        set_target_properties(${__target_name}
	        PROPERTIES
	        CXX_STANDARD ${AU_CXX_STANDARD}
	        CXX_STANDARD_REQUIRED true
	        INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
            OUTPUT_NAME ${output_name}
        )
    endif()
    if(${AU_BUILD_SHARED_LIBS})
        #target_link_libraries(au::all PUBLIC ${__target_name})
        add_library(${__target_name}_shared SHARED "")
        target_sources(${__target_name}_shared
	        PRIVATE
	        ${${fPrefix}_SOURCES}
    	    ${${fPrefix}_HEADERS}
        )
        # Link the _shared variant of any in-project au:: dependency so the
        # shared DLL's CRT graph stays /MD end-to-end (see au_resolve_shared_deps).
        au_resolve_shared_deps("${cclib_DEPENDS}" _shared_depends)
        target_link_libraries(${__target_name}_shared
	           PUBLIC ${_shared_depends}
        )
        set_target_properties(${__target_name}_shared
	        PROPERTIES
	        CXX_STANDARD ${AU_CXX_STANDARD}
	        CXX_STANDARD_REQUIRED true
	        INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
            OUTPUT_NAME ${__target_name}
            # Shared lib: /MD. The DLL must share the CRT/heap with its /MD
            # consumers (e.g. aocl-crypto); an embedded /MT CRT splits the heap
            # and corrupts cross-DLL frees. The static lib + tests stay on the
            # /MT global default from au_compiler_msvc.cmake.
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
  else()
    add_library(${__target_name} INTERFACE)
    target_include_directories(${__target_name} INTERFACE ${AU_INCLUDE_DIRS})
  endif()

  # install targets that are not internal
  if (cclib_PUBLIC)
    if(UNIX)
        if(${AU_BUILD_STATIC_LIBS})
            install(TARGETS ${__target_name} EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
                INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
        )
        endif()
        if(${AU_BUILD_SHARED_LIBS})
            install(TARGETS ${__target_name}_shared EXPORT ${AU_INSTALL_EXPORT_NAME}
                  RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                  LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                  ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
                  INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
          )
        endif()
    else()
        if(${AU_BUILD_STATIC_LIBS})
            install(TARGETS ${__target_name} EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
            )
            install(TARGETS ${__target_name} EXPORT
                RUNTIME DESTINATION ${AU_INSTALL_LIB_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
            )
        endif()
        if(${AU_BUILD_SHARED_LIBS})
            install(TARGETS ${__target_name}_shared EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_BIN_DIR}
            )
            install(TARGETS ${__target_name}_shared
                RUNTIME DESTINATION ${AU_INSTALL_LIB_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
            )
        endif()
    endif()
  endif()
  if (${AU_BUILD_STATIC_LIBS} AND ${AU_BUILD_SHARED_LIBS})
      add_library(au::${AU_MODULE} ALIAS ${__target_name})
  elseif (${AU_BUILD_STATIC_LIBS})
      add_library(au::${AU_MODULE} ALIAS ${__target_name})
  elseif (${AU_BUILD_SHARED_LIBS})
      add_library(au::${AU_MODULE} ALIAS ${__target_name}_shared)
  endif()

endfunction(au_cc_library)
