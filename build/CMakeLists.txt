set(directories
    AYUGens
    BatUGens
    BerlachUGens
    BhobUGens
    BlackrainUGens
    JoshUGens
    LadspaUGens
    LadspaUGen
    LoopBufUGens
    MCLDUGens
    MdaUGens
    MembraneUGens
    MLfftwUGens
    NCAnalysisUGens
    RFWUGens
    RMEQSuiteUGens
    SLUGens
    StkUGens
    TagSystemUGens
    VOSIMUGens
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(DIRECTORY ${directories}
            DESTINATION "share/SuperCollider/Extensions/SC3plugins")
endif()
