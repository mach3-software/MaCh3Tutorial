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

  auto Sample1 = std::make_unique<SampleHandlerTutorial>(config, xsec, OscillatorObj);
  auto Sample2 = std::make_unique<SampleHandlerTutorial>(config, xsec, OscillatorObj);
}

void NoSplinesNoOscTest(const std::string& config){
  MACH3LOG_INFO("Utilising a shared NuOscillator object between all atmospheric samples");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
  std::vector<std::string> ParameterMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/PCATest.yaml"};
  auto xsec = std::make_unique<ParameterHandlerGeneric>(ParameterMatrixFile, "xsec_cov");
  auto Sample = std::make_unique<SampleHandlerTutorial>(config, xsec.get());
}

void SampleLLHValidation(std::ostream& outFile, const std::string& OriginalSample, ParameterHandlerGeneric* xsec, const bool W2) {
  std::string NameTString = OriginalSample;
  xsec->SetParameters();

  // Load YAML config from file
  YAML::Node config = M3OpenConfig(NameTString);

  // Set UpdateW2 based on input flag
  config["LikelihoodOptions"]["UpdateW2"] = W2;

  // Convert to string
  std::string configStr = YAMLtoSTRING(config);

  // Get MaCh3Tutorial_ROOT environment variable
  const char* rootEnv = std::getenv("MaCh3Tutorial_ROOT");
  if (!rootEnv) {
    throw std::runtime_error("Environment variable MaCh3Tutorial_ROOT is not set.");
  }

  std::string tempConfigPath = std::string(rootEnv) + "/mach3_temp_config.yaml";

  // Write config string to file
  std::ofstream configOut(tempConfigPath);
  configOut << configStr;
  configOut.close();

  // Use modified config
  auto Sample = std::make_unique<SampleHandlerTutorial>(tempConfigPath, xsec);
  Sample->Reweight();

  for(int iSample = 0; iSample < Sample->GetNsamples(); iSample++){
    TH1D* SampleHistogramPrior = static_cast<TH1D*>(Sample->GetMCHist(iSample, 1)->Clone((NameTString + "_Prior").c_str()));
    Sample->AddData(iSample, SampleHistogramPrior);

    // Set oscillation parameters and reweight for posterior
    std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
    xsec->SetGroupOnlyParameters("Osc", OscParProp);
    Sample->Reweight();

    for (int TestStat = 0; TestStat < TestStatistic::kNTestStatistics; ++TestStat) {
      Sample->SetTestStatistic(TestStatistic(TestStat));
      outFile << "Sample: " << Sample->GetSampleTitle(iSample)
      << " update w2: " << std::boolalpha << W2
      << " LLH is: " << std::fixed << std::setprecision(6)
      << std::fabs(Sample->GetSampleLikelihood(iSample))
      << " using: " << TestStatistic_ToString(TestStatistic(TestStat)) << " test stat"
      << std::endl;
    }

    Sample->PrintIntegral(iSample);
  }


  std::remove(tempConfigPath.c_str());
}

void ValidateTestStatistic(std::ostream& outFile, const std::string& OriginalSample, ParameterHandlerGeneric* xsec) {
  // Use modified config
  auto Sample = std::make_unique<SampleHandlerTutorial>(OriginalSample, xsec);
  Sample->Reweight();
  for(int iSample = 0; iSample < Sample->GetNsamples(); iSample++){
    TH1D* SampleHistogramPrior = static_cast<TH1D*>(Sample->GetMCHist(iSample, 1)->Clone("Blarb_Prior"));
    Sample->AddData(iSample, SampleHistogramPrior);
  }

  // Define test vectors, feel free to expand it
  std::vector<double> data_values = {0.0, M3::_LOW_MC_BOUND_, 0.5, 1.0, 100000};
  std::vector<double> mc_values   = {0.0, M3::_LOW_MC_BOUND_, 0.5, 1.0, 100000};
  std::vector<double> w2_values   = {0.0, M3::_LOW_MC_BOUND_, 0.5, 1.0, 100000};

  // Loop over all test statistics
  for (int TestStat = 0; TestStat < TestStatistic::kNTestStatistics; ++TestStat) {
    Sample->SetTestStatistic(TestStatistic(TestStat));
    // Loop over all combinations of data, mc, w2
    for (double data : data_values) {
      for (double mc : mc_values) {
        for (double w2 : w2_values) {
          const double llh = Sample->GetTestStatLLH(data, mc, w2);
          std::ostringstream line;
          line << std::fixed << std::setprecision(6)
          << "TestStatistic " << TestStatistic_ToString(TestStatistic(TestStat))
          << ", Data: " << data
          << ", MC: " << mc
          << ", w²: " << w2
          << ", LLH: " << llh;
          outFile << line.str() << "\n";
        } // end loop over w2
      } // end loop over mc
    } // end loop over data
  } // end loop over test stat
}

