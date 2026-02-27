#include "python/pyMaCh3.h"

#include "SamplesTutorial/SampleHandlerTutorial.h"
#include <memory>

namespace py = pybind11;

class MaCh3TutorialPyBinder : public MaCh3PyBinder {

  public:

    void initSamplesExperiment(py::module &m){

        std::cout << "Initializing SampleHandlerTutorial bindings... " << std::endl;

        // Create the samples submodule
        auto m_samples = m.def_submodule("tutorial_samples");
        m_samples.doc() = "This is a Python binding of MaCh3s C++ based samples library.";
        
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
};

MAKE_PYMACH3_MDULE( MaCh3TutorialPyBinder )
