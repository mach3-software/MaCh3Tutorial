#pragma once

//MaCh3 includes
#include "splines/splineFDBase.h"
#include "manager/MaCh3Modes.h"

class BinnedSplineTutorial : public splineFDBase
{
  public:
  BinnedSplineTutorial(covarianceXsec *xsec_cov, MaCh3Modes *Modes);
  virtual ~BinnedSplineTutorial();
  
  void FillSampleArray(std::string SampleName, std::vector<std::string> OscChanFileNames);
};
