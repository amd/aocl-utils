include(GNUInstallDirs)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/SDK/Include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h?")

install(TARGETS au_base
  EXPORT sdk
  ARCHIVE COMPONENT development
  LIBRARY COMPONENT runtime
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Af
    COMPONENT runtime
)

if (UNIX)
  install(CODE "execute_process(COMMAND ldconfig)"
          COMPONENT runtime
  )
endif()


install(EXPORT sdk
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Af/cmake
  NAMESPACE Af::
  COMPONENT runtime
)

install(FILES "af-config.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Af/cmake
)

#install(TARGETS console
#  RUNTIME COMPONENT runtime
#)

#
# CPACK configuration
#
set(CPACK_PACKAGE_NAME                  "${PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION           "AOCL Foundations")
set(CPACK_PACKAGE_VENDOR                "AMD")
set(CPACK_PACKAGE_DESCRIPTION_FILE      "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     "/usr/local")
set(CPACK_PACKAGE_VERSION_MAJOR         ${AU_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${AU_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${AU_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION               ${AU_VERSION_STRING})
 
include(CPack)
