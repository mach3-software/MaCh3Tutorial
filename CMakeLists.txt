set(CMAKE_VERBOSE_MAKEFILE ON)

# CMake version check
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

SET(MaCh3Tutorial_VERSION 1.1.6)

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
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)

message("CMAKE_CURRENT_BINARY_DIR IS ${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

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
#find_package(MaCh3 QUIET)

if(NOT MaCh3_FOUND)
  cmessage(STATUS "Didn't find MaCh3, attempting to use built-in MaCh3")

  CPMAddPackage(
    NAME MaCh3
    GIT_TAG ${MaCh3_Branch}
    GITHUB_REPOSITORY mach3-software/MaCh3
  )
endif()

#target_compile_options(MCMC PRIVATE "-pg")
#target_link_options(MCMC PRIVATE "-pg")
#target_compile_options(MaCh3 INTERFACE "-pg")
#target_link_options(MaCh3 INTERFACE "-pg")

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(MaCh3 INTERFACE OpenMP::OpenMP_CXX)
endif()

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

  target_compile_options(MaCh3 INTERFACE
    -fsycl
    -fintelfpga
    -DFPGA_EMULATOR
    -DUSE_FPGA              # preprocessor directive to switch on FPGA code
  )

endif()

target_compile_options(MaCh3 INTERFACE "-O0 -fno-eliminate-unused-debug-types")
target_link_libraries(MaCh3 INTERFACE spdlog::spdlog)
# include_directories(/opt/intel/oneapi/compiler/2024.2/include)
# link_directories(/opt/intel/oneapi/compiler/2024.2/include)

if(NOT TARGET MaCh3::All)
  cmessage(FATAL_ERROR "MaCh3 Tutorial Expected dependency target: MaCh3::All, did you source setup.MaCh3.sh")
endif()

if(MaCh3_VERSION GREATER MaCh3Tutorial_VERSION)
  cmessage(WARNING "MaCh3Tutorial_VERSION (${MaCh3Tutorial_VERSION}) is smaller than MaCh3_VERSION (${MaCh3_VERSION}), this most likely will not work")
endif()

CPMAddPackage(
  NAME NuMCMCTools
  GIT_TAG main
  GITHUB_REPOSITORY NuMCMCTools/NuMCMCTools
  DOWNLOAD_ONLY YES
)

install(DIRECTORY
    ${CPM_PACKAGE_NuMCMCTools_SOURCE_DIR}/
    DESTINATION ${CMAKE_BINARY_DIR}/NuMCMCTools)

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
