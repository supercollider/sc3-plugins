cmake_minimum_required (VERSION 2.8)
set(PROYECTO "StkInst")
project (${PROYECTO})

include (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
set(STKDIR ${CMAKE_CURRENT_SOURCE_DIR}/stk)

file(GLOB STKSources "${STKDIR}/src/*.cpp")
file(GLOB UnneededSTKSources "${STKDIR}/src/Rt*.cpp"
                             "${STKDIR}/src/Inet*.cpp"
                             "${STKDIR}/src/Socket.cpp"
                             "${STKDIR}/src/Tcp*.cpp"
                             "${STKDIR}/src/UdpSocket.cpp"
                             "${STKDIR}/src/Thread.cpp"
                             "${STKDIR}/src/Mutex.cpp"
)
#message(WARNING ${STKSources})
list(REMOVE_ITEM STKSources ${UnneededSTKSources})
include_directories(${STKDIR})
include_directories(${STKDIR}/include)
include_directories(${STKDIR}/src/include)


include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)


set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()
	
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2 -mstackrealign")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mstackrealign")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
add_definitions("-D_STK_DEBUG_") 

TEST_BIG_ENDIAN(is_big_endian)
if(NOT is_big_endian)
	add_definitions("-D__LITTLE_ENDIAN__")
endif()

add_library(${PROYECTO} MODULE ${PROYECTO}.cpp ${STKSources})
if(SUPERNOVA)
add_library(${PROYECTO}_supernova MODULE ${PROYECTO}.cpp ${STKSources})
set_property(TARGET ${PROYECTO}_supernova APPEND PROPERTY COMPILE_DEFINITIONS  "SUPERNOVA")
include_directories(${SC_PATH}/external_libraries
					${SC_PATH}/external_libraries/boost-lockfree
					${SC_PATH}/external_libraries/nova-simd
					${SC_PATH}/external_libraries/nova-tt
					)
include_directories(${SC_PATH}/external_libraries/boost)
include_directories(${SC_PATH}/external_libraries/nova-simd)
add_definitions("-DNOVA_SIMD")
endif()

#install
if (NOT APPLE)
    install(TARGETS ${PROYECTO} DESTINATION ${INSTALL_DESTINATION})
    if(SUPERNOVA)
        install(TARGETS ${PROYECTO}_supernova DESTINATION ${INSTALL_DESTINATION})
    endif()
else()
    install(TARGETS ${PROYECTO} DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
    if(SUPERNOVA)
        install(TARGETS ${PROYECTO}_supernova DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
    endif()
endif()

install(DIRECTORY "${PROYECTO}/sc/" DESTINATION "${INSTALL_DESTINATION_DISTRO}/${PROYECTO}" PATTERN "*")
