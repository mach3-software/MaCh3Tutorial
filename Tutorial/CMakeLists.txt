add_custom_target(TutorialApps)

foreach(app
    LLHScanTutorial
    PredictiveTutorial
    SigmaVarTutorial
    KinemDistributionTutorial
    TutorialEventRates
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

foreach(plugin_app
    MCMCTutorial
    )
    add_executable(${plugin_app} ${plugin_app}.cpp)
    target_link_libraries(${plugin_app} MaCh3Tutorial::${plugin_app}Plugin MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)
    add_dependencies(${plugin_app} ${plugin_app}Plugin)  # must be real target, not alias
    if(MaCh3Tutorial_PROFILING_ENABLED)
        target_link_libraries(${plugin_app}
            profiler  # Add this line to link gperftools
        )
    endif()

    add_dependencies(TutorialApps ${plugin_app})
    install(TARGETS ${plugin_app} DESTINATION bin)
endforeach(plugin_app)

install(FILES
    TutorialDiagConfig.yaml
    DESTINATION ${CMAKE_BINARY_DIR}/bin)
