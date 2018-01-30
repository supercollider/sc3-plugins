cmake_minimum_required (VERSION 2.8)
set(SUPERNOVA_CMAKE_MINVERSION 3.1)
project (sc3-plugins)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external_libraries/nova-simd/simd_memory.hpp)
  message(FATAL_ERROR "The nova-simd submodule is missing. This probably means you forgot to clone submodules. To fix this, run `git submodule update --init` from the root folder of sc3-plugins repository")
endif()

if (NOT SYSTEM_STK)
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external_libraries/stk/include/Stk.h)
    message(FATAL_ERROR "The stk submodule is missing. This probably means you forgot to clone submodules. To fix this, run `git submodule update --init` from the root folder of the sc3-plugins repository")
  endif()
endif()

if (SUPERNOVA)
  if (NOT EXISTS ${SC_PATH}/external_libraries/nova-tt/CMakeLists.txt)
    message(FATAL_ERROR "The nova-tt submodule in the SuperCollider repository is missing (required for SuperNova plugins). This probably means you forgot to clone submodules. To fix this, run `git submodule update --init` from the root folder of the SuperCollider repository")
  endif()
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

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND NOT CMAKE_VERSION VERSION_LESS SUPERNOVA_CMAKE_MINVERSION)
	option(SUPERNOVA "Build plugins for supernova" ON)
else()
	option(SUPERNOVA "Build plugins for supernova" OFF)
endif()

option(AY "Build with AY ugens" ON)
option(QUARKS "Install plugins as quarks")
option(OSX_PACKAGE "Package dmg for Apple")
option(IN_PLACE_BUILD "Build and install in cmake build folder" ON)
option(NOVA_SIMD "Build VBAP with nova-simd support." ON)
option(CPP11 "Build with c++11." ON)
option(NATIVE "Optimize for this specific machine." OFF)
option(SYSTEM_STK "Use STK libraries from system" OFF)

option(NOVA_DISK_IO "Build with Nova's DiskIO UGens. Requires boost source tree, break warranty & eats your children." OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	set(CMAKE_COMPILER_IS_CLANG 1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    add_definitions(-fvisibility=hidden)

    include (CheckCCompilerFlag)
    include (CheckCXXCompilerFlag)

    CHECK_C_COMPILER_FLAG(-msse HAS_SSE)
    CHECK_CXX_COMPILER_FLAG(-msse HAS_CXX_SSE)

    if (HAS_SSE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse")
    endif()
    if (HAS_CXX_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
    endif()

    CHECK_C_COMPILER_FLAG(-msse2 HAS_SSE2)
    CHECK_CXX_COMPILER_FLAG(-msse2 HAS_CXX_SSE2)

    if (HAS_SSE2)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
    endif()
    if (HAS_CXX_SSE2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif()

    CHECK_C_COMPILER_FLAG(-mfpmath=sse HAS_FPMATH_SSE)
    CHECK_CXX_COMPILER_FLAG(-mfpmath=sse HAS_CXX_FPMATH_SSE)

    if (HAS_FPMATH_SSE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse")
    endif()
    if (HAS_CXX_FPMATH_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
    endif()

	if(NATIVE)
		add_definitions(-march=native)
	endif()

	if(CPP11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		if(CMAKE_COMPILER_IS_CLANG)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		endif()
	endif()
endif()
if(MINGW)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mstackrealign")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mstackrealign")
endif()

if (NOVA_SIMD)
  add_definitions(-DNOVA_SIMD)
  include_directories(external_libraries/nova-simd)
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

# Configure readme file
if (WIN32 OR APPLE)
    if (WIN32)
        set(README_FILENAME README_WINDOWS.txt)
    elseif(APPLE)
        set(README_FILENAME README_MACOS.txt)
    else()
        unset(README_FILENAME)
    endif()
endif()

if (DEFINED README_FILENAME)
    configure_file( "${README_FILENAME}.in" "${README_FILENAME}" )
    install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${README_FILENAME}" DESTINATION . RENAME README.txt )
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

