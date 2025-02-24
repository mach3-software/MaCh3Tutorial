set(HEADERS
    SampleHandlerTutorial.h
    StructsTutorial.h
)

add_library(SampleHandlerTutorial SHARED
    SampleHandlerTutorial.cpp
)

set_target_properties(SampleHandlerTutorial PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME SampleHandlerTutorial)

if(NOT CPU_ONLY)
  set_target_properties(SampleHandlerTutorial PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

target_link_libraries(SampleHandlerTutorial PUBLIC MaCh3::All BinnedSplinesTutorial)
if(MaCh3_CPM)
  target_link_libraries(SampleHandlerTutorial PRIVATE MaCh3Warnings)
endif()

target_include_directories(SampleHandlerTutorial PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)

install(TARGETS SampleHandlerTutorial
        EXPORT mach3tutorial-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/samplePDF)

add_library(MaCh3Tutorial::SampleHandlerTutorial ALIAS SampleHandlerTutorial)
