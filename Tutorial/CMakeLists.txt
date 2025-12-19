add_custom_target(TutorialApps)

foreach(app
    MCMCTutorial
    LLHScanTutorial
    PredictiveTutorial
    SigmaVarTutorial
    KinemDistributionTutorial
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)
    set_target_properties(${app} PROPERTIES
        BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
        INSTALL_RPATH "@loader_path/../lib"
    )
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
