// MaCh3 spline includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"

void CreateChungusYaml(const std::string& filename = "ChungusSystematics.yaml",
                       size_t nParams = 5000)
{
  std::ofstream out(filename);
  if (!out.is_open()) {
    throw std::runtime_error("Failed to open YAML file for writing");
  }
  out << "Systematics:\n\n";

  for (size_t i = 0; i < nParams; ++i) {
    double error   = 0.10;
    double step_scale = 0.2;
    out << "- Systematic:\n";
    out << "    Names:\n";
    out << "      FancyName: Norm_Param_" << i << "\n";
    out << "    Error: " << error << "\n";
    out << "    FlatPrior: false\n";
    out << "    FixParam: false\n";
    out << "    Mode: [ 1 ]\n";
    out << "    ParameterBounds: [0, 999.]\n";
    out << "    ParameterGroup: Xsec\n";
    out << "    TargetNuclei: [12, 16]\n";
    out << "    KinematicCuts:\n";
    out << "    ParameterValues:\n";
    out << "      Generated: 1.\n";
    out << "      PreFitValue: 1.\n";
    out << "    SampleNames: [\"Tutorial_*\", \"ND2137\"]\n";
    out << "    StepScale:\n";
    out << "      MCMC: "<< step_scale <<"\n";
    out << "    Type: Norm\n\n";
  }
}

std::string FormatMB(double value, int precision)
{
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(precision) << value << " MB";
  return ss.str();
}

/// @brief This is super hacky but Catch2 doesn't support RAM printing only time...
void PrintRAMBenchmark(const std::string& name)
{
  double ram = MaCh3Utils::getValue("VmRSS") / 1024.0;

  std::cout << "\n";

  // Main row
  std::cout << std::left  << std::setw(40) << name
            << std::right << std::setw(10) << 100
            << std::right << std::setw(14) << 1
            << std::right << std::setw(11) << std::fixed << std::setprecision(3)
            << ram << " MB\n";

  // Mean row
  std::cout << std::left << std::setw(40) << " "
            << std::right << std::setw(10) << FormatMB(ram, 3)
            << std::right << std::setw(14) << FormatMB(ram, 3)
            << std::right << std::setw(11) << std::fixed << std::setprecision(3)
            << ram << " MB\n";

  // Std dev row
  std::cout << std::left << std::setw(40) << " "
            << std::right << std::setw(10) << FormatMB(0.0, 5)
            << std::right << std::setw(14) << FormatMB(0.0, 5)
            << std::right << std::setw(11) << std::fixed << std::setprecision(5)
            << 0.00000 << " MB\n";
}

TEST_CASE("Benchmark MaCh3") {
  // Initialise manger responsible for config handling
  auto FitManager = std::make_unique<Manager>("TutorialConfigs/FitterConfig.yaml");

  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  std::vector<ParameterHandlerBase*> Covs;
  Covs.push_back(xsec.get());

  // Initialise samplePDF
  auto BeamSamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>({"TutorialConfigs/Samples/SampleHandler_Tutorial.yaml"}, xsec.get());
  auto ATMSamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>({"TutorialConfigs/Samples/SampleHandler_Tutorial_ATM.yaml"}, xsec.get());
  auto NDSamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>({"TutorialConfigs/Samples/SampleHandler_Tutorial_ND.yaml"}, xsec.get());

  BENCHMARK("Beam Reweight") {
    for (size_t s = 0; s < Covs.size(); ++s) {
      Covs[s]->ProposeStep();
      Covs[s]->AcceptStep();
      Covs[s]->GetLikelihood();
    }
    for(unsigned int ivs = 0; ivs < BeamSamples.size(); ivs++ ) {
      BeamSamples[ivs]->Reweight();
      BeamSamples[ivs]->GetLikelihood();
    }
  };

  BENCHMARK("Beam Reweight w/o Osc") {
    for(unsigned int ivs = 0; ivs < BeamSamples.size(); ivs++ ) {
      BeamSamples[ivs]->Reweight();
      BeamSamples[ivs]->GetLikelihood();
    }
  };

  BENCHMARK("ATM Reweight") {
    for (size_t s = 0; s < Covs.size(); ++s) {
      Covs[s]->ProposeStep();
      Covs[s]->AcceptStep();
      Covs[s]->GetLikelihood();
    }
    for(unsigned int ivs = 0; ivs < ATMSamples.size(); ivs++ ) {
      ATMSamples[ivs]->Reweight();
      ATMSamples[ivs]->GetLikelihood();
    }
  };

  BENCHMARK("ND Reweight") {
    for (size_t s = 0; s < Covs.size(); ++s) {
      Covs[s]->ProposeStep();
      Covs[s]->AcceptStep();
      Covs[s]->GetLikelihood();
    }
    for(unsigned int ivs = 0; ivs < NDSamples.size(); ivs++ ) {
      NDSamples[ivs]->Reweight();
      NDSamples[ivs]->GetLikelihood();
    }
  };

  // Get RAM table BEFORE starting chungus
  PrintRAMBenchmark("RAM tracker");

  CreateChungusYaml("ChungusSystematics.yaml", 2000);
  auto Chungus = std::make_unique<ParameterHandlerGeneric>(std::vector<std::string>{"ChungusSystematics.yaml"}, "xsec_cov");

  BENCHMARK("ParameterHandler Operations") {
    Chungus->ProposeStep();
    Chungus->GetLikelihood();
    Chungus->AcceptStep();
  };

  for (size_t i = 0; i < NDSamples.size(); ++i) {
    delete NDSamples[i];
  }
  for (size_t i = 0; i < BeamSamples.size(); ++i) {
    delete BeamSamples[i];
  }
  for (size_t i = 0; i < ATMSamples.size(); ++i) {
    delete ATMSamples[i];
  }
}
