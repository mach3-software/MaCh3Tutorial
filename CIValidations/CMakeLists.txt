add_custom_target(ValidApps)

foreach(app
    SplineValidations
    FitterValidations
    CovarianceValidations
    MaCh3ModeValidations
    SamplePDFValidations
    NuMCMCconversion
    )
    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::samplePDFTutorial)

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
