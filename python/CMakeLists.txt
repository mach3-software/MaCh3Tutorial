
include(${MaCh3_PREFIX}/cmake/Modules/pyMaCh3.cmake)

setup_pyMaCh3(
  TARGET_NAME _pyMaCh3 
  INSTALL_DIR "NONE"
  BINDING_FILES pyMaCh3.cpp
  LINK_TARGETS 
    BinnedSplinesTutorial
    SamplesTutorial
)
