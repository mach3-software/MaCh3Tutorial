#include "SplinesTutorial/BinnedSplineMakerTutorial.h"
#include "Utils/Comparison.h"

void ValidateSplines(std::ostream& outFile){
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
  const std::string rootPath = TutorialPath + "/SplineTutorialTest.root";

  TFile rootFile(rootPath.c_str(), "READ");
  if (rootFile.IsZombie()) {
    MACH3LOG_CRITICAL("Could not open ROOT file: {}", rootPath);
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  TIter next(rootFile.GetListOfKeys());
  while (TKey *key = static_cast<TKey *>(next())) {
    TObject *object = key->ReadObj();

    auto *spline = dynamic_cast<TSpline3 *>(object);
    if (spline == nullptr) {
      delete object;
      continue;
    }

    outFile << "Spline: " << spline->GetName() << '\n';

    const int nKnots = spline->GetNp();
    for (int i = 0; i < nKnots; ++i) {
      double x = 0.0;
      double y = 0.0;

      spline->GetKnot(i, x, y);
      outFile << "Knot: " << i << ", x: " << x << ", y: " << y << '\n';
    }

    outFile << '\n';
    delete object;
  }
  rootFile.Close();
}


int main(int argc, char *argv[]) {
  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  M3::rand::SetSeed(666);

  const std::string Config = std::string(std::getenv("MaCh3Tutorial_ROOT")) + "/TutorialConfigs/Spline/SplineTutorial.yaml";
  auto SplineMaker = std::make_unique<BinnedSplineMakerTutorial>(Config);

  SplineMaker->MakeVariations();

  std::ofstream outFile("NewSplineMakerOut.txt");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");
  ValidateSplines(outFile);
  outFile.close();

  bool TheSame = CompareTwoFiles(TutorialPath + + "/CIValidations/TestOutputs/SplineMakerOut.txt", "NewSplineMakerOut.txt");
  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
