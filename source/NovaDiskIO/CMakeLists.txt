find_library(SNDFILE sndfile)

if(CPP11 AND NOT(SNDFILE MATCHES -NOTFOUND) )

  include_directories(../../external_libraries/TLSF-2.4.6/src/ sync/include)
  BUILD_PLUGIN(NovaDiskIO
              "NovaDiskIO.cpp;../../external_libraries/TLSF-2.4.6/src/tlsf.c"
              "sndfile"
              "NovaDiskIO")
endif()
