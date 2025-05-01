#pragma once

#include "covariance/covarianceOsc.h"
#include "samplePDF/samplePDFFDBase.h"
#include "samplePDF/StructsTutorial.h"

/// @brief Class responsible for handling implementation of samples used in analysis, reweighting and returning LLH
class samplePDFTutorial : public samplePDFFDBase
{
 public:
  samplePDFTutorial(std::string mc_version, covarianceXsec* xsec_cov, covarianceOsc* osc_cov = nullptr, OscillatorBase* Oscillator_ = nullptr);
  virtual ~samplePDFTutorial();
  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode, kRecoNeutrinoEnergy};

  std::vector<double> ReturnKinematicParameterBinning(std::string KinematicParameter) override;

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override;

  void SetupWeightPointers() override;

  int setupExperimentMC(int iSample) override;

  double ReturnKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  double ReturnKinematicParameter(int KinematicVariable, int iSample, int iEvent) override;
  double ReturnKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iSample, int iEvent) override;

  void setupFDMC(int iSample) override;
  void CalcWeightFunc(int iSample, int iEvent) override {return; (void)iSample; (void)iEvent;}

  std::vector<struct tutorial_base> TutorialSamples;

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
  enum FuncParEnum {kDebugShift, kEResLep, kEResTot};
  void RegisterFunctionalParameters() override;
  void resetShifts(int iSample, int iEvent) override;

  void DebugShift(const double * par, const std::size_t iSample, const std::size_t iEvent);
  void EResLep(const double * par, const std::size_t iSample, const std::size_t iEvent);
  void EResTot(const double * par, const std::size_t iSample, const std::size_t iEvent);
  // =================================
};
