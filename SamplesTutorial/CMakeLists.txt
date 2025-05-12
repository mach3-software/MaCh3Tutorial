set(HEADERS
    SampleHandlerTutorial.h
    StructsTutorial.h
)

add_library(SamplesTutorial SHARED
    SampleHandlerTutorial.cpp
)

set_target_properties(SamplesTutorial PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME SamplesTutorial)

if(MaCh3_GPU_ENABLED)
  set_target_properties(SamplesTutorial PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

target_link_libraries(SamplesTutorial PUBLIC MaCh3::All BinnedSplinesTutorial)
if(MaCh3_CPM)
  target_link_libraries(SamplesTutorial PRIVATE MaCh3Warnings)
endif()

target_include_directories(SamplesTutorial PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)

install(TARGETS SamplesTutorial
        EXPORT mach3tutorial-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/SamplesTutorial)

add_library(MaCh3Tutorial::SamplesTutorial ALIAS SamplesTutorial)
