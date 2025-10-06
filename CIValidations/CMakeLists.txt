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
    SigmaVarValidations
    SigmaVarFDValidation
    LLHValidation
    PredictiveValidations
    BinningValidations
    )

    add_executable(${app} ${app}.cpp)
    target_link_libraries(${app} MaCh3Tutorial::ValidationsUtils MaCh3Tutorial::SamplesTutorial)
    if(MaCh3Tutorial_Coverage_ENABLED)
      target_compile_options(${app} PRIVATE --coverage)
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

if(MaCh3Tutorial_Benchmark_ENABLED)
  add_subdirectory(Benchmark)
endif()

add_subdirectory(SplinePlotting)
