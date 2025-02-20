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
	std::vector< std::vector<int> > StripDuplicatedModes(std::vector< std::vector<int> > InputVector) {return InputVector;};
	std::vector< std::vector<int> > GetEventSplines(std::string SampleName, int iOscChan, int EventMode, double Var1Val, double Var2Val, double Var3Val);

  MaCh3Modes* Modes;
};
