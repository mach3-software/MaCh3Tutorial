// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "covariance/covarianceXsec.h"
#include "covariance/covarianceOsc.h"


/// Current tests include
/// testing LLH for cov xsc and osc
/// checking PCA initialisation
/// checking Adaptive initialisation
/// checking if throwing works
/// checking if dumping matrix to root file works
/// checking if DetID operation return same number of params
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

////////////// Normal Xsec //////////////
  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicModel.yaml"};
  auto xsec = std::make_unique<covarianceXsec>(xsecCovMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 0.25, 1.70, 3.20, -1.10, -1.70};
  xsec->setParameters(ParProp);
  xsec->printNominalCurrProp();

  xsec->DumpMatrixToFile("xsec_2024a_flux_21bv2.root");

  // Open a file in write mode
  std::ofstream outFile("NewCovarianceOut.txt");
  outFile << "Likelihood Xsec=" << xsec->GetLikelihood() << std::endl;

  ///// Test throwing /////
  const int Ntoys = 100;
  MACH3LOG_INFO("Testing throwing from covariance");

  for (int i = 0; i < Ntoys; i++) {
    if (i % (Ntoys/10) == 0) {
      MaCh3Utils::PrintProgressBar(i, Ntoys);
    }
    xsec->throwParameters();
  }
  xsec->setParameters(ParProp);
  ///// Test Params from DetId /////
  const int Det_Id = 1;
  for (int s = 0; s < kSystTypes; ++s)
  {
    int NParams = xsec->GetNumParamsFromDetID(Det_Id, static_cast<SystType>(s));
    outFile << "Found " << NParams << " Params of type " << SystType_ToString(static_cast<SystType>(s)) << std::endl;

    // These return vector and I am too lazy to make check so here let just run them to so if things don't break
    xsec->GetSystIndexFromDetID(Det_Id, static_cast<SystType>(s));
    xsec->GetParsIndexFromDetID(Det_Id, static_cast<SystType>(s));
    xsec->GetParsNamesFromDetID(Det_Id, static_cast<SystType>(s));
  }

////////////// Now PCA //////////////
  MACH3LOG_INFO("Testing PCA matrix");
  xsecCovMatrixFile = {"Inputs/PCATest.yaml"};
  auto PCA = std::make_shared<covarianceXsec>(xsecCovMatrixFile, "xsec_cov", 0.001);
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
  auto osc = std::make_unique<covarianceOsc>(OscCovMatrixFile, "osc_cov");
  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5};
  osc->setParameters(OscParProp);
  osc->printNominalCurrProp();
  outFile << "Likelihood Osc=" << osc->GetLikelihood() << std::endl;

////////////// Now Adaptive //////////////

// KS: To lazy to write yaml so let's make string and convert it'
std::string yamlContent = R"(
AdaptionOptions:
  Settings:
    # When do we start throwing from our adaptive matrix?
    AdaptionStartThrow: 10
    # When do we start putting steps into our adaptive covariance?
    AdaptionStartUpdate: 0
    # When do we end updating our covariance?
    AdaptionEndUpdate: 50000
    # How often do we change our matrix throws?
    AdaptionUpdateStep: 1000
  Covariance:
    # So now we list individual matrices, let's just do xsec
    xsec_cov:
      # Do we want to adapt this matrix?
      DoAdaption: true
      MatrixBlocks: [[]]
      # External Settings
      UseExternalMatrix: false
      ExternalMatrixFileName: ""
      ExternalMatrixName: ""
      ExternalMeansName: ""
)";

  // Convert the string to a YAML node
  YAML::Node AdaptSetting = STRINGtoYAML(yamlContent);
  std::vector<std::string> AdaptiveCovMatrixFile = {"Inputs/SystematicModel.yaml", "Inputs/PCATest.yaml"};
  auto Adapt = std::make_unique<covarianceXsec>(AdaptiveCovMatrixFile, "xsec_cov");
  //KS: Let's make Doctor Wallace proud
  Adapt->initialiseAdaption(AdaptSetting);

  std::vector<double> ParAdapt = {1.05, 0.90, 1.10, 1.05, 0.25, 1.70, 3.20, -1.10, -1.70, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
  Adapt->setParameters(ParAdapt);
  for(int i = 0; i < 50000; ++i ) {
    Adapt->acceptStep();
  }
  Adapt->saveAdaptiveToFile("Wacky.root", "xsec");

  outFile.close();
  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/CovarianceOut.txt", "NewCovarianceOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
