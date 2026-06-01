add_custom_target(TutorialPlugins)

foreach(app
    MCMCTutorialPlugin
    )

    add_library(${app} SHARED ${app}.cpp)

    target_link_libraries(${app} PRIVATE MaCh3Tutorial::SamplesTutorial MaCh3::All)
    target_include_directories(${app} PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>)


#    add_executable(${app} ${app}.cpp)
#    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)

#    add_dependencies(TutorialApps ${app})
#    install(TARGETS ${app} DESTINATION bin)
endforeach(app)

