#include "TRandom3.h"
#include "TSpline.h"
#include "TFile.h"
#include "TGraph.h"
#include "TString.h"
#include "TH3F.h"

#include <iostream>
#include <algorithm>

void CreateBinnedSplineFile(){

  int nTrueEnergyBins = 5;
  int nXBins = 10;
  int nYBins = 1;
  double TrueEnergyBins[] = {0., 1.0, 2.0, 3.0, 4.0, 5.0};
  double XBins[] = {0., 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  double YBins[] = {0.,10.0};

  //Let's try out 5 systematics
  //This gives us a total of 5*10*10*5 = 2500 splines
  std::vector<int> SystematicKnots = {5, 7, 7, 2, 3};//, 7, 2, 7, 7, 7};
  std::vector<std::vector<std::string>> SystematicModeNames = {{"ccqe"}, {"cc1pipm"}, {"2p2h"}, {"other"}, {"ccqe"}};
  //Values to centre the gaussian response on
  std::vector<double> SystematicMeanResponse = {1.0, 2.0, 1.0, 1.5, 1.0};
  std::vector<int> SystematicNominalKnot = {3, 4, 4, 1, 2};
  std::vector<std::string> SystematicNames = {"mysyst1", "mysyst2", "mysyst3","mysyst4","mysyst5"};

  if(SystematicKnots.size() != SystematicNames.size() && SystematicNames.size() != SystematicNominalKnot.size() && SystematicKnots.size() != SystematicMeanResponse.size()) {
    std::cerr << "Systematic vectors are not of equal length this might cause problems, please fix!" << std::endl;
    throw;
  }

  TRandom3 Random = TRandom3(0);

  auto OutputFile = std::unique_ptr<TFile>(TFile::Open("BinnedSplinesTutorialInputs.root", "RECREATE"));

  //Firstly let's write the dev.tmp.0.0 file which sets the binning
  // TH3F* BinningHist = new TH3F("dev_tmp.0.0", "dev_tmp.0.0", nTrueEnergyBins, 0, 10, nXBins, 0, 10, nYBins, 0, 10);
  TH3F* BinningHist = new TH3F("dev_tmp.0.0", "dev_tmp.0.0", nTrueEnergyBins, TrueEnergyBins, nXBins, XBins, nYBins, YBins);

  for(auto iSyst = 0 ; iSyst < SystematicNames.size() ; ++iSyst){
    for(auto SystematicModeName : SystematicModeNames[iSyst]){
      for(auto SystematicKnotNumber : SystematicKnots) {
        for(auto TrueEnergyBin_i = 0 ; TrueEnergyBin_i < nTrueEnergyBins ; ++TrueEnergyBin_i){
          for(auto XBin_i = 0 ; XBin_i < nXBins ; ++XBin_i){
            for(auto YBin_i = 0 ; YBin_i < nYBins ; ++YBin_i){
              double knot_w = 1;
              TGraph *graph = new TGraph(SystematicKnotNumber);

              for(auto iKnot = 0 ; iKnot < SystematicKnotNumber ; ++iKnot) { 
                //Check on if you are the nominal knot
                if(iKnot != SystematicNominalKnot[iSyst]) {
                  //Do a random throw from a gaussian 
                  knot_w = Random.Gaus(SystematicMeanResponse[iSyst], 1.0);
                  knot_w = std::max(0., knot_w);
                  // std::cout << "Knot_w is " << knot_w << std::endl;
                  //point number, x-val, y-val
                }
                // std::cout << "For knot " << iKnot << "/" << SystematicKnotNumber << " set weight to be " << knot_w << std::endl;
                graph->SetPoint(iKnot, iKnot, knot_w);
              }
              TSpline3 *Spline = new TSpline3(Form("dev.%s.%s.sp.%i.%i.%i", SystematicNames[iSyst].c_str(), SystematicModeName.c_str(), TrueEnergyBin_i, XBin_i, YBin_i),graph);
              Spline->SetName(Form("dev.%s.%s.sp.%i.%i.%i", SystematicNames[iSyst].c_str(), SystematicModeName.c_str(), TrueEnergyBin_i, XBin_i, YBin_i));
              Spline->Write(Spline->GetName(), TDirectoryFile::kOverwrite);//, TDirectoryFile::kOverwrite);
              delete graph;
              delete Spline;
            }
          }
        }
      }
    }
  }

  OutputFile->Write();
  OutputFile->Close();

  return;
}
