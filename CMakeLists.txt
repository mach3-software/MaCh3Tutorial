set(CMAKE_VERBOSE_MAKEFILE ON)

# CMake version check
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Create project
project(MaCh3Tutorial VERSION 1.5.0 LANGUAGES CXX)
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
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

include(CIValidations)

##################################  MaCh3  ######################################
option(MaCh3_CPM "Whether MaCh3 was obrained using CPM or find_package" OFF)

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
find_package(MaCh3 QUIET EXACT ${MaCh3Tutorial_VERSION})

if(NOT MaCh3_FOUND)
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

#target_compile_options(MCMC PRIVATE "-pg")
#target_link_options(MCMC PRIVATE "-pg")
#target_compile_options(MaCh3 INTERFACE "-pg")
#target_link_options(MaCh3 INTERFACE "-pg")

#ETA - pretty sure this needs to go into core
#find_package(OpenMP)
#if(OpenMP_CXX_FOUND)
#    target_link_libraries(MaCh3 INTERFACE OpenMP::OpenMP_CXX)
#endif()

if(USE_FPGA)
  if(NOT DEFINED FPGA_DEVICE)
    set(FPGA_DEVICE "Agilex7")
    message(STATUS "FPGA_DEVICE was not specified.\
                    \nConfiguring the design to the default FPGA family target: ${FPGA_DEVICE}\
                    \nPlease refer to the README for information on target selection.")
  endif()

  set(USER_FPGA_FLAGS ${USER_FPGA_FLAGS})

  set(EMULATOR_TARGET fpga_emu)
  set(SIMULATOR_TARGET fpga_sim)
  set(REPORT_TARGET report)
  set(FPGA_TARGET fpga)

  # Set the names of the generated files per makefile target
  set(EMULATOR_OUTPUT_NAME ${TARGET_NAME}.${EMULATOR_TARGET})
  set(SIMULATOR_OUTPUT_NAME ${TARGET_NAME}.${SIMULATOR_TARGET})
  set(REPORT_OUTPUT_NAME ${TARGET_NAME}.${REPORT_TARGET})
  set(FPGA_OUTPUT_NAME ${TARGET_NAME}.${FPGA_TARGET})

  message(STATUS "Additional USER_FPGA_FLAGS=${USER_FPGA_FLAGS}")
  message(STATUS "Additional USER_FLAGS=${USER_FLAGS}")
  set(QACTYPES "-qactypes")

  # target_compile_options(MaCh3 INTERFACE
  #   -fsycl
  #   -fintelfpga
  #   -DFPGA_HARDWARE
  #   #-DFPGA_EMULATOR
  #   -DUSE_FPGA              # preprocessor directive to switch on FPGA code
  # )

endif()

#ETA - similarly this I think is for core
#target_compile_options(MaCh3 INTERFACE "-O0 -fno-eliminate-unused-debug-types")
#target_link_libraries(MaCh3 INTERFACE spdlog::spdlog)
## include_directories(/opt/intel/oneapi/compiler/2024.2/include)
## link_directories(/opt/intel/oneapi/compiler/2024.2/include)

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
add_subdirectory(splines)
add_subdirectory(samplePDF)

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
