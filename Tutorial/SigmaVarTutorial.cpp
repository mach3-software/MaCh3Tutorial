// MaCh3 spline includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

int main(int argc, char *argv[]){
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  std::string OutputName = "SigmaVar_" + FitManager->raw()["General"]["OutputFile"].as<std::string>();
  FitManager->OverrideSettings("General", "OutputFile", OutputName);
  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(xsec.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->AddSampleHandler(mySamples[i]);
  }
  // Run LLH scan
  MaCh3Fitter->RunSigmaVarFD();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
