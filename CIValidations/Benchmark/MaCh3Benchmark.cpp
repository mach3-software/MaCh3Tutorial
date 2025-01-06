// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "samplePDF/samplePDFTutorial.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"

TEST_CASE("Benchmark MaCh3") {
  // Initialise manger responsible for config handling
  auto FitManager = std::make_unique<manager>("Inputs/FitterConfig.yaml");

  // Initialise covariance class reasonable for Systematics
  covarianceXsec* xsec = MaCh3CovarianceFactory(FitManager.get(), "Xsec");
  covarianceOsc*  osc  = MaCh3CovarianceFactory<covarianceOsc>(FitManager.get(), "Osc");

  // Initialise samplePDF
  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SamplePDFFactory<samplePDFTutorial>(SampleConfig, xsec, osc);

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSystObj(osc);
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->addSamplePDF(mySamples[i]);
  }
  // Benchmark
  BENCHMARK("MaCh3Fitter::DragRace") {
    MaCh3Fitter->DragRace(1000);
  };

  delete xsec;
  delete osc;
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
}
