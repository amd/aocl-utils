include(FetchContent)
set(INSTALL_GTEST OFF)
include(GoogleTest)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1)

#
# On Windows: Prevent overriding the parent project's compiler/linker settings
#
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

option(GTEST_ENABLE_INSTALL "" OFF)
option(GMOCK_ENABLE_INSTALL "" OFF)
FetchContent_MakeAvailable(gtest)

