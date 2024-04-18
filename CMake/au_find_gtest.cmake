include(FetchContent)
set(INSTALL_GTEST OFF)
include(GoogleTest)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
  SYSTEM)
FetchContent_MakeAvailable(gtest)

#
# On Windows: Prevent overriding the parent project's compiler/linker settings
#
IF(WIN32)
	target_link_libraries(gmock PUBLIC gtest)
	target_link_libraries(gmock_main PUBLIC gtest_main)
ENDIF()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

option(GTEST_ENABLE_INSTALL "" OFF)
option(GMOCK_ENABLE_INSTALL "" OFF)
FetchContent_MakeAvailable(gtest)
# You can disable clang-tidy for specific folders in the fetched content project
if(CMAKE_CXX_CLANG_TIDY)
    set_target_properties(gtest PROPERTIES CXX_CLANG_TIDY "")
    set_target_properties(gtest_main PROPERTIES CXX_CLANG_TIDY "")
    set_target_properties(gmock PROPERTIES CXX_CLANG_TIDY "")
    set_target_properties(gmock_main PROPERTIES CXX_CLANG_TIDY "")
endif()
