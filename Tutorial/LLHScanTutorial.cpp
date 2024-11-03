// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[]){

  MaCh3Utils::MaCh3Usage(argc, argv);
  // Initialise manger responsible for config handling
  manager *FitManager = new manager(argv[1]);

  // Initialise covariance class reasonable for Systematics
  covarianceXsec* xsec = MaCh3CovarianceFactory(FitManager, "Xsec");

  std::vector<std::string> OscMatrixFile = FitManager->raw()["General"]["Systematics"]["OscCovFile"].as<std::vector<std::string>>();
  covarianceOsc* osc = new covarianceOsc(OscMatrixFile, "osc_cov");

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = std::make_unique<mcmc>(FitManager);
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSystObj(osc);

  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SamplePDFFactory<samplePDFTutorial>(SampleConfig, xsec, osc);
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->addSamplePDF(mySamples[i]);
  }

  // Run MCMCM
  MaCh3Fitter->RunLLHScan();

  delete FitManager;
  delete xsec;
  MaCh3Fitter.reset();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
