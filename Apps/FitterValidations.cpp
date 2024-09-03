// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "Utils/Comparison.h"

// TODO add also Adaption and PCA matrix
// TODO add maybe different algorithms?
int main(int argc, char *argv[])
{
  std::string ManagerInput = "Inputs/ManagerTest.yaml";
  manager *FitManager = new manager(ManagerInput);

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  mcmc *markovChain = new mcmc(FitManager);

  markovChain->addSystObj(xsec);
  markovChain->runMCMC();

  delete FitManager;
  delete xsec;
  delete markovChain;
  return 0;
}
