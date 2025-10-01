set(MACH3_PYTHON_PATH "${MaCh3_PREFIX}/../python")

# Check the location exists
if(EXISTS "${MACH3_PYTHON_PATH}")
    message(STATUS "Copying MaCh3 core Python files from ${MACH3_PYTHON_PATH} to ${CMAKE_CURRENT_BINARY_DIR}")
    file(COPY "${MACH3_PYTHON_PATH}/" 
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
else()
    message(STATUS "MaCh3 Python source directory not found: ${MACH3_PYTHON_PATH}")
endif()

################################# pybind11 stuff ##################################
# EM: make a module target out of all the python*Module.cpp files (currently just one...)
pybind11_add_module(
  _pyMaCh3 MODULE
  pyMaCh3.cpp
  plotting.cpp
  fitters.cpp
  samples.cpp
  manager.cpp
  parameters.cpp
  splines.cpp
)

## EM: only works with code compiled with -fPIC enabled.. I think this flag can make things slightly slower
## so would be good to find a way around this.
set_property( TARGET _pyMaCh3 PROPERTY POSITION_INDEPENDENT_CODE ON )
target_link_libraries( _pyMaCh3 PRIVATE MaCh3::All NuOscillator MaCh3Warnings )

install( DIRECTORY pyMaCh3 DESTINATION ./ )
install( TARGETS _pyMaCh3 DESTINATION pyMaCh3/ )