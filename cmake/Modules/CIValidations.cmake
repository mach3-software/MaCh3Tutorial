option(MaCh3Tutorial_UNITTESTS_ENABLED "Whether to build MaCh3 Unit Tests" OFF)
option(MaCh3Tutorial_Coverage_ENABLED "Whether to build MaCh3 Coverage" OFF)
option(MaCh3Tutorial_Benchmark_ENABLED "Enable benchmarking" OFF)

CPMAddPackage(
  NAME NuMCMCTools
  GIT_TAG "tags/v1.0.0"
  GIT_SHALLOW YES
  GITHUB_REPOSITORY NuMCMCTools/NuMCMCTools
  DOWNLOAD_ONLY YES
)

install(DIRECTORY
    ${CPM_PACKAGE_NuMCMCTools_SOURCE_DIR}/
    DESTINATION ${CMAKE_BINARY_DIR}/NuMCMCTools)

############################  Catch2/CTest  ####################################
if(MaCh3Tutorial_UNITTESTS_ENABLED OR MaCh3Tutorial_Benchmark_ENABLED)
  find_package(Catch2 QUIET)
  if(NOT Catch2_FOUND)
    CPMAddPackage("gh:catchorg/Catch2@3.5.2")
    LIST(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
  endif()

  include(CTest)
  include(Catch)
endif()
