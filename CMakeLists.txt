cmake_minimum_required (VERSION 2.8)
project (sc3-plugins)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libs/nova-simd/simd_memory.hpp)
        message(FATAL_ERROR "nova-simd submodule is missing: please run `git submodule init && git submodule update' from the toplevel of your git working tree")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

find_package(SuperCollider3)
if (NOT SC_FOUND)
	message(SEND_ERROR "cannot find SuperCollider3 headers. Set the variable SC_PATH.")
else()
	message(STATUS "Using SC source located at ${SC_PATH}")
endif()

include("${SC_PATH}/SCVersion.txt")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
message(STATUS "Building plugins for SuperCollider version: ${PROJECT_VERSION}")

option(SUPERNOVA "Build plugins for supernova")
option(AY "Build with AY ugens" ON)
option(QUARKS "Install plugins as quarks")
option(OSX_PACKAGE "Package dmg for Apple")
option(IN_PLACE_BUILD "Build and install in cmake build folder" ON)
option(NOVA_SIMD "Build VBAP with nova-simd support." ON)
option(NOVA_UGENS "Build nova ugens (requires c++11 compliant compiler)." OFF)
option(CPP11 "Build with c++11." OFF)
option(NATIVE "Optimize for this specific machine." OFF)
option(DSO_VISIBILITY "Hide private symbols." OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_COMPILER_IS_CLANG 1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
	if(DSO_VISIBILITY)
		add_definitions(-fvisibility=hidden)
	endif()

	if(NATIVE)
		add_definitions(-march=native)
	endif()

	if(CPP11)
		set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
		if(CMAKE_COMPILER_IS_CLANG)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		endif()
	endif()
endif()


if (NOVA_SIMD)
  add_definitions(-DNOVA_SIMD)
  include_directories(libs/nova-simd)
endif()

# osx `make install' defaults into cmake_build/SC3-plugins directory
if (APPLE AND IN_PLACE_BUILD)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
endif()

# when packaging OSX dmg, install in 'cmake_build/build_osx' so we can include
# directly the created SC3plugins subfolder in the root of the dmg (instead of
# all plugins directories in the dmg root)
if (APPLE AND OSX_PACKAGE)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/build_osx")
endif()

add_subdirectory(source)

if (QUARKS OR OSX_PACKAGE) # add quarks DIRECTORY in osx dmg
  add_subdirectory(quarks)
endif()

if (OSX_PACKAGE)
  add_subdirectory(osx_package)
endif()

if(WIN32)
    configure_file( README_WINDOWS.txt.in README_WINDOWS.txt)
    install( FILES "${CMAKE_CURRENT_BINARY_DIR}/README_WINDOWS.txt" DESTINATION . RENAME README.txt )
endif()

#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

if(WIN32)
    set(CPACK_GENERATOR ZIP)
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
    set(CPACK_MONOLITHIC_INSTALL 1)
endif()

include(CPack)
