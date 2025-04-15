// MaCh3 spline includes
#include "Fitter/MaCh3Factory.h"
#include "samplePDF/SampleHandlerTutorial.h"

int main(int argc, char *argv[]){
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  std::string OutputName = "LLH_" + FitManager->raw()["General"]["OutputFile"].as<std::string>();
  FitManager->OverrideSettings("General", "OutputFile", OutputName);
  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");
  auto osc  = MaCh3CovarianceFactory<ParameterHandlerOsc>(FitManager.get(), "Osc");

  // Initialise samplePDF
  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SamplePDFFactory<SampleHandlerTutorial>(SampleConfig, xsec.get(), osc.get());

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(xsec.get());
  MaCh3Fitter->AddSystObj(osc.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->AddSampleHandler(mySamples[i]);
  }
  // Run LLH scan
  MaCh3Fitter->RunLLHScan();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
