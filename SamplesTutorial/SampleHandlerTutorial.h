#pragma once

#include "Samples/SampleHandlerBase.h"
#include "StructsTutorial.h"
#include "SplinesTutorial/BinnedSplinesTutorial.h"
#include <random>

class SampleHandlerTutorial : public SampleHandlerBase
{
 public:
  SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                        const std::shared_ptr<OscillationHandler>& Oscillator_ = nullptr);
  virtual ~SampleHandlerTutorial();

  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode, kTarget, kRecoNeutrinoEnergy, kOscChannel};
  
  // === JM enum for particle-level parameters ===
  enum KinematicParticleVecs {kParticleEnergy, kParticlePDG, kParticleBeamAngle};
  // =============================================

 private:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() final;

  void AddAdditionalWeightPointers() final;

  int SetupExperimentMC() final;

  void CleanMemoryBeforeFit() final;

  double ReturnKinematicParameter(const int KinematicVariable, const int iEvent) const final;
  
  // === JM ReturnKinematicVector declarations for particle-level parameters ===
  void FillParticles(int eventIndex, int nParticles, int PDGLep, double ELep, std::mt19937& gen);
  std::vector<double> ReturnKinematicVector(const int KinematicVector, const int iEvent) const final;
  // ===========================================================================

  const double* GetPointerToKinematicParameter(const int KinematicVariable, const int iEvent) const final;
  /// @brief Function which translates experiment struct into core struct
  void SetupMC() final;
  /// @brief Function responsible for loading data from file or loading from file
  void InititialiseData() final;
  void CalcWeightFunc(const int iEvent) final {return; (void)iEvent;}

  std::vector<TutorialMCInfo> TutorialSamples;
  std::vector<TutorialMCPlottingInfo> TutorialPlottingSamples;

  const std::unordered_map<std::string, int> KinematicParametersTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueQ2", kTrueQ2},
    {"Mode", kM3Mode},
    {"Target", kTarget},
    {"RecoNeutrinoEnergy", kRecoNeutrinoEnergy},
    {"OscillationChannel", kOscChannel},
  };

  const std::unordered_map<int, std::string> ReversedKinematicParametersTutorial = {
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
    {kTrueQ2, "TrueQ2"},
    {kM3Mode, "Mode"},
    {kTarget, "Target"},
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
  void RegisterFunctionalParameters() final;
  void ResetShifts(const int iEvent) final;

  void DebugShift(const M3::float_t* par, std::size_t iEvent);
  void EResLep(const M3::float_t* par, std::size_t iEvent);
  void EResTot(const M3::float_t* par, std::size_t iEvent);
  // =================================
};
