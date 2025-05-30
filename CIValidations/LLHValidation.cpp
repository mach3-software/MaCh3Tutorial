// MaCh3 spline includes
#include "Utils/Comparison.h"

/// Current tests include
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  MACH3LOG_INFO("Testing LLH scan");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  std::string command = TutorialPath + "/bin/LLHScanTutorial " +
                        TutorialPath + "/TutorialConfigs/FitterConfig.yaml";
  int ret = system(command.c_str());
  if (ret != 0) {
    MACH3LOG_WARN("Error: system call failed with code {}", ret);
  }

  auto file = std::unique_ptr<TFile>(TFile::Open("LLH_Test.root", "UPDATE"));
  std::vector<std::string> Names = {"Norm_Param_0", "Norm_Param_1", "Norm_Param_2", "BinnedSplineParam1", "BinnedSplineParam2", "BinnedSplineParam3", "BinnedSplineParam4", "BinnedSplineParam5", "EResLep", "EResTot"};
  std::ofstream outFile("LLH.txt");
  for (size_t i = 0; i < Names.size(); ++i) {
    std::string histPath = "Sample_LLH/" + Names[i] + "_sam";
    TH1D* hist = static_cast<TH1D*>(file->Get(histPath.c_str()));
    if (!hist) {
      MACH3LOG_CRITICAL("Something is wrong with LLH: missing histogram '{}'", histPath);
      throw MaCh3Exception(__FILE__, __LINE__);
    }
    for (int j = 1; j <= hist->GetNbinsX(); ++j) {
      double binContent = hist->GetBinContent(j);
      outFile << "Dial " << Names[i] << " bin = " << j << " content = " << std::fixed << std::setprecision(6) << std::fabs(binContent) << std::endl;
    }
  }
  outFile.close();

  bool TheSame = CompareTwoFiles(TutorialPath + "/CIValidations/TestOutputs/LLHOut.txt", "LLH.txt");
  if(!TheSame) {
    MACH3LOG_CRITICAL("Something is wrong with LLH");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  file->Close();
  return 0;
}
