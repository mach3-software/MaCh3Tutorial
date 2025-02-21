// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "manager/MaCh3Modes.h"

/// Current tests include
int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  MACH3LOG_INFO("Testing MaCh3 Mode");

  std::string ModeInput = "TutorialConfigs/MaCh3Modes.yaml";
  auto Modes = std::make_unique<MaCh3Modes>(ModeInput);

  std::ofstream outFile("NewMaCh3ModeOut.txt");

  outFile << "Mode from generator for 1  = "  << Modes->GetModeFromGenerator(1) << std::endl;
  outFile << "Mode from generator for 2  = "  << Modes->GetModeFromGenerator(2) << std::endl;
  outFile << "Mode from generator for 10 = "  << Modes->GetModeFromGenerator(10) << std::endl;
  outFile << "Mode from generator for 60 = "  << Modes->GetModeFromGenerator(60) << std::endl;

  outFile << "MaCh3ModeName for 1  = "  << Modes->GetMaCh3ModeName(1) << std::endl;
  outFile << "MaCh3ModeName for 2  = "  << Modes->GetMaCh3ModeName(2) << std::endl;
  outFile << "MaCh3ModeName for 10 = "  << Modes->GetMaCh3ModeName(10) << std::endl;
  outFile << "MaCh3ModeName for 60 = "  << Modes->GetMaCh3ModeName(60) << std::endl;

  outFile << "MaCh3ModeFancyName for 1  = "   << Modes->GetMaCh3ModeFancyName(1) << std::endl;
  outFile << "MaCh3ModeFancyName for 2  = "   << Modes->GetMaCh3ModeFancyName(2) << std::endl;
  outFile << "MaCh3ModeFancyName for 10 = "  << Modes->GetMaCh3ModeFancyName(10) << std::endl;
  outFile << "MaCh3ModeFancyName for 60 = "  << Modes->GetMaCh3ModeFancyName(60) << std::endl;

  outFile << "GetMode for CCQE   = "  << Modes->GetMode("CCQE") << std::endl;
  outFile << "GetMode for CC2p2h = "  << Modes->GetMode("CC2p2h") << std::endl;
  outFile << "GetMode for Test   = "  << Modes->GetMode("Test") << std::endl;

  outFile << "SplineSuffixFromMaCh3Mode for 1  = "   << Modes->GetSplineSuffixFromMaCh3Mode(1) << std::endl;
  outFile << "SplineSuffixFromMaCh3Mode for 2  = "   << Modes->GetSplineSuffixFromMaCh3Mode(2) << std::endl;
  outFile << "SplineSuffixFromMaCh3Mode for 10 = "   << Modes->GetSplineSuffixFromMaCh3Mode(10) << std::endl;
  outFile << "SplineSuffixFromMaCh3Mode for 60 = "   << Modes->GetSplineSuffixFromMaCh3Mode(60) << std::endl;

  outFile << "IsMaCh3ModeNC for 1  = "   << Modes->IsMaCh3ModeNC(1) << std::endl;
  outFile << "IsMaCh3ModeNC for 2  = "   << Modes->IsMaCh3ModeNC(2) << std::endl;
  outFile << "IsMaCh3ModeNC for 10 = "   << Modes->IsMaCh3ModeNC(10) << std::endl;
  outFile << "IsMaCh3ModeNC for 60 = "   << Modes->IsMaCh3ModeNC(60) << std::endl;

  outFile << "GetNModes = " << Modes->GetNModes() << std::endl;

  outFile.close();

  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/MaCh3ModeOut.txt", "NewMaCh3ModeOut.txt");
  if(!TheSame) {
    MACH3LOG_CRITICAL("Something is wrong with MaCh3 modes");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }
  return 0;
}
