// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "Samples/SampleStructs.h"

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  // Open a file in write mode
  std::ofstream outFile("NewBinningOut.txt");

  auto Binning = std::make_unique<SampleBinningInfo>();
  Binning->XBinEdges = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
  Binning->YBinEdges = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
  Binning->nXBins = Binning->XBinEdges.size() - 1;
  Binning->nYBins = Binning->YBinEdges.size() - 1;
  Binning->nBins = Binning->nXBins * Binning->nYBins;
  Binning->InitialiseBinMigrationLookUp();

  std::vector<std::pair<double, int>> testPairs = {
    {-1.0, -1},
    {-1.0, 0},
    {0.0, 0},
    {1.0, 9},

    {0.5, 1},
    {0.5, 2},
    {0.5, 3},
    {0.5, 4},
    {0.5, 5},
    {0.5, 6},
    {0.5, 7},
    {0.5, 8},
    {0.5, 9},

    {0.65, 1},
    {0.65, 2},
    {0.65, 3},
    {0.65, 4},
    {0.65, 5},
    {0.65, 6},
    {0.65, 7},
    {0.65, 8},
    {0.65, 9},

    {10.0, 4},
    {10.0, 5},
    {10.0, 6},

    {-10.0, 0},
  };

  for (const auto& [XVar, NomXBin] : testPairs) {
    int binIndex = Binning->FindXBin(XVar, NomXBin);
    outFile << "XVar: " << XVar << ", NomXBin: " << NomXBin << ", Bin: " << binIndex << std::endl;
  }

  for (size_t yBin = 0; yBin < Binning->nYBins; ++yBin) {
    for (size_t xBin = 0; xBin < Binning->nXBins; ++xBin) {
      const int binIndex = Binning->GetBinSafe(xBin, yBin);
      outFile << "yBin: " << yBin << ", xBin: " << xBin << ", Bin: " << binIndex << std::endl;

    }
  }

////////////// The End //////////////
  outFile.close();
  bool TheSame = CompareTwoFiles(TutorialPath + + "/CIValidations/TestOutputs/BinningOut.txt", "NewBinningOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}

