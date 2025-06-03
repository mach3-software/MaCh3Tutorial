// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

_MaCh3_Safe_Include_Start_ //{
#include "Oscillator/OscillatorFactory.h"
_MaCh3_Safe_Include_End_ //}

void SharedNuOscTest(const std::string& config, ParameterHandlerGeneric* xsec){
  MACH3LOG_INFO("Utilising a shared NuOscillator object between all atmospheric samples");

  std::string OscillatorConfig = std::string(std::getenv("MaCh3Tutorial_ROOT")) + "/TutorialConfigs/NuOscillator/CUDAProb3.yaml";
  std::vector<const double*> OscParams = xsec->GetOscParsFromSampleName("Tutorial ATM");
  auto OscillatorObj = std::make_shared<OscillationHandler>(OscillatorConfig, true, OscParams, 6);

  SampleHandlerTutorial *Sample1 = new SampleHandlerTutorial(config, xsec, OscillatorObj);
  SampleHandlerTutorial *Sample2 = new SampleHandlerTutorial(config, xsec, OscillatorObj);

  delete Sample1;
  delete Sample2;
}

void NoSplinesNoOscTest(const std::string& config){
  MACH3LOG_INFO("Utilising a shared NuOscillator object between all atmospheric samples");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
  std::vector<std::string> ParameterMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/PCATest.yaml"};
  auto xsec = std::make_unique<ParameterHandlerGeneric>(ParameterMatrixFile, "xsec_cov");
  auto Sample = std::make_unique<SampleHandlerTutorial>(config, xsec.get());
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::vector<std::string> xsecCovMatrixFile = {"TutorialConfigs/CovObjs/SystematicModel.yaml", "TutorialConfigs/CovObjs/OscillationModel.yaml"};
  ParameterHandlerGeneric* xsec = new ParameterHandlerGeneric(xsecCovMatrixFile, "xsec_cov");
  xsec->SetParameters();

  // Open a file in write mode
  std::ofstream outFile("NewSampleOut.txt");
  std::vector<std::string> SampleConfig = {"TutorialConfigs/Samples/SampleHandler_Tutorial.yaml", "TutorialConfigs/Samples/SampleHandler_Tutorial_ATM.yaml"};
  for (const auto& configPath : SampleConfig) {
    SampleHandlerTutorial *Sample = new SampleHandlerTutorial({configPath}, xsec);

    std::string name = Sample->GetTitle();
    TString NameTString = TString(name.c_str());

    // Reweight and process prior histogram
    Sample->Reweight();
    TH1D *SampleHistogramPrior = (TH1D*)Sample->Get1DHist()->Clone(NameTString + "_Prior");
    Sample->AddData(SampleHistogramPrior);

    // Write initial info to file
    outFile << "Info for sample: " << NameTString << std::endl;
    outFile << "Rates Prior: " << SampleHistogramPrior->Integral() << std::endl;
    outFile << "Likelihood: " << std::fabs(Sample->GetLikelihood()) << std::endl;

    // Set oscillation parameters and reweight for posterior
    std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
    xsec->SetGroupOnlyParameters("Osc", OscParProp);
    Sample->Reweight();

    // Process posterior histogram
    TH1D *SampleHistogramPost = (TH1D*)Sample->Get1DHist()->Clone(NameTString + "_Post");
    outFile << "Rates Post:" << SampleHistogramPrior->Integral() << std::endl;
    outFile << "Likelihood:" << std::fabs(Sample->GetLikelihood()) << std::endl;

    MACH3LOG_INFO("Now trying to compare each weight individually");
    for (int iEntry = 0; iEntry < Sample->GetNEvents(); ++iEntry) {
      double weight = Sample->GetEventWeight(iEntry);
      outFile<< "Sample: "<< NameTString << " Event: " << iEntry <<" weight: " << weight << std::endl;
    }

    for (int TestStat = 0; TestStat < TestStatistic::kNTestStatistics; ++TestStat) {
      Sample->SetTestStatistic(TestStatistic(TestStat));
      outFile<< "Sample: "<< NameTString << " LLH is: " << std::fixed << std::setprecision(6) << std::fabs(Sample->GetLikelihood()) <<" using: " << TestStatistic_ToString(TestStatistic(TestStat)) <<" test stat"<< std::endl;
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

  SharedNuOscTest(SampleConfig.back(), xsec);
  NoSplinesNoOscTest(SampleConfig.back());

  delete xsec;

  return 0;
}
