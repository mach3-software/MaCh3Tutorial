/*
HW: VERY simple wrapper to expose some of MaCh3 to python
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <list>
#include <string>
#include <vector>

#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

namespace py = pybind11;

/// Simple wrapper around MaCh3
class MaCh3TutorialWrapper{
    public:
        MaCh3TutorialWrapper(std::string config_file){
            man = std::make_unique<manager>(config_file);
            par_handler = MaCh3CovarianceFactory<ParameterHandlerGeneric>(man.get(), "Xsec");
            auto sample_configs = Get<std::vector<std::string>>(man->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
            sample_handlers = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(sample_configs, par_handler.get());
        }

        void SetParVals(std::vector<double> vals){
            par_handler->SetParameters(vals);
        }

        void Reweight(){
            for(auto& sample : sample_handlers){
                sample->Reweight();
            }
        }

        std::vector<double> GetMCBins(){
            std::vector<double> mc_bins;
            for(auto& sample : sample_handlers){
                auto sample_bins = sample->GetMCVals();
                mc_bins.insert(mc_bins.end(), sample_bins.begin(), sample_bins.end());
            }
            return mc_bins;
        }

        std::vector<double> GetDataBins(){
            std::vector<double> data_bins;
            for(auto& sample : sample_handlers){
                auto sample_bins = sample->GetDataVals();
                data_bins.insert(data_bins.end(), sample_bins.begin(), sample_bins.end());
            }
            return data_bins;
        }

        std::vector<double> simulate(std::vector<double> par_vals){
            SetParVals(par_vals);
            Reweight();
            return GetMCBins();
        }

        std::vector<std::vector<double>> simulate(std::vector<std::vector<double>> par_vals){
            std::vector<std::vector<double>> bins;
            bins.reserve(par_vals.size());
            for(auto& x: par_vals){
                bins.push_back(simulate(x));
            }
            return bins;
        }


    protected:
        std::unique_ptr<manager> man;
        std::unique_ptr<ParameterHandlerGeneric> par_handler;
        std::vector<SampleHandlerTutorial*> sample_handlers;
};


void register_handler(py::module& m){
    py::class_<MaCh3TutorialWrapper>(m, "MaCh3TutorialWrapper")
        .def(py::init<const std::string &>())
        .def("set_par_vals", &MaCh3TutorialWrapper::SetParVals, py::arg("vals"))
        .def("reweight", &MaCh3TutorialWrapper::Reweight)
        .def("get_mc_bins", &MaCh3TutorialWrapper::GetMCBins)
        .def("get_data_bins", &MaCh3TutorialWrapper::GetDataBins)
        .def("simulate", py::overload_cast<std::vector<double>>(&MaCh3TutorialWrapper::simulate), py::arg("vals"))
        .def("simulate", py::overload_cast<std::vector<std::vector<double>>>(&MaCh3TutorialWrapper::simulate), py::arg("vals"));
    ;
}