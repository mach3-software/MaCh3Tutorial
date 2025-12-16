
################################# pybind11 stuff ##################################
## EM: make a module target out of all the python*Module.cpp files (currently just one...)
pybind11_add_module(
  _pyMaCh3Tutorial MODULE
  module.cpp
  wrapper.cpp
)
## EM: only works with code compiled with -fPIC enabled.. I think this flag can make things slightly slower
## so would be good to find a way around this.
set_property( TARGET _pyMaCh3Tutorial PROPERTY POSITION_INDEPENDENT_CODE ON )
target_link_libraries( _pyMaCh3Tutorial PUBLIC MaCh3::All MaCh3Tutorial::SamplesTutorial NuOscillator yaml-cpp::yaml-cpp MaCh3Warnings )
install( DIRECTORY pyMaCh3Tutorial DESTINATION ./ )
install( TARGETS _pyMaCh3Tutorial DESTINATION pyMaCh3Tutorial/ )
