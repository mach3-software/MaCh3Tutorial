#include "BinnedSplineMakerTutorial.h"

// ************************************************
BinnedSplineMakerTutorial::BinnedSplineMakerTutorial(const std::string& Config) : BinnedSplineMakerBase(Config) {
// ************************************************
  KinematicParameters = &BinningTypeTutorial;
  ReversedKinematicParameters = &ReversedBinningTypeTutorial;
  Initialise();
}

// ************************************************
BinnedSplineMakerTutorial::~BinnedSplineMakerTutorial() {
// ************************************************
}

// ************************************************
std::vector<double> BinnedSplineMakerTutorial::GetETrueBinning() {
// ************************************************
  return {0., 400, 500, 600, 700, 1000, 1500, 2500, 3500, 5000, 7000, 30000};
}

// ************************************************
double BinnedSplineMakerTutorial::GetWeight([[maybe_unused]] Long64_t jentry,
                                            size_t iSyst,
                                            int iKnot) {
// ************************************************
  auto tweak = systematicProperties[iSyst].knotArray->At(iKnot);
  return std::max(tweak*M3::rand::Gaus(1.0, 0.1), 0.);
}

// ************************************************
Long64_t BinnedSplineMakerTutorial::GetEntries() {
// ************************************************
  return 1000000;
}

// ************************************************
void BinnedSplineMakerTutorial::LoadKnots() {
// ************************************************
  const float knotValues[] = {0.1f, 0.3f, 1.0f, 1.1f, 1.3f};
  int numSysts = static_cast<int>(systematicProperties.size());

  for (int iSyst = 0; iSyst < numSysts; iSyst++) {
    systematicProperties[iSyst].knotArray = new TArrayF(5, knotValues);
  }
}

// ************************************************
double BinnedSplineMakerTutorial::ReturnKinematicParameter(int binTypes, [[maybe_unused]] const Long64_t jentry) const {
// ************************************************
  double var = 0;
  constexpr std::array<int, 13> allowedModes = {1, 2, 11, 12, 13, 21, 26, 31, 32, 33, 34, 51, 52};
  switch (binTypes)
  {
    case kErec:
      var = std::max(M3::rand::Gaus(600, 100), 0.);
      break;
    case kTheta:
      var = M3::rand::Uniform(0.0, 180.0);
      break;
    case kTrueNeutrinoEnergy:
      var = std::max(M3::rand::Gaus(0.6, 0.1), 0.);
      break;
    case kGeneratorMode:
      do {
        var = static_cast<int>(M3::rand::Uniform(0.0, 53.0));
      } while (std::find(allowedModes.begin(), allowedModes.end(),static_cast<int>(var)) == allowedModes.end());
      break;
    default:
      MACH3LOG_ERROR("Can only bin parameter 1 in erec (0) or momentum (1), white you passed {}", binTypes);
      throw MaCh3Exception(__FILE__, __LINE__);
  };
  return var;
}
