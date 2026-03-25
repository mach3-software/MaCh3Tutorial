#pragma once

#include "Samples/SampleHandlerBase.h"
#include "StructsTutorial.h"
#include "SplinesTutorial/BinnedSplinesTutorial.h"

class SampleHandlerTutorial : public SampleHandlerBase
{
 public:
  SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                        const std::shared_ptr<OscillationHandler>& Oscillator_ = nullptr);
  virtual ~SampleHandlerTutorial();

  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode, kRecoNeutrinoEnergy, kOscChannel};
  
  // === JM enum for particle-level parameters ===
  enum KinematicParticleVecs {kParticleEnergy, kParticlePDG, kParticleBeamAngle};
  // =============================================

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override final;

  void AddAdditionalWeightPointers() override final;

  int SetupExperimentMC() override final;

  void CleanMemoryBeforeFit() override final;

  double ReturnKinematicParameter(KinematicTypes KinPar, int iEvent);
  double ReturnKinematicParameter(int KinematicVariable, int iEvent) override final;
  
  // === JM ReturnKinematicVector declarations for particle-level parameters ===
  std::vector<double> ReturnKinematicVector(KinematicParticleVecs KinVec, int iEvent);
  std::vector<double> ReturnKinematicVector(int KinematicVector, int iEvent) override final;
  // ===========================================================================

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iEvent);
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iEvent) override final;

  void SetupMC() override final;
  void CalcWeightFunc(int iEvent) override final {return; (void)iEvent;}

  std::vector<TutorialMCInfo> TutorialSamples;
  std::vector<TutorialMCPlottingInfo> TutorialPlottingSamples;

  const std::unordered_map<std::string, int> KinematicParametersTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueQ2", kTrueQ2},
    {"Mode", kM3Mode},
    {"RecoNeutrinoEnergy", kRecoNeutrinoEnergy},
    {"OscillationChannel", kOscChannel},
  };

  const std::unordered_map<int, std::string> ReversedKinematicParametersTutorial = {
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
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
  void RegisterFunctionalParameters() override final;
  void ResetShifts(const int iEvent) override final;

  void DebugShift(const M3::float_t* par, std::size_t iEvent);
  void EResLep(const M3::float_t* par, std::size_t iEvent);
  void EResTot(const M3::float_t* par, std::size_t iEvent);
  // =================================
};
