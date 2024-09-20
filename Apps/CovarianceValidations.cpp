// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "covariance/covarianceXsec.h"
#include "covariance/covarianceOsc.h"

/// @todo Add adaptive testing
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

////////////// Normal Xsec //////////////
  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 0.25, 1.70, 3.20, -1.10, -1.70};
  xsec->setParameters(ParProp);
  xsec->printNominalCurrProp();

  xsec->DumpMatrixToFile("xsec_2024a_flux_21bv2.root");

  // Open a file in write mode
  std::ofstream outFile("NewCovarianceOut.txt");
  outFile << "Likelihood Xsec=" << xsec->GetLikelihood() << std::endl;

  //Test throwing
  const int Ntoys = 100;
  MACH3LOG_INFO("Testing throwing from covariance");

  for (int i = 0; i < Ntoys; i++) {
    if (i % (Ntoys/10) == 0) {
      MaCh3Utils::PrintProgressBar(i, Ntoys);
    }
    xsec->throwParameters();
  }
  xsec->setParameters(ParProp);

////////////// Now PCA //////////////
  MACH3LOG_INFO("Testing PCA matrix");
  xsecCovMatrixFile = {"Inputs/PCATest.yaml"};
  covarianceXsec* PCA = new covarianceXsec(xsecCovMatrixFile, "xsec_cov", 0.001);

  std::vector<double>  EigenVal = PCA->getEigenValuesMaster();
  for(size_t i = 0; i < EigenVal.size(); i++) {
    outFile << "Eigen Value " << i << " = " << EigenVal[i] << std::endl;
  }

  TVectorD eigen_values = PCA->getEigenValues();
  double sum = 0;
  for(int i = 0; i < eigen_values.GetNrows(); i++){
    sum += eigen_values(i);
  }
  outFile << "Sum of Eigen Value: " << sum << std::endl;

////////////// Now Osc //////////////
  std::vector<std::string> OscCovMatrixFile = {"Inputs/Osc_Test.yaml"};
  covarianceOsc* osc = new covarianceOsc(OscCovMatrixFile, "osc_cov");
  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6};
  osc->setParameters(OscParProp);
  osc->printNominalCurrProp();
  outFile << "Likelihood Osc=" << osc->GetLikelihood() << std::endl;

  outFile.close();
  delete xsec;
  delete osc;
  delete PCA;

  bool TheSame = CompareTwoFiles("Inputs/CovarianceOut.txt", "NewCovarianceOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
