#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

extern void register_handler(py::module& m);

PYBIND11_MODULE( _pyMaCh3Tutorial, m ) {
    register_handler(m);
}