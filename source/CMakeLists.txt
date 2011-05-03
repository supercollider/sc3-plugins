find_package(FFTW3f)

if (!FFTW3F_FOUND)
    message(SEND_ERROR "Cannot find libfftw3f")
endif()


#---------
# includes
#---------

if (SUPERNOVA)
    include_directories(${SC_PATH}/external_libraries/nova-tt)
    include_directories(${SC_PATH}/external_libraries/boost_lockfree)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(scfft_old/)

include_directories(${SC_PATH}/common)
include_directories(${SC_PATH}/plugin_interface)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/server/plugins) # for FFT_UGens.h

# old-style directory layout
include_directories(${SC_PATH}/common/Headers/plugin_interface)
include_directories(${SC_PATH}/common/Headers/common)
include_directories(${SC_PATH}/common/Source/plugins) # for FFT_UGens.h

include_directories(${SC_PATH}/external_libraries/libsndfile/)


#-------
# macros
#-------

macro(GET_GCC_VERSION VAR)
    if (CMAKE_COMPILER_IS_GNUCC)
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE OUTPUT)
        string(REGEX REPLACE
            ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1" ${VAR} ${OUTPUT})
    endif()
endmacro(GET_GCC_VERSION)


macro(FIND_AND_BUILD_PLUGINS DIR)
    file(GLOB PLUGIN_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${DIR}/*.cpp")
    foreach(plugin ${PLUGIN_SRC})
        get_filename_component(basename ${plugin} NAME_WE)
        BUILD_PLUGIN(${basename} ${plugin} "")
    endforeach(plugin)
endmacro()


macro(BUILD_PLUGIN PLUGIN_NAME PLUGIN_SOURCES PLUGIN_LIBS)
    add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES})
    list(APPEND plugins ${PLUGIN_NAME})
    target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

    if(SUPERNOVA)
        add_library(${PLUGIN_NAME}_supernova MODULE ${PLUGIN_SOURCES})
        target_link_libraries(${PLUGIN_NAME}_supernova ${PLUGIN_LIBS})

        list(APPEND supernova_plugins ${PLUGIN_NAME}_supernova)
        set_property(TARGET ${PLUGIN_NAME}_supernova
                     PROPERTY COMPILE_DEFINITION SUPERNOVA)
    endif()
endmacro()


# INSTALL_DIST_FILES
# GLOB patterns are extra arguments
macro(INSTALL_DIST_FILES SEARCH_DIR DESTINATION)
    # TODO: new help system files?
    file(GLOB_RECURSE DIST_FILES RELATIVE "${SEARCH_DIR}" ${ARGN})
    install(FILES ${DIST_FILES}
            DESTINATION "${DESTINATION}"
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro()


#-----------------------
# plugins without extras
#-----------------------

set(PLUGIN_DIRS
    BatUGens
    BBCut2UGens
    BerlachUGens
    BhobUGens
    BitUGens
    BlackrainUGens
    ConcatUGens
    DistortionUGens
    GlitchUGens
    JoshUGens
    LoopBufUGens
    MCLDUGens
    MdaUGens
    RFWUGens
    RMEQSuiteUGens
    SLUGens
    SummerUGens
    TagSystemUGens
    VBAPUGens
    VOSIMUGens
)

# plugins with extra lib
set(PLUGIN_DIRS_EXTRA
    AYUGens
    LadspaUGen
    MembraneUGen
    NCAnalysisUGens
    PitchDetectionUGens
    StkUGens
    TJUGens
)


set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
if (APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

GET_GCC_VERSION(gcc_version)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(INSTALL_DESTINATION "lib/SuperCollider/plugins")
    set(INSTALL_DESTINATION_SUPERNOVA "lib/supernova/plugins")
    if (QUARKS)
        set(INSTALL_DESTINATION_LADSPALIST
                "share/SuperCollider/SC3plugins/LadspaUGen/")
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/SC3plugins")
    else()
        set(INSTALL_DESTINATION_LADSPALIST
                "share/SuperCollider/Extensions/SC3plugins/LadspaUGen/")
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/Extensions/SC3plugins")
    endif(QUARKS)
else()
    set(INSTALL_DESTINATION "plugins")
    set(INSTALL_DESTINATION_SUPERNOVA "plugins_supernova")
    set(INSTALL_DESTINATION_LADSPALIST "SC3plugins/LadspaUGen")
    set(INSTALL_DESTINATION_DISTRO "SC3plugins")
endif()


foreach(DIR ${PLUGIN_DIRS})
    FIND_AND_BUILD_PLUGINS(${DIR})
endforeach(DIR)

# install help and sc files
list(APPEND ${PLUGIN_DIRS} ${PLUGIN_DIRS_EXTRA})
foreach(DIR ${PLUGIN_DIRS})
    INSTALL_DIST_FILES("${CMAKE_CURRENT_SOURCE}" "${INSTALL_DESTINATION_DISTRO}/${DIR}"
            "${DIR}/*.html" "${DIR}/*.rtf" "${DIR}/*.sc" "${DIR}/*.scd")
endforeach(DIR)


#---------------------
# plugins with extras:
#---------------------

# MembraneUGens
BUILD_PLUGIN(MembraneUGens "MembraneUGens/Membrane_shape.c;MembraneUGens/Membrane.cpp" "")

# LadspaUGen
BUILD_PLUGIN(LadspaUGen "LadspaUGen/LadspaUGen.cpp;LadspaUGen/search.c" "")
add_executable(ladspalist LadspaUGen/ladspalist.c LadspaUGen/search.c)
target_link_libraries(ladspalist dl)

# PitchDetectionUGens
file(GLOB PitchDetectionSources PitchDetection/*cpp)
if(APPLE)
    set (PitchDetectionLibs "-framework vecLib")
else()
    set (PitchDetectionLibs ${FFTW3F_LIBRARY})
endif()

BUILD_PLUGIN(PitchDetection "${PitchDetectionSources};${FFTSOURCES}" ${PitchDetectionLibs})

# StkUGens
file(GLOB STKSources "StkUGens/stk-4.4.2/src/*.cpp")
file(GLOB UnneededSTKSources "StkUGens/stk-4.4.2/src/Rt*.cpp"
                             "StkUGens/stk-4.4.2/src/Inet*.cpp"
                             "StkUGens/stk-4.4.2/src/Socket.cpp"
                             "StkUGens/stk-4.4.2/src/Tcp*.cpp"
                             "StkUGens/stk-4.4.2/src/UdpSocket.cpp"
                             "StkUGens/stk-4.4.2/src/Thread.cpp"
                             "StkUGens/stk-4.4.2/src/Mutex.cpp"
)
list(REMOVE_ITEM STKSources ${UnneededSTKSources})
include_directories(StkUGens/stk-4.4.2)
include_directories(StkUGens/stk-4.4.2/include)
include_directories(StkUGens/stk-4.4.2/src/include)
BUILD_PLUGIN(StkUGens "StkUGens/StkAll.cpp;${STKSources}" "")

# NCAnalysisUGens
set(FFTSOURCES
    scfft_old/fftlib.c
    scfft_old/SC_fftlib.cpp
    scfft_old/SCComplex.cpp
)

file(GLOB NCAnalysisSources NCAnalysis/*cpp)
if(APPLE)
    set (NCAnalysisLibs "-framework vecLib")
else()
    set (NCAnalysisLibs ${FFTW3F_LIBRARY})
endif()

BUILD_PLUGIN(NCAnalysis "${NCAnalysisSources};${FFTSOURCES}" ${NCAnalysisLibs})

# AYUGens
if (AY)
    include_directories(AY_libayemu/include)
    BUILD_PLUGIN(AY_UGen "AYUGens/AY_UGen.cpp;AY_libayemu/src/ay8912.c" "")
endif()

# TJUGens
if (APPLE AND gcc_version MATCHES "4\\.0.*")
    message(WARNING "TJUGens won't be built with gcc-4.0 on Apple")
else()
    file(GLOB TJUGensSources TJUGens/*.cpp)
    BUILD_PLUGIN(TJUGens "${TJUGensSources}" "")
endif()


#--------
# INSTALL
#--------

install(TARGETS ${plugins}
        DESTINATION ${INSTALL_DESTINATION}
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(TARGETS ${supernova_plugins}
        DESTINATION ${INSTALL_DESTINATION_SUPERNOVA}
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(TARGETS ladspalist
        DESTINATION ${INSTALL_DESTINATION_LADSPALIST}
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
