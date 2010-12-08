find_package(FFTW3f)

if (!FFTW3F_FOUND)
    message(SEND_ERROR "Cannot find libfftw3f")
endif()

if (SUPERNOVA)
    add_definitions(-DSUPERNOVA)
    include_directories(${SN_PATH}/libs/nova-tt)
    include_directories(${SN_PATH}/libs/boost_lockfree)
endif()


include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/scfft_old/)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/server/plugins) # for FFT_UGens.h

# old-style directory layout
include_directories(${SC_PATH}/common/Headers/plugin_interface)
include_directories(${SC_PATH}/common/Headers/common)
include_directories(${SC_PATH}/common/Source/plugins) # for FFT_UGens.h

include_directories(${SC_PATH}/external_libraries/libsndfile/)

set(plugin_sources
    AmbisonicUGens.cpp
    BatUGens.cpp
    BerlachUGens
    BhobChaos.cpp
    BhobFilt.cpp
    BhobFFT.cpp
    BhobGrain.cpp
    BhobNoise.cpp
    BlackrainUGens.cpp
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
)

set(plugins "")
set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

foreach(plugin ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    add_library(${plugin_name} MODULE ${plugin})
    list(APPEND plugins ${plugin_name})
endforeach(plugin)


add_library(MdaUGens MODULE MdaUGens/MdaUGens.cpp)
list(APPEND plugins MdaUGens)

add_library(MembraneUGens MODULE Membrane_shape.c Membrane.cpp)
list(APPEND plugins MembraneUGens)

find_path(LADSPA_INCLUDE_DIR NAMES ladspa.h PATHS /usr/local/include /usr/include)
if (LADSPA_INCLUDE_DIR)
    add_library(LadspaUGen MODULE LadspaUGen/LadspaUGen.cpp LadspaUGen/search.c)
    list(APPEND plugins LadspaUGen)
endif()

file(GLOB MLfftwSources MLfftwUGens/*cpp)
add_library(MLfftwUGens MODULE ${MLfftwSources})
target_link_libraries(MLfftwUGens ${FFTW3F_LIBRARY})
list(APPEND plugins MLfftwUGens)

add_subdirectory(StkUGens)
list(APPEND plugins ${StkUGens})

set(FFTSOURCES
    ${CMAKE_SOURCE_DIR}/scfft_old/fftlib.c
    ${CMAKE_SOURCE_DIR}/scfft_old/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/scfft_old/SCComplex.cpp
)

file(GLOB NCAnalysisSources NCAnalysis/*cpp)
add_library(NCAnalysis MODULE ${NCAnalysisSources} ${FFTSOURCES})
if (NOT APPLE)
    target_link_libraries(NCAnalysis ${FFTW3F_LIBRARY})
endif()

if (APPLE)
    target_link_libraries(NCAnalysis "-framework vecLib")
endif()

list(APPEND plugins NCAnalysis)

if (AY)
    include_directories(AY_libayemu/include)
    add_library(AY_UGen MODULE AY_UGen.cpp AY_libayemu/src/ay8912.c)
    list(APPEND plugins AY_UGen)
endif()


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    if (SUPERNOVA)
        install(TARGETS ${plugins}
                DESTINATION "lib/supernova/plugins"
                PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    else()
        install(TARGETS ${plugins}
                DESTINATION "lib/SuperCollider/plugins"
                PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif()

if(APPLE)
    install(TARGETS ${plugins}
       DESTINATION  ${CMAKE_SOURCE_DIR}/build/plugins/
       PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

