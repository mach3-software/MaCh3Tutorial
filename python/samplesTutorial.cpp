#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "SamplesTutorial/SampleHandlerTutorial.h"
#include <memory>

namespace py = pybind11;

// Forward declare initSamples from samples.cpp
void initSamples(py::module &m_samples);

void initSamplesTutorial(py::module &m){

    std::cout << "Initializing SampleHandlerTutorial bindings... " << std::endl;

    // Create the samples submodule
    auto m_samples = m.def_submodule("samples");
    m_samples.doc() = "This is a Python binding of MaCh3s C++ based samples library.";
    
    // First, add all the core MaCh3 samples bindings
    initSamples(m_samples);
    
    // Now add SampleHandlerTutorial to the same submodule
    py::class_<SampleHandlerTutorial, SampleHandlerFD, SampleHandlerBase>(m_samples, "SampleHandlerTutorial")
        // Constructor with 2 arguments (no oscillation handler)
        .def(py::init([](const std::string& mc_version, ParameterHandlerGeneric* xsec_cov) {
            return new SampleHandlerTutorial(mc_version, xsec_cov, nullptr);
        }),
             "Create SampleHandlerTutorial without oscillation handler",
             py::arg("mc_version"),
             py::arg("xsec_cov")
        )
        // Constructor with 3 arguments (with oscillation handler)
        .def(py::init([](const std::string& mc_version, 
                        ParameterHandlerGeneric* xsec_cov,
                        OscillationHandler* osc_cov) {
            std::shared_ptr<OscillationHandler> osc_ptr;
            if (osc_cov != nullptr) {
                osc_ptr = std::shared_ptr<OscillationHandler>(osc_cov, [](OscillationHandler*){});
            }
            return new SampleHandlerTutorial(mc_version, xsec_cov, osc_ptr);
        }),
             "Create SampleHandlerTutorial with oscillation handler",
             py::arg("mc_version"),
             py::arg("xsec_cov"),
             py::arg("osc_cov") = nullptr
        )
        ;
}