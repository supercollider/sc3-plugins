find_package(FFTW3f)

if (!FFTW3F_FOUND)
    message(SEND_ERROR "Cannot find libfftw3f")
endif()

if (SUPERNOVA)
    add_definitions(-DSUPERNOVA)
    include_directories(${SN_PATH}/libs/nova-tt)
    include_directories(${SN_PATH}/libs/boost_lockfree)
endif()



include_directories(${CMAKE_SOURCE_DIR}/scfft_old/)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/server/plugins) # for FFT_UGens.h

# old-style directory layout
include_directories(${SC_PATH}/common/Headers/plugin_interface)
include_directories(${SC_PATH}/common/Headers/common)
include_directories(${SC_PATH}/common/Source/plugins) # for FFT_UGens.h

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
    VOSIM.cpp
)

set(plugins "")
set(CMAKE_SHARED_MODULE_PREFIX "")

foreach(plugin ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    add_library(${plugin_name} MODULE ${plugin})
    list(APPEND plugins ${plugin_name})
endforeach(plugin)


add_library(MdaUGens MODULE MdaUGens/MdaUGens.cpp)
add_library(MembraneUGens MODULE Membrane_shape.c Membrane.cpp)


file(GLOB MLfftwSources MLfftwUGens/*cpp)
add_library(MLfftwUGens MODULE ${MLfftwSources})
target_link_libraries(MLfftwUGens ${FFTW3F_LIBRARY})

if (STK)
    # untested
    add_library(StkUGens MODULE StkUGens/StkAll.cpp)
    target_link_libraries(StkUGens STK)
endif()

set(FFTSOURCES
    ${CMAKE_SOURCE_DIR}/scfft_old/fftlib.c
    ${CMAKE_SOURCE_DIR}/scfft_old/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/scfft_old/SCComplex.cpp
)

file(GLOB NCAnalysisSources NCAnalysis/*cpp)
add_library(NCAnalysis MODULE ${NCAnalysisSources})
target_link_libraries(NCAnalysis ${FFTW3F_LIBRARY})

if (AY)
    include_directories(AY_libayemu/include)
    add_library(AY_UGen MODULE AY_UGen.cpp AY_libayemu/src/ay8912.c)
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
