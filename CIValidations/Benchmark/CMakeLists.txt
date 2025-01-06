foreach(TEST
    MaCh3Benchmark
    )

    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} PRIVATE Catch2::Catch2WithMain MaCh3::All MaCh3Tutorial::samplePDFTutorial)
    catch_discover_tests(${TEST})
endforeach()
