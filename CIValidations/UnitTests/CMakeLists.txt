foreach(TEST
    manager_tests
    histogram_tests
    statistical_tests
    )

    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} PRIVATE Catch2::Catch2WithMain MaCh3::All)
    catch_discover_tests(${TEST})
endforeach()
