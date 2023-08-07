##Copyright (C) 2023, Advanced Micro Devices, Inc.##

CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT (aoclutils)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING
     "Choose the type of build, options are: Debug Release"
     FORCE)
ENDIF()

# ADDING INCLUDE DIRECTORY
INCLUDE_DIRECTORIES(INCLUDE)

# SPECIFY THE C STANDARD
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD_REQUIRED TRUE)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

IF(UNIX)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wall -Werror -Wpedantic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -Wall -Werror -Wpedantic")
ELSEIF(WIN32)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11 -Wall -Wpedantic")
# Disable C++98 compatibility warnings.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 -Wall -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic")
# Prefix all shared libraries with 'lib'.
SET(CMAKE_SHARED_LIBRARY_PREFIX "lib")
# Prefix all static libraries with 'lib'.
SET(CMAKE_STATIC_LIBRARY_PREFIX "lib")
# Export all symbols.
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
ENDIF()

SET(AOCL_RELEASE_VERSION "4.1.0")

STRING(TIMESTAMP AOCL_UTILS_RELEASE_VERSION_STRING "AOCL-Utils ${AOCL_RELEASE_VERSION} Build %Y%m%d")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# CONTROL WHERE THE STATIC AND SHARED LIBRARIES ARE BUILT SO THAT ON WINDOWS
# WE DON'T NEED TO TINKER WITH THE PATH TO RUN THE EXECUTABLE
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

SET(CMAKE_INSTALL_LIBDIR "lib")

OPTION(ALCI_DOCS "ENABLE DOCUMENTATION" ON)
OPTION(ALCI_DOXYGEN_DOC "ENABLE DOXYGEN DOCUMENTATION" OFF)

SET(CMAKE_DEBUG_POSTFIX _DEBUG)

ADD_SUBDIRECTORY(lib)

IF (ALCI_DOCS)
	if (ALCI_DOXYGEN_DOC)
    find_package(Doxygen 1.9.6)
    set(W_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen")
    if(NOT (DOXYGEN_FOUND))
        message(STATUS "Doxygen not found please install doxygen and try again.")
    else()
        execute_process(COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                WORKING_DIRECTORY ${W_DIR}
                COMMAND_ECHO STDOUT)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen/html/index.html)
        message(STATUS "Documentation generated successfully, to view documentation open docs/doxygen/html/index.html")
    else()
        message(STATUS "Document generation failed.")
    endif()
  endif()
ENDIF()

IF (ALCI_BENCH)
  IF (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/BENCHMARK)
    MESSAGE(FATAL_ERROR "BENCHMARKS NOT SUPPORTED")
  ENDIF()

  ENABLE_TESTING()
  ADD_SUBDIRECTORY(TEST/BENCHMARK)
ENDIF()

# Add library install path

INSTALL(TARGETS aoclutils aoclutils_static)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/include/alci
        TYPE INCLUDE
        )

# Add library version in file.
FILE(WRITE ${CMAKE_BINARY_DIR}/version.h ${AOCL_UTILS_RELEASE_VERSION_STRING})
