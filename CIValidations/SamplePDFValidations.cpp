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


  MACH3LOG_INFO("Info for sample: {}", NameTString);
  MACH3LOG_INFO("Rates Prior: {:.2f}", SampleHistogramPrior->Integral());
  MACH3LOG_INFO("Likelihood: {:.2f}", Sample->GetLikelihood());

  std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6};
  osc->setParameters(OscParProp);
  Sample->reweight();
  TH1D *SampleHistogramPost = (TH1D*)Sample->get1DHist()->Clone(NameTString+"Post");

  MACH3LOG_INFO("Rates Prior: {:.2f}", SampleHistogramPost->Integral());
  MACH3LOG_INFO("Likelihood: {:.2f}", Sample->GetLikelihood());

  return 0;
}
