// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "covariance/covarianceXsec.h"
#include "covariance/covarianceOsc.h"

// TODO Maybe add some proposal tests
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 0.25, 1.70, 3.20, -1.10, -1.70};
  xsec->setParameters(ParProp);
  xsec->printNominalCurrProp();

  // Open a file in write mode
  std::ofstream outFile("NewCovarianceOut.txt");
  outFile << "Likelihood Xsec=" << xsec->GetLikelihood() << std::endl;

  std::vector<std::string> OscCovMatrixFile = {"Inputs/Osc_Test.yaml"};
  covarianceOsc* osc = new covarianceOsc(OscCovMatrixFile, "osc_cov");
  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6};
  osc->setParameters(OscParProp);
  osc->printNominalCurrProp();
  outFile << "Likelihood Osc=" << osc->GetLikelihood() << std::endl;

  outFile.close();
  delete xsec;
  delete osc;

  bool TheSame = CompareTwoFiles("Inputs/CovarianceOut.txt", "NewCovarianceOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
