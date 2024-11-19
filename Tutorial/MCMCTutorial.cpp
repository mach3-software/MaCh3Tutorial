// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[]) {

  MaCh3Utils::MaCh3Usage(argc, argv);
  // Initialise manger responsible for config handling
  manager *FitManager = new manager(argv[1]);

  // Initialise covariance class reasonable for Systematics
  covarianceXsec* xsec = MaCh3CovarianceFactory(FitManager, "Xsec");
  covarianceOsc*  osc  = MaCh3CovarianceFactory<covarianceOsc>(FitManager, "Osc");

  // Initialise samplePDF
  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SamplePDFFactory<samplePDFTutorial>(SampleConfig, xsec, osc);

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager);
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSystObj(osc);
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->addSamplePDF(mySamples[i]);
  }
  // Run MCMCM
  MaCh3Fitter->runMCMC();

  delete FitManager;
  delete xsec;
  delete osc;
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
