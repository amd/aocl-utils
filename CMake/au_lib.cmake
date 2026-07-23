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

# ---------------------------------------------------------------------------
# Target-naming convention (identical on Linux and Windows), defined here ONCE
# and assumed by the comments below:
#   <base>         -> SHARED library  (aoclutils        -> libaoclutils.so/.dll)
#   <base>_static  -> STATIC library  (aoclutils_static -> libaoclutils.a/_static.lib)
# Umbrella base is "aoclutils"; sub-modules are au_<mod> (public) or
# au_internal_<mod> (private). The Windows "lib" filename prefix is applied via a
# target PROPERTY (PREFIX/IMPORT_PREFIX) on the umbrella only, never baked into
# the target name. The au::<mod> alias points at the _static target.
# ---------------------------------------------------------------------------

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
    set(_shared_dep "")
    if(TARGET ${_dep})
      # The au::<mod> alias resolves to the _static target, so an au:: dep passed
      # here would drag /MT static objects into this /MD shared DLL and resurface
      # the MSVC CRT (RuntimeLibrary) mismatch. Resolve the alias via
      # ALIASED_TARGET and map it to its shared sibling (the bare name).
      get_target_property(_aliased ${_dep} ALIASED_TARGET)
      if(_aliased)
        # Alias (e.g. au::core -> au_internal_core_static) -> bare name.
        string(REGEX REPLACE "_static$" "" _bare "${_aliased}")
        if(TARGET ${_bare})
          set(_shared_dep ${_bare})
        endif()
      else()
        # Non-alias in-project target: map a _static name to the bare name.
        string(REGEX REPLACE "_static$" "" _bare "${_dep}")
        if(TARGET ${_bare})
          set(_shared_dep ${_bare})
        endif()
      endif()
    endif()
    if(_shared_dep)
      list(APPEND _resolved ${_shared_dep})
    else()
      # External/system dep, header-only INTERFACE, or a dep with no shared
      # (bare) variant: pass through unchanged so the link still resolves.
      list(APPEND _resolved ${_dep})
    endif()
  endforeach()
  set(${out_var} "${_resolved}" PARENT_SCOPE)
endfunction()

