// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[]){
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  std::string OutputName = "LLH_" + FitManager->raw()["General"]["OutputFile"].as<std::string>();
  FitManager->OverrideSettings("General", "OutputFile", OutputName);
  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<covarianceXsec>(FitManager.get(), "Xsec");
  auto osc  = MaCh3CovarianceFactory<covarianceOsc>(FitManager.get(), "Osc");

  // Initialise samplePDF
  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SamplePDFFactory<samplePDFTutorial>(SampleConfig, xsec.get(), osc.get());

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec.get());
  MaCh3Fitter->addSystObj(osc.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->addSamplePDF(mySamples[i]);
  }
  // Run LLH scan
  MaCh3Fitter->RunLLHScan();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
