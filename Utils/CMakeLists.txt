set(HEADERS
    SplineMonoUtils.h
    Comparison.h
)

add_library(ValidationsUtils SHARED
    SplineMonoUtils.cpp
    Comparison.cpp
)

set_target_properties(ValidationsUtils PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME ValidationsUtils)

target_link_libraries(ValidationsUtils PUBLIC MaCh3::All)
target_link_options(ValidationsUtils PRIVATE
  -fsycl
  -fintelfpga
)
if(MaCh3_CPM)
  target_link_libraries(ValidationsUtils PRIVATE MaCh3Warnings)
endif()
target_include_directories(ValidationsUtils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)

install(TARGETS ValidationsUtils
        EXPORT mach3tutorial-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/Utils)

add_library(MaCh3Tutorial::ValidationsUtils ALIAS ValidationsUtils)
