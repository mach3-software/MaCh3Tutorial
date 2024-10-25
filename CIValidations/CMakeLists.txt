add_custom_target(ValidApps)

foreach(app
    SplineValidations
    FitterValidations
    CovarianceValidations
    MaCh3ModeValidations
    NuMCMCconversion
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Validations::ValidationsUtils)

    add_dependencies(ValidApps ${app})
endforeach(app)

install(PROGRAMS
    NuMCMCvalidations.sh
    TestNuMCMC.py

    DESTINATION CIValidations)

install(DIRECTORY
    TestOutputs
    Scripts

    DESTINATION CIValidations)
