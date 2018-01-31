# HOA SC3-plugins

This Folder contains the HOA unit generator plugins based on the Faust library [Ambitools](https://github.com/sekisushai/ambitools) by Pierre Lecomte.  
Use these plugins together with [SC-HOA](https://github.com/florian-grond/SC-HOA) quark by Florian Grond.

The original [CPP sources are here]:  
(https://github.com/sekisushai/ambitools/tree/master/Faust/bin/ambitools2supercollider/HOA)
and can be generated from the
[Faust code](https://github.com/sekisushai/ambitools/tree/master/Faust/src)
by with
[this script](https://github.com/sekisushai/ambitools/blob/master/Faust/src/ambitools2supercollider.sh)

Since Faust generated CPP sources can have deep bracket nesting.
Without setting -fbracket-depth to an appropriate level, clang will exit with error.
This is why in CMakeLists there is:

if(CMAKE_COMPILER_IS_CLANG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbracket-depth=2048")
endif()
