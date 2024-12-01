add_custom_target(ValidApps)

foreach(app
    SplineValidations
    FitterValidations
    CovarianceValidations
    MaCh3ModeValidations
    SamplePDFValidations
    NuMCMCconversion
    NuOscillatorInterfaceValidations
    pValueValidations
    )

    add_executable(${app} ${app}.cpp)
    if(MaCh3Tutorial_Coverage_ENABLED)
    target_compile_options(${app} PRIVATE --coverage)
    endif()
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::samplePDFTutorial)
    if(MaCh3Tutorial_Coverage_ENABLED)
    target_link_options(${app} PRIVATE --coverage)
    endif()
    add_dependencies(ValidApps ${app})
endforeach(app)

install(PROGRAMS
    NuMCMCvalidations.sh
    TestNuMCMC.py
    MaCh3CLI

    DESTINATION CIValidations)

install(DIRECTORY
    TestOutputs
    Scripts

    DESTINATION CIValidations)

if(MaCh3Tutorial_UNITTESTS_ENABLED)
  add_subdirectory(UnitTests)
endif()
