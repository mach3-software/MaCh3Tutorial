add_custom_target(NuDockApps)

foreach(app
    RunNuDockServer
    )

    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::SamplesTutorial)
    add_dependencies(NuDockApps ${app})
endforeach(app)
