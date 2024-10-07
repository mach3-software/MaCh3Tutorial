// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"

int main(int argc, char *argv[]){

  MaCh3Utils::MaCh3Usage(argc, argv);
  // Initialise manger responsible for config handling
  manager *FitManager = new manager(argv[1]);

  std::vector<std::string> xsecCovMatrixFile = FitManager->raw()["General"]["Systematics"]["XsecCovFile"].as<std::vector<std::string>>();
  // Initialise covariance class reasonable for Systematics
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<std::string> OscMatrixFile = FitManager->raw()["General"]["Systematics"]["OscCovFile"].as<std::vector<std::string>>();
  covarianceOsc* osc = new covarianceOsc(OscMatrixFile, "osc_cov");

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = std::make_unique<mcmc>(FitManager);
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSystObj(osc);

  // Run MCMCM
  MaCh3Fitter->RunLLHScan();

  delete FitManager;
  delete xsec;
  MaCh3Fitter.reset();
  return 0;
}