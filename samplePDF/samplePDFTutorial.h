#pragma once

#include "covariance/covarianceOsc.h"
#include "samplePDF/samplePDFFDBase.h"
#include "samplePDF/StructsTutorial.h"

class samplePDFTutorial : virtual public samplePDFFDBase
{
 public:
  samplePDFTutorial(std::string mc_version, covarianceXsec* xsec_cov, covarianceOsc* osc_cov = nullptr);
  ~samplePDFTutorial();
  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2};

  int ReturnKinematicParameterFromString(std::string KinematicParameterStr) override;
  std::string ReturnStringFromKinematicParameter(int KinematicParameterStr) override;

 protected:
  void Init() override;

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines() override;

  void SetupWeightPointers() override;

  int setupExperimentMC(int iSample) override;

  double ReturnKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  double ReturnKinematicParameter(double KinematicVariable, int iSample, int iEvent) override;
  double ReturnKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;

  const double* GetPointerToKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent);
  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) override;
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iSample, int iEvent) override;

  inline int ReturnKinematicParameterFromString(std::string KinematicParameterStr) override;
  inline std::string ReturnStringFromKinematicParameter(int KinematicParameterStr) override;
  std::vector<double> ReturnKinematicParameterBinning(std::string KinematicParameter) override;

  void setupFDMC(int iSample) override;
  M3::float_t CalcXsecWeightFunc(int iSample, int iEvent) override {(void)iSample; (void)iEvent; return 1.;}

  std::vector<struct tutorial_base> TutorialSamples;

  const std::unordered_map<std::string, int> KinematicParameters = {
    {"TrueNeutrinoEnergy", kTrueNeutrinoEnergy},
    {"TrueQ2", kTrueQ2}
  };

};
