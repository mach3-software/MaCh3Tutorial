#pragma once

#include "Samples/SampleHandlerFD.h"
#include "StructsTutorial.h"
#include "SplinesTutorial/BinnedSplinesTutorial.h"

class SampleHandlerTutorial : public SampleHandlerFD
{
 public:
  SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                        const std::shared_ptr<OscillationHandler>& Oscillator_ = nullptr);
  virtual ~SampleHandlerTutorial();
  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode, kRecoNeutrinoEnergy};

  std::vector<double> ReturnKinematicParameterBinning(std::string KinematicParameter) override;

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override;

  void SetupWeightPointers() override;

  int SetupExperimentMC() override;

  double ReturnKinematicParameter(KinematicTypes KinPar, int iEvent);
  double ReturnKinematicParameter(int KinematicVariable, int iEvent);
  double ReturnKinematicParameter(std::string KinematicParameter, int iEvent);

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iEvent);
  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iEvent) override;
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iEvent);

  void SetupFDMC() override;
  void CalcWeightFunc(int iEvent) override {return; (void)iEvent;}

  std::vector<TutorialMCInfo> TutorialSamples;

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
