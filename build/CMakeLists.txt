set(directories
    AYUGens
    BatUGens
    BBCut2UGens
    BerlachUGens
    BhobUGens
    BitUGens
    BlackrainUGens
    Concat
    JoshUGens
    GlitchUGens
    LadspaUGens
    LadspaUGen
    LoopBufUGens
    MCLDUGens
    MdaUGens
    MembraneUGens
    NCAnalysisUGens
    PitchDetection
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
