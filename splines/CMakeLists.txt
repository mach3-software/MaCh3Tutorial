set(HEADERS BinnedSplinesTutorial.h
)

add_library(BinnedSplinesTutorial SHARED
    BinnedSplinesTutorial.cpp
)

set_target_properties(BinnedSplinesTutorial PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME BinnedSplinesTutorial)

target_link_libraries(BinnedSplinesTutorial MaCh3::All)

target_include_directories(BinnedSplinesTutorial PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)

install(TARGETS BinnedSplinesTutorial
        EXPORT mach3tutorial-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/splines)

      add_library(MaCh3Tutorial::BinnedSplinesTutorial ALIAS BinnedSplinesTutorial)
