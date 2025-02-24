#pragma once

//MaCh3 includes
#include "splines/BinnedSplineHandler.h"

class BinnedSplineTutorial : public BinnedSplineHandler
{
  public:
  BinnedSplineTutorial(SystematicHandlerGeneric *xsec_cov, MaCh3Modes *Modes);
  virtual ~BinnedSplineTutorial();
  
  std::vector<std::string> GetTokensFromSplineName(std::string FullSplineName);
};
