add_custom_target(TutorialPlugins)

foreach(app
    MCMCTutorialPlugin
    )

    add_library(${app} SHARED ${app}.cpp)

    target_link_libraries(${app} PRIVATE MaCh3Tutorial::SamplesTutorial MaCh3::All)
    # target_include_directories(${app} PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>)

    target_include_directories(${app}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    install(
        TARGETS ${app}
        EXPORT MaCh3TutorialTargets
        LIBRARY DESTINATION lib
    )

    install(
        FILES ${app}.hpp
        DESTINATION include/Mach3Plugins
    )

    add_library(MaCh3Tutorial::${app} ALIAS ${app})

#    add_executable(${app} ${app}.cpp)
#    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)

#    add_dependencies(TutorialApps ${app})
#    install(TARGETS ${app} DESTINATION bin)
endforeach(app)

