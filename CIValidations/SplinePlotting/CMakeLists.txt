foreach(TEST
    SplinePlotting
    )

    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} PRIVATE MaCh3::All)
endforeach()
