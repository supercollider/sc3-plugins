find_library(SNDFILE sndfile)

if(CPP11 AND NOT(SNDFILE MATCHES -NOTFOUND) )

  include_directories(../../libs/TLSF-2.4.6/src/ sync/include)
  BUILD_PLUGIN(NovaDiskIO
              "NovaDiskIO.cpp;../../libs/TLSF-2.4.6/src/tlsf.c"
              "sndfile"
              "NovaDiskIO")
endif()
