add_custom_target(TutorialApps)

foreach(app
    MCMCTutorial
    LLHScanTutorial
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Validations::ValidationsUtils)

    add_dependencies(TutorialApps ${app})
    install(TARGETS ${app} DESTINATION bin)
endforeach(app)

install(FILES
    TutorialDiagConfig.yaml
    DESTINATION ${CMAKE_BINARY_DIR}/bin)
