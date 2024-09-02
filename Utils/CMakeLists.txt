set(HEADERS
        SplineMonoUtils.h
)

add_library(ValidationsUtils SHARED
    SplineMonoUtils.cpp
)

set_target_properties(ValidationsUtils PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME ValidationsUtils)


target_link_libraries(ValidationsUtils MaCh3::All)

target_include_directories(ValidationsUtils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<INSTALL_INTERFACE:include>)
