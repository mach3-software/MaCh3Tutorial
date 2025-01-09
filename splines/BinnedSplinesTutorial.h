#pragma once

//MaCh3 includes
#include "splines/splineFDBase.h"

class BinnedSplineTutorial : public splineFDBase
{
  public:
	BinnedSplineTutorial(covarianceXsec *xsec_cov = nullptr);
	virtual ~BinnedSplineTutorial();

	void FillSampleArray(std::string SampleName, std::vector<std::string> OscChanFileNames);
	std::vector< std::vector<int> > StripDuplicatedModes(std::vector< std::vector<int> > InputVector) {return InputVector;};
	std::vector< std::vector<int> > GetEventSplines(std::string SampleName, int iOscChan, int EventMode, double Var1Val, double Var2Val, double Var3Val);
};
