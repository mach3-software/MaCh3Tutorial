// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "Parameters/ParameterHandlerGeneric.h"

void ValidateCholeskyDecomposition(std::ostream& outFile) {
  // Create a known symmetric positive definite matrix (3x3)
  TMatrixDSym original(5);
  original(0,0) =  4.0; original(0,1) = -1.0; original(0,2) =  0.0; original(0,3) =  0.5; original(0,4) = -0.2;
  original(1,0) = -1.0; original(1,1) =  5.0; original(1,2) = -2.0; original(1,3) =  1.0; original(1,4) =  0.3;
  original(2,0) =  0.0; original(2,1) = -2.0; original(2,2) =  6.0; original(2,3) = -1.5; original(2,4) =  0.8;
  original(3,0) =  0.5; original(3,1) =  1.0; original(3,2) = -1.5; original(3,3) =  5.0; original(3,4) = -0.5;
  original(4,0) = -0.2; original(4,1) =  0.3; original(4,2) =  0.8; original(4,3) = -0.5; original(4,4) =  4.0;


  // Get decomposed matrix
  auto decomposed = M3::GetCholeskyDecomposedMatrix(original, "testMatrix");

  // Print all elements
  for (Int_t i = 0; i < decomposed.size(); i++) {
    for (Int_t j = 0; j < decomposed[i].size(); j++) {
      outFile << "L(" << i << "," << j << ") = " << decomposed[i][j] << std::endl;
    }
  }
}

