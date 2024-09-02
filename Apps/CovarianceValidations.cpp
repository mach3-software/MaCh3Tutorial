// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "covariance/covarianceXsec.h"

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 1.25, 1.70, 3.20, -1.10, -1.70};
  xsec->setParameters(ParProp);
  xsec->printNominalCurrProp();

  // Open a file in write mode
  std::ofstream outFile("NewCovarianceOut.txt");
  outFile << "Likelihood " << xsec->GetLikelihood() << std::endl;

  outFile.close();
  delete xsec;

  bool TheSame = CompareTwoFiles("Inputs/CovarianceOut.txt", "NewCovarianceOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
