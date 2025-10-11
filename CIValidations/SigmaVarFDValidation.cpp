// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "TKey.h"

/// Current tests include
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  MACH3LOG_INFO("Testing Sigma Var");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  std::string command = TutorialPath + "/bin/SigmaVarTutorial " +
                        TutorialPath + "/TutorialConfigs/FitterConfig.yaml";
  int ret = system(command.c_str());
  if (ret != 0) {
    MACH3LOG_WARN("Error: system call failed with code {}", ret);
  }

  auto file = std::unique_ptr<TFile>(TFile::Open("SigmaVar_Test.root", "UPDATE"));
  std::vector<std::string> Names = {"Norm_Param_0", "Norm_Param_1", "Norm_Param_2", "BinnedSplineParam1", "BinnedSplineParam2", "BinnedSplineParam3", "BinnedSplineParam4", "BinnedSplineParam5", "EResLep", "EResTot"};
  std::ofstream outFile("SigmaVar.txt");
  for (size_t i = 0; i < Names.size(); ++i) {
    std::string dirPath = "SigmaVar/" + Names[i] + "/Tutorial/";
    TDirectory* dir = file->GetDirectory(dirPath.c_str());

    if (!dir) {
      std::cerr << "Missing directory: " << dirPath << std::endl;
      return 1;
    }

    TList* keys = dir->GetListOfKeys();
    if (!keys) {
      std::cerr << "No keys found in: " << dirPath << std::endl;
      continue;
    }

    for (int i = 0; i < keys->GetSize(); ++i) {
      TKey* key = dynamic_cast<TKey*>(keys->At(i));
      if (!key) continue;

      TObject* obj = key->ReadObj();
      TH1* hist = dynamic_cast<TH1*>(obj);
      if (!hist) continue;

      for (int j = 1; j <= hist->GetNbinsX(); ++j) {
        double binContent = hist->GetBinContent(j);
        outFile << "Dial " << Names[i]
        << " hist = " << hist->GetTitle()
        << " bin = " << j
        << " content = " << std::fixed << std::setprecision(6)
        << std::fabs(binContent) << std::endl;
      }
    }
  }
  outFile.close();

  bool TheSame = CompareTwoFiles(TutorialPath + "/CIValidations/TestOutputs/SigmaVarOut.txt", "SigmaVar.txt");
  if(!TheSame) {
    MACH3LOG_CRITICAL("Something is wrong with SigmaVar");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  file->Close();
  return 0;
}
