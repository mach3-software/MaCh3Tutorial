// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[]){

  MaCh3Utils::MaCh3Usage(argc, argv);
  // Initialise manger responsible for config handling
  auto FitManager = std::make_unique<manager>(argv[1]);
  FitManager->OverrideSettings("General", "OutputFile", "LLH_Test.root");
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
  // Run LLH scan
  MaCh3Fitter->RunLLHScan();

  delete xsec;
  delete osc;
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
