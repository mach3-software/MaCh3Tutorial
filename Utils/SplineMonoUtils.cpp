// MaCh3 spline includes
#include "Utils/SplineMonoUtils.h"

std::vector< std::vector<TResponseFunction_red*> > GetMasterSpline(
    std::string FileName, std::vector<std::string> DialNames)
{
  std::vector< std::vector<TResponseFunction_red*> > MasterSpline;

  TChain* chain = new TChain("sample_sum");
  chain->AddFile(FileName.c_str());

  const int nSplineParams = static_cast<int>(DialNames.size());
  TGraph** xsecgraph = new TGraph*[nSplineParams];
  for(int i = 0; i < nSplineParams; i++) {
    xsecgraph[i] = NULL;
  }
  TObjArray** grapharrays = new TObjArray*[nSplineParams];
  for(int i = 0; i < nSplineParams; i++) {
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

  unsigned int n = static_cast<unsigned int>(chain->GetEntries());
  MasterSpline.resize(n);
  for(unsigned int i = 0; i < n; ++i)
  {
    chain->GetEntry(i);
    MasterSpline[i].resize(nSplineParams);
    // Now set the xsecgraphs for the struct
    for (int k = 0; k < nSplineParams; k++)
    {
      // get the graph
      xsecgraph[k] = static_cast<TGraph*>(grapharrays[k]->At(0));
      MasterSpline[i][k] = CreateResponseFunction(xsecgraph[k], kTSpline3_red, kTSpline3, std::to_string(k).c_str());
    }
  } // end for j loop
  for(int i = 0; i < nSplineParams; i++) {
    if(xsecgraph[i] != NULL) delete xsecgraph[i];
    //if(grapharrays[i] != NULL) delete grapharrays[i];
  }
  delete[] xsecgraph;
  delete[] grapharrays;
  delete chain;
  return MasterSpline;
}
