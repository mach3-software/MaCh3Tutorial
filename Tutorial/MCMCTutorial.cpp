// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

int main(int argc, char *argv[]) {
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);

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
  // Run MCMCM
  MaCh3Fitter->RunMCMC();

  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
