// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

_MaCh3_Safe_Include_Start_ //{
#include "Oscillator/OscillatorFactory.h"
_MaCh3_Safe_Include_End_ //}

void PrepareConfig(const std::string& OriginalConfig) {
  std::string NameTString = OriginalConfig;

  // Load YAML config from file
  YAML::Node config = M3OpenConfig(NameTString);

  // Set UpdateW2 based on input flag
  config["General"]["OutputFile"] = "PredictiveOutputTest.root";
  config["General"]["TutorialSamples"] = YAML::Node(YAML::NodeType::Sequence);
  config["General"]["TutorialSamples"].push_back("TutorialConfigs/Samples/SampleHandler_Tutorial.yaml");
  config["General"]["TutorialSamples"].push_back("TutorialConfigs/Samples/SampleHandler_Tutorial_ATM.yaml");
  config["Predictive"]["PosteriorFile"] = "CIValidations/TestOutputs/PredictiveToys.root";

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
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  const char* rootEnv = std::getenv("MaCh3Tutorial_ROOT");
  PrepareConfig(std::string(rootEnv) + "/TutorialConfigs/FitterConfig.yaml");

  std::string tutorialPath = std::string(rootEnv);

  std::string command = tutorialPath + "/bin/PredictiveTutorial " +
                        tutorialPath + "/mach3_temp_config.yaml";
  int ret = system(command.c_str());
  if (ret != 0) {
    MACH3LOG_WARN("Error: system call failed with code {}", ret);
  }

  std::ofstream outFile("NewPredictiveOut.txt");

  TFile* outFileRoot = TFile::Open("PredictiveOutputTest.root", "READ");
  TDirectory* predictiveDir = static_cast<TDirectory*>(outFileRoot->Get("Predictive"));
  if (!predictiveDir) {
    MACH3LOG_CRITICAL("Failed to find 'Predictive' directory in PredictiveOutputTest.root");
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  std::vector<std::string> sampleNames = {"Tutorial", "Tutorial ATM"};
  for (const auto& sampleName : sampleNames) {
    TDirectory* sampleDir = static_cast<TDirectory*>(predictiveDir->Get(sampleName.c_str()));
    if (!sampleDir) {
      MACH3LOG_CRITICAL("Sample directory '{}' not found in 'Predictive'", sampleName);
      throw MaCh3Exception(__FILE__, __LINE__);
    }

    std::string histName = sampleName + "_mc";
    TH2D* hist = dynamic_cast<TH2D*>(sampleDir->Get(histName.c_str()));
    if (!hist) {
      MACH3LOG_CRITICAL("Histogram '{}' not found in sample directory '{}'", histName, sampleName);
      throw MaCh3Exception(__FILE__, __LINE__);
    }
    histName = sampleName + "_mc_PostPred";
    TH1D* hist1D = dynamic_cast<TH1D*>(sampleDir->Get(histName.c_str()));
    if (!hist1D) {
      MACH3LOG_CRITICAL("Histogram '{}' not found in sample directory '{}'", histName, sampleName);
      throw MaCh3Exception(__FILE__, __LINE__);
    }

    outFile << "Integral for sample "<< sampleName<< " integral" << hist1D->Integral() << std::endl;

    // Check each bin in 1D histogram
    int nBins1D = hist1D->GetNbinsX();
    for (int i = 1; i <= nBins1D; ++i) {
      const double content = hist1D->GetBinContent(i);
      const double error = hist1D->GetBinError(i);
      outFile << "Sample "<< sampleName<< " bin " << i << " content " << content << std::endl;
      outFile << "Sample "<< sampleName<< " bin " << i << " error " << error << std::endl;
    }
  }

  command = tutorialPath + "/bin/PredictivePlotting PredictiveOutputTest.root PredictiveOutputTest.root";
  ret = system(command.c_str());
  if (ret != 0) {
    MACH3LOG_WARN("Error: system call failed with code {}", ret);
  }

  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/Predictive.txt", "NewPredictiveOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }
}
