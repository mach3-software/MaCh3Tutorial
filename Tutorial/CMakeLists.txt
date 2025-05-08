add_custom_target(TutorialApps)

foreach(app
    MCMCTutorial
    LLHScanTutorial
    KinemDistributionTutorial
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SampleHandlerTutorial)

    add_dependencies(TutorialApps ${app})
    install(TARGETS ${app} DESTINATION bin)
endforeach(app)

install(FILES
    TutorialDiagConfig.yaml
    DESTINATION ${CMAKE_BINARY_DIR}/bin)
