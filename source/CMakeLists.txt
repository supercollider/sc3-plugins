#---------
# includes
#---------

if (SUPERNOVA)
    include_directories(${SC_PATH}/external_libraries/nova-tt)
    # actually just boost.atomic
    include_directories(${SC_PATH}/external_libraries/boost)
    include_directories(${SC_PATH}/external_libraries/boost_lockfree)
    include_directories(${SC_PATH}/external_libraries/boost-lockfree)
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
        BUILD_PLUGIN(${basename} ${plugin} "" "${DIR}")
    endforeach(plugin)
endmacro()


# Hacky: OSX plugins should install in PLUGIN_DIR
macro(BUILD_PLUGIN PLUGIN_NAME PLUGIN_SOURCES PLUGIN_LIBS PLUGIN_DIR)
    add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES})
    list(APPEND plugins ${PLUGIN_NAME})
    target_link_libraries(${PLUGIN_NAME} ${PLUGIN_LIBS})

    if(APPLE)
        install(TARGETS ${PLUGIN_NAME}
                DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
    endif()

    if(SUPERNOVA)
        add_library(${PLUGIN_NAME}_supernova MODULE ${PLUGIN_SOURCES})
        target_link_libraries(${PLUGIN_NAME}_supernova ${PLUGIN_LIBS})

        list(APPEND supernova_plugins ${PLUGIN_NAME}_supernova)
        set_property(TARGET ${PLUGIN_NAME}_supernova
                     PROPERTY COMPILE_DEFINITIONS SUPERNOVA)
        if(APPLE)
            install(TARGETS ${PLUGIN_NAME}_supernova
                    DESTINATION ${INSTALL_DESTINATION}/${PLUGIN_DIR})
        endif()
    endif()

endmacro()

#-----------------------
# plugins without extras
#-----------------------

set(PLUGIN_DIRS
)

# plugins with extra lib
set(PLUGIN_DIRS_EXTRA
)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" AND ${CMAKE_CXX_COMPILER_VERSION} MATCHES "^6.0.")
  set(BHOBFILT_CPP "${CMAKE_CURRENT_SOURCE_DIR}/BhobUGens/BhobFilt.cpp")
  message(STATUS "Skipping vectorization on BhobFilt.cpp because of Clang bug. (${BHOBFILT_CPP})")
  SET_SOURCE_FILES_PROPERTIES(${BHOBFILT_CPP} PROPERTIES COMPILE_FLAGS "-fno-slp-vectorize")
endif()

set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(PLUGIN_EXTENSION ".so")

if (APPLE OR WIN32)
    set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
    set(PLUGIN_EXTENSION ".scx")
endif()


GET_GCC_VERSION(gcc_version)


if (CMAKE_SYSTEM_NAME MATCHES "Linux|.*BSD|DragonFly")
    set(INSTALL_DESTINATION "lib/SuperCollider/plugins")
    if (QUARKS)
        set(INSTALL_DESTINATION_LADSPALIST
                "share/SuperCollider/SC3plugins/LadspaUGen")
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/SC3plugins")
    else()
        set(INSTALL_DESTINATION_LADSPALIST
                "share/SuperCollider/Extensions/SC3plugins/LadspaUGen")
        set(INSTALL_DESTINATION_DISTRO
                "share/SuperCollider/Extensions/SC3plugins")
    endif(QUARKS)
else()
    set(INSTALL_DESTINATION "SC3plugins")
    set(INSTALL_DESTINATION_LADSPALIST "SC3plugins/LadspaUGen")
    set(INSTALL_DESTINATION_DISTRO "SC3plugins")
endif()

foreach(DIR ${PLUGIN_DIRS})
    FIND_AND_BUILD_PLUGINS(${DIR})
endforeach(DIR)

# build source files placed in 'local' by the user (i.e. by default this does nothing)
file(GLOB local_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "local/*.cpp")
message(STATUS "local_sources: ${local_sources}")
foreach(plugin ${local_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    string(REPLACE "local/" "" plugin_name ${plugin_name} )
    BUILD_PLUGIN(${plugin_name} ${plugin} "" "local")
endforeach(plugin)
# local can have help files and classes too
install(DIRECTORY local DESTINATION "${INSTALL_DESTINATION_DISTRO}"
    FILES_MATCHING PATTERN "*.sc" PATTERN "*.scd" PATTERN "*.html" PATTERN "*.schelp" PATTERN "*.rtf")

#---------------------
# plugins with extras:
#---------------------

#BetablockerUGens
BUILD_PLUGIN(BetablockerUGens "BetablockerUGens/BetablockerUGens.cpp;BetablockerUGens/betablocker/machine.cpp;BetablockerUGens/betablocker/thread.cpp"
             "" "BetablockerUGens")

#--------
# INSTALL
#--------

# OSX plugins install is done in BUILD_PLUGIN macro
if (NOT APPLE)
    install(TARGETS ${plugins} ${supernova_plugins}
            DESTINATION ${INSTALL_DESTINATION})
endif()

# install help and sc specific files that are found in 'PLUGIN_NAME/sc' folder
foreach (DIR ${PLUGIN_DIRS};${PLUGIN_DIRS_EXTRA})
    install(DIRECTORY "${DIR}/sc/" DESTINATION "${INSTALL_DESTINATION_DISTRO}/${DIR}"
            PATTERN "*")
endforeach(DIR)