/// @brief This simply updates YAML file
void TuneValidations(std::ostream& outFile)
{
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
  YAML::Node Node = M3OpenConfig(TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml");
  std::vector<double> TuneValues = {1.05, 0.90, 1.10, 1.05, 1.05, 1.05, 1.05, 1.05, 0., 10};
  std::string Tune = "WackyTune";

  M3::AddTuneValues(Node, TuneValues, Tune);

  std::vector<double> TuneErrors =  {0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01};

  // Example correlation matrix (10x10 identity matrix as a placeholder)
  std::vector<std::vector<double>> CorrelationMatrix(10, std::vector<double>(10, 0.0));
  for (std::size_t i = 0; i < 10; ++i) {
    for (std::size_t j = 0; j < 10; ++j) {
      if (i == j) {
        CorrelationMatrix[i][j] = 1.0;
      } else {
        CorrelationMatrix[i][j] = 0.1;
      }
    }
  }

  M3::MakeCorrelationMatrix(Node, TuneValues, TuneErrors, CorrelationMatrix, "UpdatedCorrelationMatrix_Blarb.yaml");

  std::vector<std::string> FancyNames = {
    "Norm_Param_0",
    "Norm_Param_1",
    "Norm_Param_2",
    "BinnedSplineParam1",
    "BinnedSplineParam2",
    "BinnedSplineParam3",
    "BinnedSplineParam4",
    "BinnedSplineParam5",
    "EResLep",
    "EResTot"
  };
  M3::AddTuneValues(Node, TuneValues, Tune, FancyNames);
  M3::MakeCorrelationMatrix(Node, TuneValues, TuneErrors, CorrelationMatrix, "UpdatedCorrelationMatrix.yaml", FancyNames);

  std::vector<std::string> ParameterMatrixFile = {TutorialPath + "/UpdatedMatrixWithTuneWackyTune.yaml"};
  auto TuneTestx = std::make_unique<ParameterHandlerGeneric>(ParameterMatrixFile, "xsec_cov");

  //Now we check if Tune works
  TuneTestx->SetTune("WackyTune");

  for (int i = 0; i < TuneTestx->GetNumParams(); ++i) {
    outFile << "WackyTune for param " << i  << " is equal to=" << TuneTestx->GetParProp(i) << std::endl;
  }

  ParameterMatrixFile = {TutorialPath + "/UpdatedCorrelationMatrix.yaml"};
  auto CorrTest = std::make_unique<ParameterHandlerGeneric>(ParameterMatrixFile, "xsec_cov");

  for (int i = 0; i < CorrTest->GetNumParams(); ++i) {
    for (int j = 0; j < CorrTest->GetNumParams(); ++j) {
      outFile << "Yaml loaded Inv Cov Matrix for " << i << " and " << j << " is equal to=" << CorrTest->GetInvCovMatrix(i, j) << std::endl;
    }
  }
}


void TestPCA(const std::string& label,
             const std::vector<std::string>& matrixFiles,
             double threshold,
             int firstIndex,
             int lastIndex,
             int Ntoys,
             std::ostream& outFile)
{
  MACH3LOG_INFO("Testing PCA matrix {}", label);
  std::unique_ptr<ParameterHandlerGeneric> PCA = nullptr;
  if(firstIndex == -999 && lastIndex == -999){
    PCA = std::make_unique<ParameterHandlerGeneric>(matrixFiles, "xsec_cov", threshold);
  } else{
    PCA = std::make_unique<ParameterHandlerGeneric>(matrixFiles, "xsec_cov", threshold, firstIndex, lastIndex);
  }

  std::vector<double> EigenVal = PCA->GetPCAHandler()->GetEigenValuesMaster();
  for (size_t i = 0; i < EigenVal.size(); i++) {
    outFile << label << " Eigen Value " << i << " = " << EigenVal[i] << std::endl;
  }

  TVectorD eigen_values = PCA->GetPCAHandler()->GetEigenValues();
  double sum = 0;
  for (int i = 0; i < eigen_values.GetNrows(); i++) {
    sum += eigen_values(i);
  }
  outFile << label << " Sum of Eigen Value: " << sum << std::endl;

  outFile << "Num of " << label << " PCA pars: " << PCA->GetNParameters() << std::endl;
  for (int i = 0; i < PCA->GetNParameters(); i++) {
    outFile << "Param in " << label << " PCA base: " << i << " = " << PCA->GetPCAHandler()->GetParCurrPCA(i) << std::endl;
  }

  TMatrixD EigenVectors = PCA->GetPCAHandler()->GetEigenVectors();
  for (int i = 0; i < EigenVectors.GetNrows(); ++i) {
    for (int j = 0; j < EigenVectors.GetNcols(); ++j) {
      outFile << label << " Eigen Vectors: " << i << ", " << j << " = " << EigenVectors(i, j) << std::endl;
    }
  }

  TMatrixD TransferMatrix = PCA->GetPCAHandler()->GetTransferMatrix();
  for (int i = 0; i < TransferMatrix.GetNrows(); ++i) {
    for (int j = 0; j < TransferMatrix.GetNcols(); ++j) {
      outFile << label << " Transfer Matrix: " << i << ", " << j << " = " << TransferMatrix(i, j) << std::endl;
    }
  }

  for (int i = 0; i < PCA->GetPCAHandler()->GetNumberPCAedParameters(); i++) {
    outFile << "Param in " << label << " Is Decomposed: " << i << " = " << PCA->GetPCAHandler()->IsParameterDecomposed(i) << std::endl;
  }

  for (int i = 0; i < PCA->GetPCAHandler()->GetNumberPCAedParameters(); i++) {
    outFile << "Param in " << label << " Prefit Value: " << i << " = " << PCA->GetPCAHandler()->GetPreFitValuePCA(i) << std::endl;
  }

  for (int i = 0; i < PCA->GetPCAHandler()->GetNumberPCAedParameters(); i++) {
    outFile << "Param in " << label << " is Fixed: " << i << " = " << PCA->GetPCAHandler()->IsParameterFixedPCA(i) << std::endl;
  }

  MACH3LOG_INFO("Testing throwing from covariance for {}", label);
  for (int i = 0; i < Ntoys; i++) {
    if (i % (Ntoys / 10) == 0) {
      MaCh3Utils::PrintProgressBar(i, Ntoys);
    }
    PCA->ThrowParameters();
  }

  PCA->AcceptStep();

  PCA->ToggleFixAllParameters();
  for (int i = 0; i < PCA->GetPCAHandler()->GetNumberPCAedParameters(); i++) {
    outFile << "Param in " << label << " is Fixed: " << i << " = " << PCA->GetPCAHandler()->IsParameterFixedPCA(i) << std::endl;
  }
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
////////////// Normal Xsec //////////////
  std::vector<std::string> ParameterMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml"};
  auto xsec = std::make_unique<ParameterHandlerGeneric>(ParameterMatrixFile, "xsec_cov");

  std::vector<double> ParProp = {1.05, 0.90, 1.10, 1.05, 1.05, 1.05, 1.05, 1.05, 0., 0.2};
  xsec->SetParameters(ParProp);
  xsec->PrintNominalCurrProp();

  xsec->DumpMatrixToFile("xsec_2024a_flux_21bv2.root");

  // Open a file in write mode
  std::ofstream outFile("NewCovarianceOut.txt");
  outFile << "Likelihood Xsec=" << xsec->GetLikelihood() << std::endl;

  ///// Test throwing /////
  constexpr int Ntoys = 100;
  MACH3LOG_INFO("Testing throwing from covariance");

  for (int i = 0; i < Ntoys; i++) {
    if (i % (Ntoys/10) == 0) {
      MaCh3Utils::PrintProgressBar(i, Ntoys);
    }
    xsec->ThrowParameters();
  }
  xsec->SetParameters(ParProp);
  xsec->AcceptStep();
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
    outFile << "Proposed Step for param " << i  << " is equal to=" << xsec->GetParProp(i) << std::endl;
  }

  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "Diagonal error for param " << i  << " is equal to=" << xsec->GetDiagonalError(i) << std::endl;
  }

  //Now we check if Tune works
  xsec->SetTune("PostND");
  for (int i = 0; i < xsec->GetNumParams(); ++i) {
    outFile << "PostND Tune for param " << i  << " is equal to=" << xsec->GetParProp(i) << std::endl;
  }

  std::vector<std::string> paramNames = {"Norm_Param_2", "BinnedSplineParam3", "EResLep"};
  for (const auto& name : paramNames) {
    outFile << "Index for param " << name << " is " << xsec->GetParIndex(name) << std::endl;
  }

  std::vector<std::string> groupNames = {"Xsec", "Flux"};
  for (const auto& group : groupNames) {
    outFile << "Number of parameters in group " << group << " is " << xsec->GetNumParFromGroup(group) << std::endl;
  }

  xsec->SetGroupOnlyParameters("Xsec");
  xsec->SetGroupOnlyParameters(std::vector<std::string>{ "Xsec", "Flux" });

  // Now check fixing
  for (int i = 0; i < xsec->GetNumParams(); i++) {
    outFile << "Is param " << i  << " fixed=" << xsec->IsParameterFixed(i) << std::endl;
  }
  xsec->ToggleFixAllParameters();
  for (int i = 0; i < xsec->GetNumParams(); i++) {
    outFile << "Is param " << i  << " fixed=" << xsec->IsParameterFixed(i) << std::endl;
  }

