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
    set(INSTALL_DESTINATION "share/SuperCollider/Extensions/SC3plugins")
else()
    set(INSTALL_DESTINATION ".")
endif()

install(DIRECTORY ${directories}
        DESTINATION ${INSTALL_DESTINATION}
        FILES_MATCHING
        PATTERN "*"
        PATTERN "*~" EXCLUDE)
