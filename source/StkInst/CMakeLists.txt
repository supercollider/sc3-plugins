cmake_minimum_required (VERSION 2.8)
set(PROJECT "StkInst")
project (${PROJECT})

if(NOT SYSTEM_STK)
    set(STKDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external_libraries/stk)

    if(NOT EXISTS ${STKDIR}/include/Stk.h)
       message(FATAL_ERROR "Should use git submodule update and git submodule init to get Stk")
    endif()

    file(GLOB STKSources "${STKDIR}/src/*.cpp")
    file(GLOB UnneededSTKSources "${STKDIR}/src/Rt*.cpp"
                             "${STKDIR}/src/Inet*.cpp"
                             "${STKDIR}/src/Socket.cpp"
                             "${STKDIR}/src/Tcp*.cpp"
                             "${STKDIR}/src/UdpSocket.cpp"
                             "${STKDIR}/src/Thread.cpp"
                             "${STKDIR}/src/Mutex.cpp"
                             "${STKDIR}/src/Messager.cpp"
    )
    list(REMOVE_ITEM STKSources ${UnneededSTKSources})
    include_directories(${STKDIR})
    include_directories(${STKDIR}/include)
    include_directories(${STKDIR}/src/include)

    include (TestBigEndian)
    TEST_BIG_ENDIAN(is_big_endian)
    if(NOT is_big_endian)
        add_definitions("-D__LITTLE_ENDIAN__")
    endif()

    add_definitions("-D_STK_DEBUG_")
endif()

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)


set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
    set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()
    
add_library(${PROJECT} MODULE ${PROJECT}.cpp ${STKSources})
if(SYSTEM_STK)
    target_link_libraries(${PROJECT} ${STK_LIBRARY})
endif()

if(SUPERNOVA)
    add_library(${PROJECT}_supernova MODULE ${PROJECT}.cpp ${STKSources})
    if(SYSTEM_STK)
        target_link_libraries(${PROJECT}_supernova ${STK_LIBRARY})
    endif()
    set_property(TARGET ${PROJECT}_supernova APPEND PROPERTY COMPILE_DEFINITIONS  "SUPERNOVA")
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
    install(TARGETS ${PROJECT} DESTINATION ${INSTALL_DESTINATION})
    if(SUPERNOVA)
        install(TARGETS ${PROJECT}_supernova DESTINATION ${INSTALL_DESTINATION})
    endif()
else()
    install(TARGETS ${PROJECT} DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
    if(SUPERNOVA)
        install(TARGETS ${PROJECT}_supernova DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
    endif()
endif()

install(DIRECTORY "sc/" DESTINATION "${INSTALL_DESTINATION_DISTRO}/${PROJECT}" PATTERN "*")
if(NOT SYSTEM_STK)
    install(DIRECTORY "${STKDIR}/rawwaves/" DESTINATION "${INSTALL_DESTINATION_DISTRO}/${PROJECT}/rawwaves" PATTERN "*.raw")
endif()
