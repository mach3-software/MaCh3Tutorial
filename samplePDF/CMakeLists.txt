set(HEADERS
    samplePDFTutorial.h
    StructsTutorial.h
)

add_library(samplePDFTutorial SHARED
    samplePDFTutorial.cpp
)

set_target_properties(samplePDFTutorial PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME samplePDFTutorial)

if(MaCh3_GPU_ENABLED)
  set_target_properties(samplePDFTutorial PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

target_link_libraries(samplePDFTutorial PUBLIC MaCh3::All BinnedSplinesTutorial)
if(MaCh3_CPM)
  target_link_libraries(samplePDFTutorial PRIVATE MaCh3Warnings)
endif()

target_include_directories(samplePDFTutorial PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)

install(TARGETS samplePDFTutorial
        EXPORT mach3tutorial-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/samplePDF)

add_library(MaCh3Tutorial::samplePDFTutorial ALIAS samplePDFTutorial)
