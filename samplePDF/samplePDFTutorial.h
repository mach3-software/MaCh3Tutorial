#pragma once

#include "samplePDF/samplePDFFDBase.h"
#include "samplePDF/StructsTutorial.h"

/// @todo add target
class samplePDFTutorial : virtual public samplePDFFDBase
{
public:
  samplePDFTutorial(std::string mc_version, covarianceXsec* xsec_cov);
  ~samplePDFTutorial();
  enum KinematicTypes {kTrueNeutrinoEnergy, kTrueQ2};

 protected:
  void Init();

  ///@brief Setup our spline file, this calls InitialseSplineObject() under the hood
  void SetupSplines();

  void SetupWeightPointers();

  int setupExperimentMC(int iSample);

  double ReturnKinematicParameter (double KinematicVariable, int iSample, int iEvent);
  double ReturnKinematicParameter(std::string KinematicParameter, int iSample, int iEvent);

  const double* GetPointerToKinematicParameter(std::string KinematicParameter, int iSample, int iEvent);
  const double* GetPointerToKinematicParameter(double KinematicVariable, int iSample, int iEvent);

  inline int ReturnKinematicParameterFromString(std::string KinematicParameterStr);
  inline std::string ReturnStringFromKinematicParameter(int KinematicParameterStr);
  std::vector<double> ReturnKinematicParameterBinning(std::string KinematicParameter);

  void setupFDMC(int iSample);
  double CalcXsecWeightFunc(int iSample, int iEvent) {(void)iSample; (void)iEvent; return 1.;}

  std::vector<struct tutorial_base> TutorialSamples;
};
