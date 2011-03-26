set(directories
    AYUGens
    BatUGens
    BBCut2UGens
    BerlachUGens
    BhobUGens
    BlackrainUGens
    Concat
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
    VBAPUGens
    VOSIMUGens
    TJUGens
    SummerUGens
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(DIRECTORY ${directories}
            DESTINATION "share/SuperCollider/Extensions/SC3plugins"
            FILES_MATCHING
            PATTERN "*"
            PATTERN "*~" EXCLUDE)
endif()
