// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "samplePDF/samplePDFTutorial.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"

TEST_CASE("Benchmark MaCh3") {
  // Initialise manger responsible for config handling
  auto FitManager = std::make_unique<manager>("TutorialConfigs/FitterConfig.yaml");

  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<covarianceXsec>(FitManager.get(), "Xsec");
  auto osc  = MaCh3CovarianceFactory<covarianceOsc>(FitManager.get(), "Osc");

  std::vector<covarianceBase*> Covs;
  Covs.push_back(xsec.get());
  Covs.push_back(osc.get());

  // Initialise samplePDF
  auto BeamSamples = MaCh3SamplePDFFactory<samplePDFTutorial>({"TutorialConfigs/Samples/SamplePDF_Tutorial.yaml"}, xsec.get(), osc.get());
  auto ATMSamples = MaCh3SamplePDFFactory<samplePDFTutorial>({"TutorialConfigs/Samples/SamplePDF_Tutorial_ATM.yaml"}, xsec.get(), osc.get());

  BENCHMARK("Beam Reweight") {
    for (size_t s = 0; s < Covs.size(); ++s) {
      Covs[s]->proposeStep();
      Covs[s]->acceptStep();
      Covs[s]->GetLikelihood();
    }
    for(unsigned int ivs = 0; ivs < BeamSamples.size(); ivs++ ) {
      BeamSamples[ivs]->reweight();
      BeamSamples[ivs]->GetLikelihood();
    }
  };

  BENCHMARK("Beam Reweight w/o Osc") {
    for(unsigned int ivs = 0; ivs < BeamSamples.size(); ivs++ ) {
      BeamSamples[ivs]->reweight();
      BeamSamples[ivs]->GetLikelihood();
    }
  };

  BENCHMARK("ATM Reweight") {
    for (size_t s = 0; s < Covs.size(); ++s) {
      Covs[s]->proposeStep();
      Covs[s]->acceptStep();
      Covs[s]->GetLikelihood();
    }
    for(unsigned int ivs = 0; ivs < ATMSamples.size(); ivs++ ) {
      ATMSamples[ivs]->reweight();
      ATMSamples[ivs]->GetLikelihood();
    }
  };

  for (size_t i = 0; i < BeamSamples.size(); ++i) {
    delete BeamSamples[i];
  }
  for (size_t i = 0; i < ATMSamples.size(); ++i) {
    delete ATMSamples[i];
  }
}
