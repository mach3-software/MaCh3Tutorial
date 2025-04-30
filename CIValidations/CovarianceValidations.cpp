// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "covariance/covarianceXsec.h"
#include "covariance/covarianceOsc.h"


int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

////////////// Normal Xsec //////////////
  std::vector<std::string> xsecCovMatrixFile = {"TutorialConfigs/CovObjs/SystematicModel.yaml"};
  auto xsec = std::make_unique<covarianceXsec>(xsecCovMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 1.05, 1.05, 1.05, 1.05, 0., 0.2};
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
  xsec->acceptStep();
  ///// Test Params from DetId /////
  const std::vector<std::string> AffectedSamples = {"Tutorial Beam", "Tutorial ATM", "tutorial beam", "blarb" "ATM"};
  for (size_t id = 0; id < AffectedSamples.size(); ++id)
  {
    for (int s = 0; s < kSystTypes; ++s)
    {
      int NParams = xsec->GetNumParamsFromSampleName(AffectedSamples[id], static_cast<SystType>(s));
      outFile << "Found " << NParams << " for "<< AffectedSamples[id] << " Params of type " << SystType_ToString(static_cast<SystType>(s)) << std::endl;

      // These return vector and I am too lazy to make check so here let just run them to so if things don't break
      xsec->GetSystIndexFromSampleName(AffectedSamples[id], static_cast<SystType>(s));
      xsec->GetParsIndexFromSampleName(AffectedSamples[id], static_cast<SystType>(s));
      xsec->GetParsNamesFromSampleName(AffectedSamples[id], static_cast<SystType>(s));
    }

    auto Norm = xsec->GetNormParsFromSampleName(AffectedSamples[id]);
    outFile << "Found " << Norm.size()   << " for "<< AffectedSamples[id] << " From GetNormParsFromSampleName" << std::endl;
  }
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    for (int j = 0; j < xsec->GetNumParams(); ++j) {
      outFile << "Inv Cov Matrix for " << i << " and " << j << " is equal to=" << xsec->GetInvCovMatrix(i, j) << std::endl;
    }
  }
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    for (int j = 0; j < xsec->GetNumParams(); ++j) {
      outFile << "Throw Matrix for " << i << " and " << j << " is equal to=" << xsec->GetThrowMatrix(i, j) << std::endl;
    }
  }

  // KS: Set random to ensure we can reproduce
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    xsec->SetRandomThrow(i, 0 + i*0.001);
  }
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "Random number for " << i  << " is equal to=" << xsec->GetRandomThrow(i) << std::endl;
  }
  xsec->CorrelateSteps();
  // Make sure throw matrix works
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "Corr Throws for " << i  << " is equal to=" << xsec->GetCorrThrows(i) << std::endl;
  }
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "Indv Step Scale for param " << i  << " is equal to=" << xsec->GetIndivStepScale(i) << std::endl;
  }
  outFile << "Global Step Scale is equal to=" << xsec->GetGlobalStepScale() << std::endl;

  // Make sure we can reproduce parameter proposal
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "Proposed Step for param " << i  << " is equal to=" << xsec->getParProp(i) << std::endl;
  }

////////////// Now PCA //////////////
  MACH3LOG_INFO("Testing PCA matrix");
  xsecCovMatrixFile = {"TutorialConfigs/CovObjs/PCATest.yaml"};
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

  outFile << "Num of PCA pars: " << PCA->getNpars() << std::endl;
  for(int i = 0; i < PCA->getNpars(); i++){
    outFile << "Param in PCA base: " << i << " = " << PCA->getParCurr_PCA(i) << std::endl;
  }

////////////// Now Osc //////////////
  std::vector<std::string> OscCovMatrixFile = {"TutorialConfigs/CovObjs/OscillationModel.yaml"};
  auto osc = std::make_unique<covarianceOsc>(OscCovMatrixFile, "osc_cov");
  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
  osc->setParameters(OscParProp);
  osc->printNominalCurrProp();
  outFile << "Likelihood Osc=" << osc->GetLikelihood() << std::endl;

////////////// Now Adaptive //////////////

// KS: To lazy to write yaml so let's make string and convert it'
std::string yamlContent = R"(
AdaptionOptions:
  Settings:
    # When do we start throwing from our adaptive matrix?
    StartThrow: 10
    # When do we start putting steps into our adaptive covariance?
    StartUpdate: 0
    # When do we end updating our covariance?
    EndUpdate: 50000
    # How often do we change our matrix throws?
    UpdateStep: 1000
    # Where shall we write the adapted matrices to?
    OutputFileName: "test_adaptive_output.root"
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
  std::vector<std::string> AdaptiveCovMatrixFile = {"TutorialConfigs/CovObjs/SystematicModel.yaml", "TutorialConfigs/CovObjs/PCATest.yaml"};
  auto Adapt = std::make_unique<covarianceXsec>(AdaptiveCovMatrixFile, "xsec_cov");
  //KS: Let's make Doctor Wallace proud
  Adapt->initialiseAdaption(AdaptSetting);

  std::vector<double> ParAdapt = {1.05, 0.90, 1.10, 1.05, 1.05, 1.05, 1.05, 1.05, 0., 0.2, -0.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  Adapt->setParameters(ParAdapt);
  bool increase = true;
  for(int i = 0; i < 50000; ++i ) {

    // Determine the direction of adjustment
    if (i % 20 == 10) { // Switch direction every 10 iterations
      increase = !increase;
    }
    // Adjust parameters
    for (double& param : ParAdapt) {
      param += (increase ? 0.001 : -0.001);
    }
    Adapt->setParameters(ParAdapt);
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
