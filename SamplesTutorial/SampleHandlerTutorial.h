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

  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode, kRecoNeutrinoEnergy};
  
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

  double ReturnKinematicParameter(KinematicTypes KinPar, int iEvent);
  double ReturnKinematicParameter(int KinematicVariable, int iEvent);
  double ReturnKinematicParameter(std::string KinematicParameter, int iEvent);
  
  // === JM ReturnKinematicVector declarations for particle-level parameters ===
  std::vector<double> ReturnKinematicVector(KinematicParticleVecs KinVec, int iEvent);
  std::vector<double> ReturnKinematicVector(int KinematicVector, int iEvent);
  std::vector<double> ReturnKinematicVector(std::string KinematicVector, int iEvent);
  std::vector<double> ReturnKinematicVectorBinning(std::string KinematicParameter);
  // ===========================================================================

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iEvent);
  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iEvent) override;
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iEvent);

  void SetupFDMC() override;
  void CalcWeightFunc(int iEvent) override {return; (void)iEvent;}

  std::vector<TutorialMCInfo> TutorialSamples;
  std::vector<TutorialMCPlottingInfo> TutorialPlottingSamples;

  const std::unordered_map<std::string, int> KinematicParametersTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueQ2", kTrueQ2},
    {"Mode", kM3Mode},
    {"RecoNeutrinoEnergy", kRecoNeutrinoEnergy},
  };

  const std::unordered_map<int, std::string> ReversedKinematicParametersTutorial = {
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
    {kTrueQ2, "TrueQ2"},
    {kM3Mode,"Mode"},
    {kRecoNeutrinoEnergy, "RecoNeutrinoEnergy"},
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
