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

  // Open a file in write mode
  std::ofstream outFile("NewSampleOut.txt");
  std::vector<std::string> SampleConfig = {"Inputs/SamplePDF_Tutorial.yaml"};
  for (const auto& configPath : SampleConfig) {
    samplePDFTutorial *Sample = new samplePDFTutorial({configPath}, xsec);
    Sample->SetXsecCov(xsec);
    Sample->SetOscCov(osc);

    std::string name = Sample->GetName();
    TString NameTString = TString(name.c_str());

    // Reweight and process prior histogram
    Sample->reweight();
    TH1D *SampleHistogramPrior = (TH1D*)Sample->get1DHist()->Clone(NameTString + "_Prior");
    Sample->addData(SampleHistogramPrior);

    // Write initial info to file
    outFile << "Info for sample:" << NameTString << std::endl;
    outFile << "Rates Prior:" << SampleHistogramPrior->Integral() << std::endl;
    outFile << "Likelihood:" << Sample->GetLikelihood() << std::endl;

    // Set oscillation parameters and reweight for posterior
    std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6};
    osc->setParameters(OscParProp);
    Sample->reweight();

    // Process posterior histogram
    TH1D *SampleHistogramPost = (TH1D*)Sample->get1DHist()->Clone(NameTString + "_Post");
    outFile << "Rates Post:" << SampleHistogramPrior->Integral() << std::endl;
    outFile << "Likelihood:" << Sample->GetLikelihood() << std::endl;

    MACH3LOG_INFO("Now trying to compare each weight individually");
    for (int iSample = 0; iSample < Sample->getNMCSamples(); ++iSample) {
      for (int iEntry = 0; iEntry < Sample->getNEventsInSample(iSample); ++iEntry) {
        double weight = Sample->GetEventWeight(iSample, iEntry);
        outFile << "Channel: "<< iSample<<" Event: " << iEntry <<" weight: " << weight << std::endl;
      }
    }

    // Clean up dynamically allocated Sample if needed
    delete SampleHistogramPost;
    delete Sample;
  }
  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/SampleOut.txt", "NewSampleOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  delete xsec;
  delete osc;

  return 0;
}