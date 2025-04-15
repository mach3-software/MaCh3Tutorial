#pragma once

#include "ParameterHandler/ParameterHandlerOsc.h"
#include "ParameterHandler/ParameterHandlerGeneric.h"
#include "Samples/SampleHandlerFD.h"
#include "StructsTutorial.h"
#include "splines/BinnedSplinesTutorial.h"

class SampleHandlerTutorial : public SampleHandlerFD
{
 public:
  SampleHandlerTutorial(std::string mc_version, ParameterHandlerGeneric* xsec_cov, ParameterHandlerOsc* osc_cov = nullptr);
  virtual ~SampleHandlerTutorial();
  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2, kM3Mode};

  std::vector<double> ReturnKinematicParameterBinning(std::string KinematicParameter) override;

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override;

  void SetupWeightPointers() override;

  int SetupExperimentMC(int iSample) override;

  double ReturnKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  double ReturnKinematicParameter(double KinematicVariable, int iSample, int iEvent) override;
  double ReturnKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iSample, int iEvent) override;

  void SetupFDMC(int iSample) override;
  void CalcWeightFunc(int iSample, int iEvent) override {return; (void)iSample; (void)iEvent;}

  std::vector<struct tutorial_base> TutorialSamples;

  const std::unordered_map<std::string, int> KinematicParametersTutorial = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueQ2", kTrueQ2},
    {"Mode", kM3Mode},
  };

  const std::unordered_map<int, std::string> ReversedKinematicParametersTutorial = {
    {kTrueNeutrinoEnergy, "TrueNeutrinoEnergy"},
    {kTrueQ2, "TrueQ2"},
    {kM3Mode,"Mode"},
  };

  double pot;
  bool isATM;
};
