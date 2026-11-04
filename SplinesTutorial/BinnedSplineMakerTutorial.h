#pragma once

//MaCh3 includes
#include "Splines/BinnedSplineMakerBase.h"
#include "Manager/Random.h"
class BinnedSplineMakerTutorial : public BinnedSplineMakerBase
{
 public:
  BinnedSplineMakerTutorial(const std::string& Config);
  virtual ~BinnedSplineMakerTutorial();

  std::vector<double> GetETrueBinning() final;
  Long64_t GetEntries() final;
  double ReturnKinematicParameter(int binTypes, const Long64_t jentry) const final;
  void GetEntry(Long64_t jentry) final {}
  void LoadVariables() final {};
  void LoadKnots() final;
  double GetWeight(Long64_t jentry, size_t iSyst, int iKnot) final;
  double GetWeight([[maybe_unused]] Long64_t jentry,
                   [[maybe_unused]] size_t iSyst,
                   [[maybe_unused]] double Value) final {
    MACH3LOG_ERROR("Not implemented");
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  enum BinningType{kNoBinning, kTrueNeutrinoEnergy, kGeneratorMode, kErec, kTheta, kMom, kGenmom, kGencz};

  const std::unordered_map<std::string, int> BinningTypeTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"GeneratorMode", kGeneratorMode},
    {"erec", kErec},
    {"theta", kTheta},
  };

  const std::unordered_map<int, std::string> ReversedBinningTypeTutorial = {
    {kNoBinning, "none"},
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
    {kGeneratorMode, "GeneratorMode"},
    {kErec, "erec"},
    {kTheta, "theta"},
  };
};
