find_package(FFTW3f)

if (!FFTW3F_FOUND)
    message(SEND_ERROR "Cannot find libfftw3f")
endif()

if (SUPERNOVA)
    include_directories(${SC_PATH}/external_libraries/nova-tt)
    include_directories(${SC_PATH}/external_libraries/boost_lockfree)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/scfft_old/)

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

set(plugin_sources
    AmbisonicUGens.cpp
    BatUGens.cpp
    BBCut2UGens.cpp
    BerlachUGens
    BhobChaos.cpp
    BhobFilt.cpp
    BhobFFT.cpp
    BhobGrain.cpp
    BhobNoise.cpp
    Bit.cpp
    BlackrainUGens.cpp
    Concat.cpp
    GlitchUGens.cpp
    JoshUGens.cpp
    JoshAmbiUGens.cpp
    JoshGrainUGens.cpp
    JoshPVUGens.cpp
    LadspaUGens.cpp
    LoopBuf.cpp
    MCLDBufferUGens.cpp
    MCLDChaosUGens.cpp
    MCLDDistortionUGens.cpp
    MCLDFilterUGens.cpp
    MCLDFFTUGens.cpp
    MCLDGetenvUGen.cpp
    MCLDOscUGens.cpp
    MCLDSOMUGens.cpp
    MCLDTreeUGens.cpp
    MCLDTriggeredStatsUgens.cpp
    RFWUGens.cpp
    RMEQSuite.cpp
    SLUGens.cpp
    TagSystemUgens.cpp
    VBAP.cpp
    VOSIM.cpp
    SummerUGens.cpp
)

set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

foreach(plugin ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    BUILD_PLUGIN(${plugin_name} ${plugin} "")
endforeach(plugin)

BUILD_PLUGIN(MdaUGens MdaUGens/MdaUGens.cpp "")

BUILD_PLUGIN(MembraneUGens "Membrane_shape.c;Membrane.cpp" "")

BUILD_PLUGIN(LadspaUGen "LadspaUGen/LadspaUGen.cpp;LadspaUGen/search.c" "")
add_executable(ladspalist LadspaUGen/ladspalist.c LadspaUGen/search.c)
target_link_libraries(ladspalist dl)

file(GLOB PitchDetectionSources PitchDetection/*cpp)
if(APPLE)
    set (PitchDetectionLibs "-framework vecLib")
else()
    set (PitchDetectionLibs ${FFTW3F_LIBRARY})
endif()

BUILD_PLUGIN(PitchDetection "${PitchDetectionSources};${FFTSOURCES}" ${PitchDetectionLibs})

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

set(FFTSOURCES
    ${CMAKE_SOURCE_DIR}/scfft_old/fftlib.c
    ${CMAKE_SOURCE_DIR}/scfft_old/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/scfft_old/SCComplex.cpp
)

file(GLOB NCAnalysisSources NCAnalysis/*cpp)
if(APPLE)
    set (NCAnalysisLibs "-framework vecLib")
else()
    set (NCAnalysisLibs ${FFTW3F_LIBRARY})
endif()

BUILD_PLUGIN(NCAnalysis "${NCAnalysisSources};${FFTSOURCES}" ${NCAnalysisLibs})

if (AY)
    include_directories(AY_libayemu/include)
    BUILD_PLUGIN(AY_UGen "AY_UGen.cpp;AY_libayemu/src/ay8912.c" "")
endif()

file(GLOB TJUGensSources TJUGens/*.cpp)
BUILD_PLUGIN(TJUGens "${TJUGensSources}" "")

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS ${supernova_plugins}
            DESTINATION "lib/supernova/plugins"
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(TARGETS ${plugins}
                DESTINATION "lib/SuperCollider/plugins"
                PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    if (LADSPA_INCLUDE_DIR)
        install(TARGETS ladspalist
            DESTINATION "share/SuperCollider/Extensions/SC3plugins/LadspaUGen/"
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif()

if(APPLE)
    install(TARGETS ${plugins}
       DESTINATION  ${CMAKE_SOURCE_DIR}/build/plugins/
       PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    if (LADSPA_INCLUDE_DIR)
        install(TARGETS ladspalist
            DESTINATION ${CMAKE_SOURCE_DIR}/build/LadspaUGen/
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif()

