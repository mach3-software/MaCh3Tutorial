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


YAML::Node CreateUniformBinnins() {
  // --- Uniform Binning ---
  std::string yamlUniform = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2"]
VarBins: [ [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1],
           [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1] ]
Uniform: true
)";
  YAML::Node ConfigUniform = STRINGtoYAML(yamlUniform);
  return ConfigUniform;
}

YAML::Node CreateNonUniformBinnins() {
  // --- Non-Uniform Binning ---
  std::string yamlNonUniform = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2"]
Bins: [
  [[0.0, 0.05],[0.0, 0.04]], [[0.0, 0.05],[0.04, 0.09]], [[0.0, 0.05],[0.09, 0.15]], [[0.0, 0.05],[0.15, 0.22]], [[0.0, 0.05],[0.22, 0.30]],
  [[0.05, 0.10],[0.0, 0.05]], [[0.05, 0.10],[0.05, 0.11]], [[0.05, 0.10],[0.11, 0.18]], [[0.05, 0.10],[0.18, 0.26]], [[0.05, 0.10],[0.26, 0.35]],
  [[0.10, 0.20],[0.0, 0.06]], [[0.10, 0.20],[0.06, 0.13]], [[0.10, 0.20],[0.13, 0.21]], [[0.10, 0.20],[0.21, 0.30]], [[0.10, 0.20],[0.30, 0.40]],
  [[0.20, 0.30],[0.0, 0.07]], [[0.20, 0.30],[0.07, 0.15]], [[0.20, 0.30],[0.15, 0.24]], [[0.20, 0.30],[0.24, 0.34]], [[0.20, 0.30],[0.34, 0.45]],
  [[0.30, 0.40],[0.0, 0.08]], [[0.30, 0.40],[0.08, 0.17]], [[0.30, 0.40],[0.17, 0.27]], [[0.30, 0.40],[0.27, 0.38]], [[0.30, 0.40],[0.38, 0.50]],
  [[0.40, 0.50],[0.0, 0.09]], [[0.40, 0.50],[0.09, 0.19]], [[0.40, 0.50],[0.19, 0.30]], [[0.40, 0.50],[0.30, 0.42]], [[0.40, 0.50],[0.42, 0.55]],
  [[0.50, 0.60],[0.0, 0.10]], [[0.50, 0.60],[0.10, 0.21]], [[0.50, 0.60],[0.21, 0.33]], [[0.50, 0.60],[0.33, 0.46]], [[0.50, 0.60],[0.46, 0.60]],
  [[0.60, 0.70],[0.0, 0.11]], [[0.60, 0.70],[0.11, 0.23]], [[0.60, 0.70],[0.23, 0.36]], [[0.60, 0.70],[0.36, 0.50]], [[0.60, 0.70],[0.50, 0.65]],
  [[0.70, 0.80],[0.0, 0.12]], [[0.70, 0.80],[0.12, 0.25]], [[0.70, 0.80],[0.25, 0.39]], [[0.70, 0.80],[0.39, 0.54]], [[0.70, 0.80],[0.54, 0.70]],
  [[0.80, 0.90],[0.0, 0.13]], [[0.80, 0.90],[0.13, 0.27]], [[0.80, 0.90],[0.27, 0.42]], [[0.80, 0.90],[0.42, 0.58]], [[0.80, 0.90],[0.58, 0.75]],
  [[0.90, 1.00],[0.0, 0.14]], [[0.90, 1.00],[0.14, 0.29]], [[0.90, 1.00],[0.29, 0.45]], [[0.90, 1.00],[0.45, 0.62]], [[0.90, 1.00],[0.62, 0.80]]
]
Uniform: false
)";
  YAML::Node ConfigNonUniform = STRINGtoYAML(yamlNonUniform);
  return ConfigNonUniform;
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

  CreateChungusYaml("ChungusSystematics.yaml", 2000);
  auto Chungus = std::make_unique<ParameterHandlerGeneric>(std::vector<std::string>{"ChungusSystematics.yaml"}, "xsec_cov");

  BENCHMARK("ParameterHandler Operations") {
    Chungus->ProposeStep();
    Chungus->GetLikelihood();
    Chungus->AcceptStep();
  };

  constexpr size_t nEvents = 100000;

  YAML::Node ConfigUniform = CreateUniformBinnins();
  auto UniformBinning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  UniformBinning->SetupSampleBinning(ConfigUniform, SingleSample);

  auto NonUniformBinning = std::make_unique<BinningHandler>();
  YAML::Node ConfigNonUniform = CreateNonUniformBinnins();
  NonUniformBinning->SetupSampleBinning(ConfigNonUniform, SingleSample);

  // Generate random events
  std::vector<std::pair<double,double>> Events(nEvents);
  for(size_t i = 0; i < nEvents; ++i) {
    double x = static_cast<double>(rand()) / RAND_MAX; // 0..1
    double y = static_cast<double>(rand()) / RAND_MAX; // 0..1
    Events[i] = std::make_pair(x, y);
  }

  BENCHMARK("Uniform BinningHandler 100k events") {
    for(const auto& ev : Events) {
      std::vector<const double*> KinVars = { &ev.first, &ev.second };
      std::vector<int> NomBins = { -1, -1 };
      UniformBinning->FindGlobalBin(0, KinVars, NomBins);
    }
  };

  BENCHMARK("Non-Uniform BinningHandler") {
    for(const auto& ev : Events) {
      std::vector<const double*> KinVars = { &ev.first, &ev.second };
      std::vector<int> NomBins = { -1, -1 };
      NonUniformBinning->FindGlobalBin(0, KinVars, NomBins);
    }
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
