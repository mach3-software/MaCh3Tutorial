#pragma once

//MaCh3 includes
#include "ParameterHandler/ParameterHandlerGeneric.h"
#include "splines/BinnedSplineHandler.h"

class BinnedSplineTutorial : public BinnedSplineHandler
{
  public:
  BinnedSplineTutorial(ParameterHandlerGeneric *parameter_handler, MaCh3Modes *Modes);
  virtual ~BinnedSplineTutorial();
  
  std::vector<std::string> GetTokensFromSplineName(std::string FullSplineName);
};
