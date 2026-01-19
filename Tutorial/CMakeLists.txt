add_custom_target(TutorialApps)

foreach(app
    MCMCTutorial
    LLHScanTutorial
    LLHMapTutorial
    PredictiveTutorial
    SigmaVarTutorial
    KinemDistributionTutorial
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)
    if(MaCh3Tutorial_PROFILING_ENABLED)
        target_link_libraries(${app}
            profiler  # Add this line to link gperftools
        )
    endif()

    add_dependencies(TutorialApps ${app})
    install(TARGETS ${app} DESTINATION bin)
endforeach(app)

install(FILES
    TutorialDiagConfig.yaml
    DESTINATION ${CMAKE_BINARY_DIR}/bin)
