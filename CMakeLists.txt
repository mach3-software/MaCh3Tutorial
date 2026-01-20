set(CMAKE_VERBOSE_MAKEFILE ON)

# CMake version check
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Create project
project(MaCh3Tutorial VERSION 2.4.0 LANGUAGES CXX)
set(MaCh3Tutorial_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD_REQUIRED True)

# Changes default install path to be a subdirectory of the build dir.
# Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
endif()

# Use the compilers found in the path
find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

################################################################################
#                            Check Dependencies
################################################################################

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules)
# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

include(CIValidations)

##################################  MaCh3  ######################################
option(MaCh3_CPM "Whether MaCh3 was obtained using CPM or find_package" OFF)
option(MaCh3Tutorial_PROFILING_ENABLED "Will run with gperftools" OFF)
option(Use_External_MaCh3 "Use an externally installed MaCh3 instead of CPM" OFF)

# KS: Here we try to find tag matching tutorial version. If we can't find one then use develop
# This will allow to grab tutorial for tagged MaCh3 version without a need of manually changing version
if(NOT DEFINED MaCh3_Branch)
  set(REPO_URL "https://api.github.com/repos/mach3-software/MaCh3/tags")
  # Execute curl to fetch the tags from GitHub
  execute_process(
      COMMAND curl -s ${REPO_URL}
      OUTPUT_VARIABLE curl_output
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # Check if the tag exists in the response
  string(FIND "${curl_output}" "\"name\": \"v${MaCh3Tutorial_VERSION}\"" tag_found_index)

  if(tag_found_index GREATER -1)
      SET(MaCh3_Branch "tags/v${MaCh3Tutorial_VERSION}")
  else()
      SET(MaCh3_Branch "develop")
  endif()
endif()

#If MaCh3 was sourced find it, otherwise use CPM
if(Use_External_MaCh3)
  find_package(MaCh3 EXACT ${MaCh3Tutorial_VERSION})
endif()

if(NOT MaCh3_FOUND)
  message(STATUS "MaCh3 is not found locally!")
  set(MaCh3_OPTIONS "")

  if(PYTHON_ENABLED)
      list(APPEND MaCh3_OPTIONS "MaCh3_PYTHON_ENABLED ${PYTHON_ENABLED}")
  endif()

  # KS: For benchmark we turn off logger to not interfere with output messages.
  # Also to have code compile let's turn off Werror
  if (MaCh3Tutorial_Benchmark_ENABLED)
    list(APPEND MaCh3_OPTIONS "MaCh3_WERROR_ENABLED OFF")
    set(LOG_LEVEL "OFF")
  endif()

  # Add LOG_LEVEL if defined
  if (DEFINED LOG_LEVEL)
    list(APPEND MaCh3_OPTIONS "LOG_LEVEL ${LOG_LEVEL}")
  endif()

  if (MaCh3Tutorial_NATIVE_ENABLED)
    list(APPEND MaCh3_OPTIONS "MaCh3_NATIVE_ENABLED ON")
  endif()

  CPMAddPackage(
    NAME MaCh3
    GIT_TAG ${MaCh3_Branch}
    GIT_SHALLOW YES
    GITHUB_REPOSITORY mach3-software/MaCh3
    OPTIONS
      ${MaCh3_OPTIONS}
      "Prob3ppLinear_ENABLED ON"
      "NuFastLinear_ENABLED ON"
      "CUDAProb3_ENABLED ON"
  )
  # MaCh3 sets up cmessage for us, so can't cmessage until we've set up MaCh3
  cmessage(STATUS "Didn't find MaCh3, will checkout the ${MaCh3_Branch} branch from mach3-software/MaCh3")
  set(MaCh3_CPM TRUE)
else()
  # If you
  cmessage(STATUS "Using local build of MaCh3: ${MaCh3_DIR}")
endif()

if(NOT TARGET MaCh3::All)
  cmessage(FATAL_ERROR "MaCh3 Tutorial Expected dependency target: MaCh3::All, did you source setup.MaCh3.sh")
endif()

if(MaCh3_VERSION GREATER MaCh3Tutorial_VERSION)
  cmessage(WARNING "MaCh3Tutorial_VERSION (${MaCh3Tutorial_VERSION}) is smaller than MaCh3_VERSION (${MaCh3_VERSION}), this most likely will not work")
endif()

############################  C++ Compiler  ####################################
get_property(MACH3_CXX_STANDARD GLOBAL PROPERTY MACH3_CXX_STANDARD)
if(NOT DEFINED MACH3_CXX_STANDARD)
  SET(CMAKE_CXX_STANDARD 14)
else()
  SET(CMAKE_CXX_STANDARD ${MACH3_CXX_STANDARD})
endif()
cmessage(STATUS "CMAKE CXX Standard: ${CMAKE_CXX_STANDARD}")

################################# Build MaCh3 ##################################
add_subdirectory(Utils)
add_subdirectory(CIValidations)
add_subdirectory(Tutorial)
add_subdirectory(SplinesTutorial)
add_subdirectory(SamplesTutorial)

configure_file(cmake/Templates/setup.MaCh3Tutorial.sh.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.MaCh3Tutorial.sh" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.MaCh3Tutorial.sh" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

############################  Install  ####################################
install(DIRECTORY TutorialConfigs DESTINATION ${CMAKE_BINARY_DIR})
install(DIRECTORY plotting DESTINATION ${CMAKE_BINARY_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/MaCh3TutorialConfig.cmake.in ${CMAKE_BINARY_DIR}/MaCh3TutorialConfig.cmake
  INSTALL_DESTINATION
    /this/is/ignored/for/some/reason/thanks/kitware
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

  write_basic_package_version_file(${CMAKE_BINARY_DIR}/MaCh3TutorialConfigVersion.cmake
  VERSION ${MaCh3_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES
    ${CMAKE_BINARY_DIR}/MaCh3TutorialConfig.cmake
    ${CMAKE_BINARY_DIR}/MaCh3TutorialConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/MaCh3Tutorial)

include(mach3tutorial-config)
