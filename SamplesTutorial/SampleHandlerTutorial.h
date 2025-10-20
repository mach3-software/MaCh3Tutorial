#pragma once

#include "Samples/SampleHandlerFD.h"
#include "StructsTutorial.h"
#include "SplinesTutorial/BinnedSplinesTutorial.h"
#include <random>

class SampleHandlerTutorial : public SampleHandlerFD
{
 public:
  SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                        const std::shared_ptr<OscillationHandler>& Oscillator_ = nullptr);
  virtual ~SampleHandlerTutorial();

  enum KinematicTypes {kTrueNeutrinoEnergy, TrueCosineZenith, kTrueQ2, kTarget, kM3Mode, kRecoNeutrinoEnergy, kOscChannel};
  
  // === JM enum for particle-level parameters ===
  enum KinematicParticleVecs {kParticleEnergy, kParticlePDG, kParticleBeamAngle};
  // =============================================

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override;

  void SetupWeightPointers() override;

  int SetupExperimentMC() override;

  void CleanMemoryBeforeFit() override;

  double ReturnKinematicParameter(const KinematicTypes KinPar, const int iEvent) const;
  double ReturnKinematicParameter(const int KinematicVariable, const int iEvent) const override;
  double ReturnKinematicParameter(const std::string& KinematicParameter, const int iEvent) const override;
  
  // === JM ReturnKinematicVector declarations for particle-level parameters ===
  std::vector<double> ReturnKinematicVector(const KinematicParticleVecs KinVec, const int iEvent) const;
  std::vector<double> ReturnKinematicVector(const int KinematicVector, const int iEvent) const override;
  std::vector<double> ReturnKinematicVector(const std::string& KinematicVector, const int iEvent) const override;
  // ===========================================================================

  const double* GetPointerToKinematicParameter(const KinematicTypes KinPar, const int iEvent) const;
  const double* GetPointerToKinematicParameter(const std::string& KinematicParameter, const int iEvent) const override;
  const double* GetPointerToKinematicParameter(const double KinematicVariable, const int iEvent) const override;

  void SetupFDMC() override;
  void CalcWeightFunc(int iEvent) override {return; (void)iEvent;}

  std::vector<TutorialMCInfo> TutorialSamples;
  std::vector<TutorialMCPlottingInfo> TutorialPlottingSamples;

  const std::unordered_map<std::string, int> KinematicParametersTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueCosineZenith", TrueCosineZenith},
    {"Target", kTarget},
    {"TrueQ2", kTrueQ2},
    {"Mode", kM3Mode},
    {"RecoNeutrinoEnergy", kRecoNeutrinoEnergy},
    {"OscillationChannel", kOscChannel},
  };

  const std::unordered_map<int, std::string> ReversedKinematicParametersTutorial = {
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
    {TrueCosineZenith, "TrueCosineZenith"},
    {kTarget, "Target"},
    {kTrueQ2, "TrueQ2"},
    {kM3Mode,"Mode"},
    {kRecoNeutrinoEnergy, "RecoNeutrinoEnergy"},
    {kOscChannel, "OscillationChannel"},
  };
  
  // === JM maps for particle-level parameters ===
  const std::unordered_map<std::string, int> KinematicVectorsTutorial = {
    {"ParticleEnergy", kParticleEnergy},
    {"ParticlePDG", kParticlePDG},
    {"ParticleBeamAngle", kParticleBeamAngle},
  };

  const std::unordered_map<int, std::string> ReversedKinematicVectorsTutorial = {
    {kParticleEnergy, "ParticleEnergy"},
    {kParticlePDG, "ParticlePDG"},
    {kParticleBeamAngle, "kParticleBeamAngle"},
  };
  // =============================================

  double pot;
  bool isATM;

  // === HH: Functional parameters ===
  enum FuncParEnum {kDebugNothing, kDebugShift, kEResLep, kEResTot};
  void RegisterFunctionalParameters() override;
  void resetShifts(int iEvent) override;

  void DebugShift(const double* par, std::size_t iEvent);
  void EResLep(const double* par, std::size_t iEvent);
  void EResTot(const double* par, std::size_t iEvent);
  // =================================
};