////////////// Now PCA //////////////
  TestPCA("PCA",
          {TutorialPath + "/TutorialConfigs/CovObjs/PCATest.yaml"},
          0.001,
          -999, -999,
          Ntoys,
          outFile);
////////////// Now PCA with several undecomposed //////////////
  TestPCA("PCA_2",
          {TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml",
            TutorialPath + "/TutorialConfigs/CovObjs/PCATest.yaml"},
          0.01,
          10, 19,
          Ntoys,
          outFile);
////////////// Now Osc //////////////
  std::vector<std::string> OscCovMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/OscillationModel.yaml"};
  auto osc = std::make_unique<ParameterHandlerGeneric>(OscCovMatrixFile, "osc_cov");
  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
  osc->SetParameters(OscParProp);
  osc->PrintNominalCurrProp();
  outFile << "Likelihood Osc=" << osc->GetLikelihood() << std::endl;

////////////// Now Adaptive //////////////

// KS: To lazy to write yaml so let's make string and convert it'
std::string yamlContent = R"(
AdaptionOptions:
  Settings:
    # When do we start throwing from our adaptive matrix?
    StartThrow: 4000
    # When do we start putting steps into our adaptive covariance?
    StartUpdate: 1000
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
  std::vector<std::string> AdaptiveCovMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml",
                                                    TutorialPath + "/TutorialConfigs/CovObjs/PCATest.yaml"};

  MACH3LOG_INFO("Creating adaptive covariance handler");
  auto Adapt = std::make_unique<ParameterHandlerGeneric>(AdaptiveCovMatrixFile, "xsec_cov");
  //KS: Let's make Doctor Wallace proud
  Adapt->InitialiseAdaption(AdaptSetting);
  MACH3LOG_INFO("Adaption initialised");

  std::vector<double> ParAdapt = {1.05, 0.90, 1.10, 1.05, 1.05, 1.05, 1.05, 1.05, 0., 0.2, -0.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  Adapt->SetParameters(ParAdapt);
  bool increase = true;
  for(int i = 0; i < 50000; ++i ) {
    
    // Determine the direction of adjustment
    if (i % 20 == 10) { // Switch direction every 10 iterations
      increase = !increase;
    }
    // Adjust parameters
    for (size_t i = 0; i < ParAdapt.size(); i++) {
      ParAdapt[i] += (increase ? 0.01 : -0.01)*i;
    }
    Adapt->SetParameters(ParAdapt);
    Adapt->UpdateAdaptiveCovariance();
    Adapt->AcceptStep();
  }
  auto ParMeans = Adapt->GetAdaptiveHandler()->GetParameterMeans();
  for(size_t i = 0; i < ParMeans.size(); i++) {
    outFile << "Adapt, Param means: " << i << " = " << ParMeans[i] << std::endl;
  }
  TMatrixDSym* Matrix = Adapt->GetAdaptiveHandler()->GetAdaptiveCovariance();
  double adapt_scale = Adapt->GetAdaptiveHandler()->GetAdaptionScale();
  outFile << "Adapt scale: " << adapt_scale << std::endl;
  int dim = Matrix->GetNrows();
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      outFile << "Adapt matrix: " << i << ", " << j << " = " << (*Matrix)(i, j)*adapt_scale << std::endl;
    }
  }
  outFile << "Total Number Of Steps " << Adapt->GetAdaptiveHandler()->GetTotalSteps() << std::endl;
  outFile << "Total Number Of AMCMC Params " << Adapt->GetAdaptiveHandler()->GetNumParams() << std::endl;

  Adapt->SaveAdaptiveToFile("Wacky.root", "xsec");

  // Now check fixing
  for (int i = 0; i < Adapt->GetNumParams(); i++) {
    outFile << "Adapt is param " << i  << " fixed=" << Adapt->IsParameterFixed(i) << std::endl;
  }
  Adapt->ToggleFixAllParameters();
  for (int i = 0; i < Adapt->GetNumParams(); i++) {
    outFile << "Adapt is param " << i  << " fixed=" << Adapt->IsParameterFixed(i) << std::endl;
  }

////////////// Now Tune //////////////
  TuneValidations(outFile);
////////////// Other tests //////////////
  ValidateCholeskyDecomposition(outFile);

////////////// The End //////////////
  outFile.close();
  bool TheSame = CompareTwoFiles(TutorialPath + + "/CIValidations/TestOutputs/CovarianceOut.txt", "NewCovarianceOut.txt");


  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}

