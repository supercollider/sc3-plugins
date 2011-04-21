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

option(SUPERNOVA "Build plugins for supernova")
option(AY "Build with AY ugens")

add_subdirectory(build)
add_subdirectory(source)


#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH dev)
include(CPack)
if(WIN32)
    set(CPACK_MONOLITHIC_INSTALL 1)
endif()
