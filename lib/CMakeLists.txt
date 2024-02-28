##Copyright (C) 2023, Advanced Micro Devices, Inc.##
SET(ALCI_LIB_SRCS 
      cpuidrawdata.cc
      cpu.cc
      cache.cc
      ostream.cc
      capi/c_arch.cc
      capi/c_cache.cc
      capi/c_error.cc)

IF (BUILD_SHARED_LIBS)
ADD_LIBRARY(aoclutils SHARED
      ${ALCI_LIB_SRCS}
      )
ENDIF()

ADD_LIBRARY(aoclutils_static STATIC
      ${ALCI_LIB_SRCS}
      )

IF(UNIX)
  SET_TARGET_PROPERTIES(aoclutils_static PROPERTIES OUTPUT_NAME aoclutils)
  TARGET_COMPILE_OPTIONS(aoclutils_static PRIVATE "-fPIC")
ELSEIF(WIN32)
  IF (BUILD_SHARED_LIBS)
    # Set target properties for shared library to have common name for .lib and .dll
    SET_TARGET_PROPERTIES(aoclutils PROPERTIES ARCHIVE_OUTPUT_NAME "${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}")
    
    # Flag to append _DEBUG for .dll installation in debug mode.
    IF ((${CMAKE_BUILD_TYPE} STREQUAL Debug) OR (${CMAKE_BUILD_TYPE} STREQUAL DEBUG))
      SET(AOCL_APPEND_BUILDTYPE_FOR_DLL "_DEBUG")
    ELSE()
      SET(AOCL_APPEND_BUILDTYPE_FOR_DLL "")
    ENDIF()
    # Install .dll into lib directory to have .lib and .dll in same path for shared library
    INSTALL(FILES "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${AOCL_APPEND_BUILDTYPE_FOR_DLL}.dll"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF()
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    )
