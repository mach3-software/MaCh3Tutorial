// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<std::string> OscCovMatrixFile = {"Inputs/Osc_Test.yaml"};
  covarianceOsc* osc = new covarianceOsc(OscCovMatrixFile, "osc_cov");
  osc->setParameters();

  std::string SampleConfig = {"Inputs/SamplePDF_Tutorial.yaml"};
  samplePDFTutorial *Sample = new samplePDFTutorial(SampleConfig, xsec);
  Sample->SetXsecCov(xsec);
  Sample->SetOscCov(osc);

  std::string name = Sample->GetName();
  TString NameTString = TString(name.c_str());
  Sample->reweight();
  TH1D *SampleHistogramPrior = (TH1D*)Sample->get1DHist()->Clone(NameTString+"_Prior");
  Sample->addData(SampleHistogramPrior);

  // Open a file in write mode
  std::ofstream outFile("NewSampleOut.txt");
  outFile << "Info for sample:" << NameTString << std::endl;
  outFile << "Rates Prior:" << SampleHistogramPrior->Integral() << std::endl;
  outFile << "Likelihood:" << Sample->GetLikelihood() << std::endl;

  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6};
  osc->setParameters(OscParProp);
  Sample->reweight();
  TH1D *SampleHistogramPost = (TH1D*)Sample->get1DHist()->Clone(NameTString+"Post");
  outFile << "Rates Post:" << SampleHistogramPrior->Integral() << std::endl;
  outFile << "Likelihood:" << Sample->GetLikelihood() << std::endl;

  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/SampleOut.txt", "NewSampleOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