void LoadSplineValidation(std::ostream& outFile, const std::string& OriginalSample, ParameterHandlerGeneric* xsec, bool LoadFile, bool MakeSFile) {
  std::string NameTString = OriginalSample;
  xsec->SetParameters();

  // Load YAML config from file
  YAML::Node config = M3OpenConfig(NameTString);

  // Set UpdateW2 based on input flag
  config["InputFiles"]["LoadSplineFile"] = LoadFile;
  config["InputFiles"]["PrepSplineFile"] = MakeSFile;

  // Convert to string
  std::string configStr = YAMLtoSTRING(config);

  // Get MaCh3Tutorial_ROOT environment variable
  const char* rootEnv = std::getenv("MaCh3Tutorial_ROOT");
  if (!rootEnv) {
    throw std::runtime_error("Environment variable MaCh3Tutorial_ROOT is not set.");
  }

  std::string tempConfigPath = std::string(rootEnv) + "/mach3_temp_config.yaml";

  // Write config string to file
  std::ofstream configOut(tempConfigPath);
  configOut << configStr;
  configOut.close();

  // Use modified config
  auto Sample = std::make_unique<SampleHandlerTutorial>(tempConfigPath, xsec);
  Sample->Reweight();

  for(int iSample = 0; iSample < Sample->GetNsamples(); iSample++){
    TH1D* SampleHistogramPrior = static_cast<TH1D*>(Sample->GetMCHist(iSample, 1)->Clone((NameTString + "_Prior").c_str()));
    Sample->AddData(iSample, SampleHistogramPrior);

    // Set oscillation parameters and reweight for posterior
    std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
    xsec->SetGroupOnlyParameters("Osc", OscParProp);
    Sample->Reweight();

    outFile << "Info for sample: " << NameTString << std::endl;
    outFile << "Rates Prior: " << SampleHistogramPrior->Integral() << std::endl;
    outFile << "Likelihood: " << std::fabs(Sample->GetLikelihood()) << std::endl;
  }

  std::remove(tempConfigPath.c_str());
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

    for(int iSample = 0; iSample < Sample->GetNsamples(); iSample++){
      std::string name = Sample->GetSampleTitle(iSample);
      TString NameTString = TString(name.c_str());

      // Reweight and process prior histogram
      Sample->Reweight();
      TH1D *SampleHistogramPrior = (TH1D*)Sample->GetMCHist(iSample, 1)->Clone(NameTString + "_Prior");
      Sample->AddData(iSample, SampleHistogramPrior);

      // Write initial info to file
      outFile << "Info for sample: " << NameTString << std::endl;
      outFile << "Rates Prior: " << SampleHistogramPrior->Integral() << std::endl;
      outFile << "Likelihood: " << std::fabs(Sample->GetSampleLikelihood(iSample)) << std::endl;

      // Set oscillation parameters and reweight for posterior
      std::vector<double> OscParProp = {0.3, 0.5, 0.020, 7.53e-5, 2.494e-3, 0.0, 295, 2.6, 0.5, 15};
      xsec->SetGroupOnlyParameters("Osc", OscParProp);
      Sample->Reweight();

      // Process posterior histogram
      TH1D *SampleHistogramPost = (TH1D*)Sample->GetMCHist(iSample, 1)->Clone(NameTString + "_Post");
      outFile << "Rates Post:" << SampleHistogramPrior->Integral() << std::endl;
      outFile << "Likelihood:" << std::fabs(Sample->GetLikelihood()) << std::endl;
      delete SampleHistogramPost;
    }
    MACH3LOG_INFO("Now trying to compare each weight individually");
    for (int iEntry = 0; iEntry < Sample->GetNEvents(); ++iEntry) {
      double weight = Sample->GetEventWeight(iEntry);
      outFile<< "Sample: "<< Sample->GetSampleHandlerName() << " Event: " << iEntry <<" weight: " << weight << std::endl;
    }

    SampleLLHValidation(outFile, configPath, xsec, true);
    SampleLLHValidation(outFile, configPath, xsec, false);
    LoadSplineValidation(outFile, configPath, xsec, false, true);
    LoadSplineValidation(outFile, configPath, xsec, true, false);

    // Clean up dynamically allocated Sample if needed
    delete Sample;
  }
  ValidateTestStatistic(outFile, SampleConfig[0], xsec);
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
