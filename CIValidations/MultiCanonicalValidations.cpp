// MaCh3 spline includes
#include "Utils/SplineMonoUtils.h"
#include "Utils/Comparison.h"

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  if(!std::getenv("MaCh3Tutorial_ROOT")){
    MACH3LOG_CRITICAL("${MaCh3Tutorial_ROOT} is not defined in the environment,"
      " have you sourced setup.MaCh3Tutorial.sh? ");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  // Open a file in write mode
  std::ofstream outFile("NewUmbrellaOut.txt");

  TFile *Output = TFile::Open("umbrella_output.root", "READ");
  if (!Output || Output->IsZombie()) {
    MACH3LOG_CRITICAL("Couldn't find umbrella file, please run via CIValidations/MaCh3CLI --MultiCanonicalValidations");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  TTree *Tree = Output->Get<TTree>("posteriors");

  int window_id = M3::_BAD_INT_;
  double umbrella_weight = M3::_BAD_DOUBLE_;
  Tree->SetBranchStatus("*", false);
  Tree->SetBranchStatus("umbrella_weight", true);
  Tree->SetBranchAddress("umbrella_weight", &umbrella_weight);
  Tree->SetBranchStatus("window_id", true);
  Tree->SetBranchAddress("window_id", &window_id);

  for (Long64_t iEvent = 0; iEvent < Tree->GetEntries(); iEvent += 10) {
    Tree->GetEntry(iEvent);
    outFile << "Event: " << iEvent << " umbrella_weight " << umbrella_weight << " window_id " << window_id << std::endl;
  }

  outFile.close();
  bool TheSame = CompareTwoFiles(TutorialPath + "/CIValidations/TestOutputs/UmbrellaOut.txt", "NewUmbrellaOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different weights mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
