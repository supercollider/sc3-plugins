cmake_minimum_required (VERSION 2.8)
project (sc3-plugins)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


set(SC_PATH "/usr/include/SuperCollider" CACHE STRING "SuperCollider path")


# osx `make install' defaults into sc3-plugins directory
# FIXME: install in "/Library/Application Support/SuperCollider" ??
if (${CMAKE_INSTALL_PREFIX} MATCHES "/usr/local" AND APPLE)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build_osx")
endif()


option(SUPERNOVA "Build plugins for supernova")
option(AY "Build with AY ugens")
option(QUARKS "Install plugins as quarks")

add_subdirectory(source)
if (QUARKS)
  add_subdirectory(quarks)
endif()

#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH dev)

include(CPack)
if(WIN32)
    set(CPACK_MONOLITHIC_INSTALL 1)
endif()
