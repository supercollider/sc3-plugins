# HOA SC3-plugins

This Folder contains the HOA unit generator plugins based on the Faust library [Ambitools](https://github.com/sekisushai/ambitools) by Pierre Lecomte.  
Use these plugins together with SC-HOA quark by Florian Grond.

The original [CPP sources are here]:  
(https://github.com/sekisushai/ambitools/tree/master/Faust/bin/ambitools2supercollider/HOA)

In order to compile the Faust based CPP sources, we added to the CMakeLists.txt in the source folder
the following lines:

# This is to be able to switch off the debug messages coming from the Faust generated CPP sources
# by setting to release mode -DCMAKE_BUILD_TYPE=Release
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

# This is necessary since Faust generated CPP sources can have (very) deep bracket nesting
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbracket-depth=2048")
