add_custom_target(ValidApps)

foreach(app
    SplineValidations
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3::All ValidationsUtils)

    add_dependencies(ValidApps ${app})

endforeach(app)