macro(setlibname NAME isPublic __target_name)
  # Target names per the convention at the top of this file.
  if(${NAME} STREQUAL "aoclutils")
      set(__target_name "aoclutils")
  elseif(${isPublic})
      set(__target_name "au_${NAME}")
  else()
      set(__target_name "au_internal_${NAME}")
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

  set(libaoclutils "aoclutils")  # umbrella base name (see convention at top)

  if(${AU_BUILD_STATIC_LIBS})
    if (TARGET ${libaoclutils}_static)
        target_sources(${libaoclutils}_static PRIVATE ${${fPrefix}_SOURCES} ${${fPrefix}_HEADERS})
    endif()
  endif()
  if(${AU_BUILD_SHARED_LIBS})
    if (TARGET ${libaoclutils})
        target_sources(${libaoclutils}  PRIVATE ${${fPrefix}_SOURCES} ${${fPrefix}_HEADERS})
    endif()
  endif()

  if (DEFINED tmp_src_list)
    set(isPublic ${cclib_PUBLIC})
    setlibname(${NAME} ${isPublic} __target_name)

    if(${AU_BUILD_STATIC_LIBS})
        # STATIC target = "<base>_static". OUTPUT_NAME keeps the shipped file
        # byte-identical: Unix "<base>" -> libaoclutils.a; Windows
        # "<base>_static" + PREFIX "lib" -> libaoclutils_static.lib.
        if(UNIX)
          set(static_output_name ${__target_name})
        else()
          set(static_output_name ${__target_name}_static)
        endif()
        add_library(${__target_name}_static STATIC "")
        target_sources(${__target_name}_static
	        PRIVATE
	        ${${fPrefix}_SOURCES}
    	    ${${fPrefix}_HEADERS})

        target_link_libraries(${__target_name}_static
	        PUBLIC ${cclib_DEPENDS}
        )

        set_target_properties(${__target_name}_static
	        PROPERTIES
	        CXX_STANDARD ${AU_CXX_STANDARD}
	        CXX_STANDARD_REQUIRED true
	        INCLUDE_DIRECTORIES "${AU_INCLUDE_DIRS}"
            OUTPUT_NAME ${static_output_name}
        )
        # The Windows "lib" filename prefix belongs ONLY to the umbrella
        # (libaoclutils_static.lib). Sub-module archives (au_cpuid_static.lib)
        # never had it -- downstream consumers (aocl-crypto) link them by the
        # bare name, so do NOT prefix them.
        if(WIN32 AND "${__target_name}" STREQUAL "aoclutils")
            set_target_properties(${__target_name}_static PROPERTIES PREFIX "lib")
        endif()
    endif()
    if(${AU_BUILD_SHARED_LIBS})
        # SHARED target = BARE name. OUTPUT_NAME "<base>" + PREFIX "lib" on
        # Windows -> libaoclutils.dll + import libaoclutils.lib; Unix ->
        # libaoclutils.so.
        add_library(${__target_name} SHARED "")
        target_sources(${__target_name}
	        PRIVATE
	        ${${fPrefix}_SOURCES}
    	    ${${fPrefix}_HEADERS}
        )
        # Link the shared (bare) variant of any in-project au:: dependency so the
        # shared DLL's CRT graph stays /MD end-to-end (see au_resolve_shared_deps).
        au_resolve_shared_deps("${cclib_DEPENDS}" _shared_depends)
        target_link_libraries(${__target_name}
	           PUBLIC ${_shared_depends}
        )
        set_target_properties(${__target_name}
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
        # The Windows "lib" prefix belongs ONLY to the umbrella target: its DLL is
        # libaoclutils.dll (PREFIX) and its import lib libaoclutils.lib
        # (IMPORT_PREFIX -- named separately from PREFIX). Sub-modules (au_cpuid)
        # ship bare au_cpuid.dll / au_cpuid.lib as before the rename; aocl-crypto
        # links au_cpuid.lib by that exact name, so must NOT be prefixed.
        if(WIN32 AND "${__target_name}" STREQUAL "aoclutils")
            set_target_properties(${__target_name} PROPERTIES
                PREFIX "lib" IMPORT_PREFIX "lib")
        endif()
    endif()
  else()
    add_library(${__target_name} INTERFACE)
    target_include_directories(${__target_name} INTERFACE ${AU_INCLUDE_DIRS})
  endif()

  # install targets that are not internal
  if (cclib_PUBLIC)
    if(UNIX)
        if(${AU_BUILD_STATIC_LIBS})
            install(TARGETS ${__target_name}_static EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
                INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
        )
        endif()
        if(${AU_BUILD_SHARED_LIBS})
            install(TARGETS ${__target_name} EXPORT ${AU_INSTALL_EXPORT_NAME}
                  RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                  LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                  ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
                  INCLUDES DESTINATION ${AU_INSTALL_INCLUDE_DIR}
          )
        endif()
    else()
        if(${AU_BUILD_STATIC_LIBS})
            install(TARGETS ${__target_name}_static EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
            )
        endif()
        if(${AU_BUILD_SHARED_LIBS})
            install(TARGETS ${__target_name} EXPORT ${AU_INSTALL_EXPORT_NAME}
                RUNTIME DESTINATION ${AU_INSTALL_BIN_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_BIN_DIR}
            )
            # Second (non-EXPORT) install of the SAME shared target into lib/.
            # This is NOT dead cruft: the EXPORT install above records the import
            # lib under bin/ (ARCHIVE -> BIN_DIR), but downstream consumers that
            # link by explicit path expect the import lib (and DLL) under
            # <prefix>/lib/ -- the aocl-crypto Windows presub links
            # lib/libaoclutils.lib + lib/au_cpuid.lib, so removing this drops
            # those files and breaks its lld-link step. No EXPORT here (the target
            # is already in the export set) so it adds files only, not a second
            # export entry. Mirrors the pre-existing green install layout, which
            # shipped both the DLL and the import lib into lib/ as well.
            install(TARGETS ${__target_name}
                RUNTIME DESTINATION ${AU_INSTALL_LIB_DIR}
                LIBRARY DESTINATION ${AU_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${AU_INSTALL_ARCHIVE_DIR}
            )
        endif()
    endif()
  endif()
  # The au::<mod> alias keeps pointing at the STATIC target on ALL platforms
  # (byte-identical to prior behavior): in-tree /MT tests/examples must not pull
  # in the /MD shared DLL. Fall back to the bare (shared) target only when no
  # static variant is built.
  if (${AU_BUILD_STATIC_LIBS})
      add_library(au::${AU_MODULE} ALIAS ${__target_name}_static)
  elseif (${AU_BUILD_SHARED_LIBS})
      add_library(au::${AU_MODULE} ALIAS ${__target_name})
  endif()

endfunction(au_cc_library)
