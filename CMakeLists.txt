set(CMAKE_VERBOSE_MAKEFILE ON)

# CMake version check
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

SET(MaCh3Tutorial_VERSION 1.1.4)

# Create project (todo: set configurable CUDA)
project(MaCh3Tutorial VERSION ${MaCh3Tutorial_VERSION} LANGUAGES CXX)
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

message("CMAKE SOURCE_DIR IS ${CMAKE_SOURCE_DIR}")


################################################################################
#                            Check Dependencies
################################################################################

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules)
include(CPM)

CPMFindPackage(
      NAME CMakeModules
      GIT_TAG stable
      GITHUB_REPOSITORY NuHepMC/CMakeModules
      DOWNLOAD_ONLY
  )
include(${CMakeModules_SOURCE_DIR}/NuHepMCModules.cmake)

include(ROOT)

if(NOT TARGET ROOT::ROOT)
  cmessage(FATAL_ERROR "T2KReWeight Expected dependency target: ROOT::ROOT")
endif()

if(DEFINED ROOT_CXX_STANDARD AND ROOT_CXX_STANDARD GREATER CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
endif()

##################################  MaCh3  ######################################
if(NOT DEFINED MaCh3_Branch)
  SET(MaCh3_Branch "develop")
endif()

#If MaCh3 was sourced find it, otherwise use CPM
find_package(MaCh3 QUIET)

if(NOT MaCh3_FOUND)
  cmessage(STATUS "Didn't find MaCh3, attempting to use built-in MaCh3")

  CPMAddPackage(
    NAME MaCh3
    GIT_TAG ${MaCh3_Branch}
    GITHUB_REPOSITORY mach3-software/MaCh3
  )
endif()

if(NOT TARGET MaCh3::All)
  cmessage(FATAL_ERROR "MaCh3 Tutorial Expected dependency target: MaCh3::All, did you source setup.MaCh3.sh")
endif()

if(MaCh3_VERSION GREATER MaCh3Tutorial_VERSION)
  cmessage(WARNING "MaCh3Tutorial_VERSION (${MaCh3Tutorial_VERSION}) is smaller than MaCh3_VERSION (${MaCh3_VERSION}), this most likely will not work")
endif()

############################  C++ Compiler  ####################################
if (NOT DEFINED CMAKE_CXX_STANDARD OR "${CMAKE_CXX_STANDARD} " STREQUAL " ")
  SET(CMAKE_CXX_STANDARD 11)
endif()

if(DEFINED ROOT_CXX_STANDARD AND ROOT_CXX_STANDARD GREATER CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
endif()
cmessage(STATUS "CMAKE CXX Standard: ${CMAKE_CXX_STANDARD}")

################################# Build MaCh3 ##################################
add_subdirectory(Utils)
add_subdirectory(Apps)
add_subdirectory(Tutorial)

configure_file(cmake/Templates/setup.MaCh3Tutorial.sh.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.MaCh3Tutorial.sh" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/setup.MaCh3Tutorial.sh" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

############################  Install  ####################################
install(DIRECTORY Inputs DESTINATION ${CMAKE_BINARY_DIR})
install(DIRECTORY Scripts DESTINATION ${CMAKE_BINARY_DIR})

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
