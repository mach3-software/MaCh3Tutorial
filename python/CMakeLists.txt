
set(MACH3_PYTHON_PATH "${MaCh3_PREFIX}/../python")

# Check the location exists
if(EXISTS "${MACH3_PYTHON_PATH}")
    # Copy to source directory instead of build directory
    set(PYTHON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "Copying MaCh3 core Python files from ${MACH3_PYTHON_PATH} to ${PYTHON_SOURCE_DIR}")
    file(COPY "${MACH3_PYTHON_PATH}/" 
         DESTINATION "${PYTHON_SOURCE_DIR}/")
else()
    message(STATUS "MaCh3 Python source directory not found: ${MACH3_PYTHON_PATH}")
endif()

################################# pybind11 stuff ##################################
pybind11_add_module(
  pyMaCh3 MODULE
  pyMaCh3.cpp
  plotting.cpp
  fitters.cpp
  samples.cpp
  samplesTutorial.cpp
  manager.cpp
  parameters.cpp
  splines.cpp
)

### EM: only works with code compiled with -fPIC enabled.. I think this flag can make things slightly slower
### so would be good to find a way around this.
set_property( TARGET pyMaCh3 PROPERTY POSITION_INDEPENDENT_CODE ON )

# ============================================================================
# RPATH configuration for location-independent loading
# ============================================================================

# Get library directory from MaCh3 installation
if(DEFINED MaCh3_PREFIX)
    set(MACH3_LIB_DIR "${MaCh3_PREFIX}/lib")
endif()

set_target_properties(pyMaCh3 PROPERTIES
    # Automatically add linked library paths to RPATH
    INSTALL_RPATH_USE_LINK_PATH TRUE
    
    # Add explicit RPATH entries
    # $ORIGIN = directory containing pyMaCh3.so at runtime
    INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib:${MACH3_LIB_DIR}"
    
    # Also set BUILD_RPATH so it works before installation
    BUILD_RPATH "${MACH3_LIB_DIR}:${CMAKE_BINARY_DIR}/lib"
    
    # Don't skip RPATH in build tree
    SKIP_BUILD_RPATH FALSE
    
    # IMPORTANT: Set output directory for the .so file
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python/pyMaCh3"
)

message(STATUS "MaCh3 library directory: ${MACH3_LIB_DIR}")
message(STATUS "pyMaCh3 will be built in: ${CMAKE_BINARY_DIR}/python/pyMaCh3")

# ============================================================================

target_link_libraries( pyMaCh3 PRIVATE 
    MaCh3::All
    SamplesTutorial
)
target_include_directories(pyMaCh3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

# Create __init__.py for the package in the build directory
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/pyMaCh3/__init__.py" 
"# pyMaCh3 - Python bindings for MaCh3
from .pyMaCh3 import *

__version__ = '1.0.0'
")

# Install everything from the current source directory
install( DIRECTORY ./ DESTINATION python/pyMaCh3 
         PATTERN "CMakeFiles" EXCLUDE
         PATTERN "*.cmake" EXCLUDE
         PATTERN "Makefile" EXCLUDE
         PATTERN "pyMaCh3" EXCLUDE)  # Don't install the build directory

# Install the pyMaCh3 subdirectory which contains the .so file
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pyMaCh3/ 
         DESTINATION python/pyMaCh3 )
