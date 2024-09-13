// MaCh3 spline includes
#include "Utils/SplineMonoUtils.h"

std::vector< std::vector<TResponseFunction_red*> > GetMasterSpline(
    std::string FileName, std::vector<std::string> DialNames)
{
  std::vector< std::vector<TResponseFunction_red*> > MasterSpline;

  TChain* chain = new TChain("sample_sum");
  chain->AddFile(FileName.c_str());

  const int nSplineParams = DialNames.size();
  TGraph** xsecgraph = new TGraph*[nSplineParams];
  for(_int_ i = 0; i < nSplineParams; i++) {
    xsecgraph[i] = NULL;
  }
  TObjArray** grapharrays = new TObjArray*[nSplineParams];
  for(_int_ i = 0; i < nSplineParams; i++) {
    grapharrays[i] = NULL;
  }
  chain->SetBranchStatus("*", false);
  for(unsigned int i = 0; i < DialNames.size(); ++i) {
    std::string str = DialNames[i];
    const char *cstr = str.c_str();
    //std::cout<< "cstr = "<< cstr <<std::endl;
    chain->SetBranchStatus(cstr, true);
    chain->SetBranchAddress(cstr, &(grapharrays[i]));
  }// End the for loop over the unique spline parameters

  unsigned int n = chain->GetEntries();
  MasterSpline.resize(n);
  for(unsigned int i = 0; i < n; ++i)
  {
    chain->GetEntry(i);
    MasterSpline[i].resize(nSplineParams);
    // Now set the xsecgraphs for the struct
    for (_int_ k = 0; k < nSplineParams; k++)
    {
      // get the graph
      xsecgraph[k] = (TGraph*)(grapharrays[k]->At(0));

      TSpline3 *spline = NULL;
      TSpline3_red *spline_red = NULL;
      // For a valid spline we require it not be null and have more than one point
      if (xsecgraph[k] && xsecgraph[k]->GetN() > 1)
      {
        // Create the TSpline3* from the TGraph* and build the coefficients
        spline = new TSpline3(std::to_string(k).c_str(), xsecgraph[k]);
        spline->SetNameTitle(std::to_string(k).c_str(), std::to_string(k).c_str());

        // Make the reduced TSpline3 format and delete the old spline
        spline_red = new TSpline3_red(spline, kTSpline3);

        // For events without the jth spline, set to NULL
      } else {
        spline_red = NULL;
      }
      MasterSpline[i][k] = spline_red;
    }
  } // end for j loop
  for(_int_ i = 0; i < nSplineParams; i++) {
    if(xsecgraph[i] != NULL) delete xsecgraph[i];
    if(grapharrays[i] != NULL) delete grapharrays[i];
  }
  delete[] xsecgraph;
  delete[] grapharrays;
  delete chain;
  return MasterSpline;
}
