#pragma once

//MaCh3 includes
#include "splines/splineFDBase.h"

class BinnedSplineTutorial : public splineFDBase
{
  public:
  BinnedSplineTutorial(covarianceXsec *xsec_cov, MaCh3Modes *Modes);
  virtual ~BinnedSplineTutorial();
  
  std::vector<std::string> GetTokensFromSplineName(std::string FullSplineName);
};
