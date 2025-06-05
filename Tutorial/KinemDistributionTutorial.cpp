// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

struct PlotKinematicCut {
  std::string ParamToCutOn = "";
  double LowerBound = M3::_BAD_DOUBLE_;
  double UpperBound = M3::_BAD_DOUBLE_;
};

struct Plot {
  std::string Name;
  std::vector<std::string> VarStrings;
  std::vector<std::vector<double>> BinEdges;
  std::vector<PlotKinematicCut> SelectionCuts = {};
};

int main(int argc, char **argv) {
  int Selection = 1; //0 to loop modes, 1 to loop osc channels

  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);

  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");
  auto osc  = MaCh3CovarianceFactory<ParameterHandlerOsc>(FitManager.get(), "Osc");

  // Initialise samplePDF
  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get(), osc.get());

  // Output file is named after output filename in config plus plot variable(s)
  std::string configOutName = FitManager->raw()["General"]["OutputFile"].as<std::string>();
  std::stringstream OutName;
  OutName<<configOutName.substr(0,configOutName.find(".root"));

  TString OutputName = TString(OutName.str()) + "_KinemPlot" + ".pdf";

  std::vector<std::string> vecParams = {"TrueNeutrinoEnergy", "TrueQ2", "RecoNeutrinoEnergy"};
  
  //JM Read in kinematic distribution plots from config
  std::vector<Plot> PlotsToDraw = {};
  auto ConfigPlots = FitManager->raw()["KinematicDistributionPlots"];
  
  for (const auto& ConfigPlot : ConfigPlots) {
    Plot PlotToDraw;
    PlotToDraw.Name = ConfigPlot["Name"].as<std::string>();
    PlotToDraw.VarStrings = ConfigPlot["VarStrings"].as<std::vector<std::string>>();
    if (PlotToDraw.VarStrings.size() != 1 && PlotToDraw.VarStrings.size() != 2) {
      MACH3LOG_ERROR("Error in yaml file: In KinemDistribtuion Plot {}, VarStrings is of size {}. VarString should be of size 1 or 2 (higher dimensional histogram plotting is not yet supported)");
      throw MaCh3Exception(__FILE__,__LINE__);
    }
    PlotToDraw.BinEdges = ConfigPlot["VarBins"].as<std::vector<std::vector<double>>>();
    if (PlotToDraw.BinEdges.size() != 1 && PlotToDraw.BinEdges.size() != 2) {
      MACH3LOG_ERROR("Error in yaml file: In KinemDistribtuion Plot {}, BinEdges is of size {}. VarString should be of size 1 or 2 (higher dimensional histogram plotting is not yet supported)");
      throw MaCh3Exception(__FILE__,__LINE__);
    }
    
    //If binning vector is of size 3, treat as [nbins, xmin, xmax] (otherwise treat as bin edges)
    for (unsigned int iBinning=0; iBinning<PlotToDraw.BinEdges.size(); iBinning++) {
      if (PlotToDraw.BinEdges[iBinning].size() == 3) {
        double nbins = PlotToDraw.BinEdges[iBinning][0];
        double xmin = PlotToDraw.BinEdges[iBinning][1];
        double xmax = PlotToDraw.BinEdges[iBinning][2];
        double step = (xmax-xmin)/nbins;
        PlotToDraw.BinEdges[iBinning] = {};
        for (double iBinEdge=xmin; iBinEdge<=xmax; iBinEdge+=step) {
          PlotToDraw.BinEdges[iBinning].push_back(iBinEdge);
        }
        if (PlotToDraw.BinEdges[iBinning].size() == nbins+1) {
          PlotToDraw.BinEdges[iBinning].back() = xmax;
        } else {
          PlotToDraw.BinEdges[iBinning].push_back(xmax);
        }
      }
    }
    
    for (const auto& Cut : ConfigPlot["KinematicCuts"]) {
      PlotKinematicCut SelectionCut;
      SelectionCut.ParamToCutOn = Cut["VarString"].as<std::string>();
      std::vector<double> range = Cut["Range"].as<std::vector<double>>();
      
      if (range.size() != 2) {
        MACH3LOG_ERROR("Error in yaml file: In KinemDistribution Plot {}, KinematicCut {} has range of size {}. Range should be of size 2.", PlotToDraw.Name, SelectionCut.ParamToCutOn, range.size());
        throw MaCh3Exception(__FILE__,__LINE__);
      }
      SelectionCut.LowerBound = range[0];
      SelectionCut.UpperBound = range[1];
      PlotToDraw.SelectionCuts.push_back(SelectionCut);
    }
    PlotsToDraw.push_back(PlotToDraw);
  }

  TCanvas* Canv = new TCanvas("Canv","");
  Canv->Divide(1,2);
  Canv->Print(OutputName+"[");

  for (size_t iParam = 0; iParam < vecParams.size(); iParam++) {
    TString ParamDirName = vecParams[iParam];

    for (size_t iPDF=0; iPDF < mySamples.size(); iPDF++) {
      MACH3LOG_INFO("Number of samples: {}", mySamples[iPDF]->GetNsamples());

      THStack* Stack = new THStack(*mySamples[iPDF]->ReturnStackedHistBySelection1D(vecParams[iParam], Selection));
      TLegend* Legend = new TLegend(*mySamples[iPDF]->ReturnStackHistLegend());

      Canv->cd(1);
      Stack->Draw("HIST");
      //Due to crappy TStack design, you need to draw THStack first then assign axis titles
      Stack->SetTitle(mySamples[iPDF]->GetTitle().c_str());
      Stack->GetXaxis()->SetTitle((vecParams[iParam]).c_str());
      Canv->cd(2);
      Legend->Draw();

      Canv->Update();
      Canv->Print(OutputName);
      delete Stack; Stack = nullptr;
      delete Legend; Legend = nullptr;
    }
  }

  delete Canv;
  Canv = new TCanvas("Canv","");
  TH1* Hist;
  int WeightStyle = 1;
  for (size_t iHist=0; iHist<PlotsToDraw.size(); iHist++) {
    MACH3LOG_INFO("Plotting kinematic distributions in config: {} / {}", iHist+1, PlotsToDraw.size());  

    std::vector<std::string> PlotVar_Str = PlotsToDraw[iHist].VarStrings;
    int histdim = PlotVar_Str.size();
    TAxis AxisX = TAxis(PlotsToDraw[iHist].BinEdges[0].size()-1,PlotsToDraw[iHist].BinEdges[0].data());
    TAxis AxisY;
    if (histdim == 2) AxisY = TAxis(PlotsToDraw[iHist].BinEdges[1].size()-1,PlotsToDraw[iHist].BinEdges[1].data());

    for (size_t iPDF = 0;iPDF < mySamples.size(); iPDF++) {
      std::vector<KinematicCut> EventSelectionVector = {};
      std::vector<KinematicCut> SubEventSelectionVector = {};

      for (size_t iCut=0; iCut<PlotsToDraw[iHist].SelectionCuts.size(); iCut++) {
        KinematicCut Selection;
        Selection.LowerBound = PlotsToDraw[iHist].SelectionCuts[iCut].LowerBound;
        Selection.UpperBound = PlotsToDraw[iHist].SelectionCuts[iCut].UpperBound;

        if (mySamples[iPDF]->IsSubEventVarString(PlotsToDraw[iHist].SelectionCuts[iCut].ParamToCutOn)) {
          Selection.ParamToCutOnIt = mySamples[iPDF]->ReturnKinematicVectorFromString(PlotsToDraw[iHist].SelectionCuts[iCut].ParamToCutOn);
          SubEventSelectionVector.push_back(Selection);
        }
        else {
          Selection.ParamToCutOnIt = mySamples[iPDF]->ReturnKinematicParameterFromString(PlotsToDraw[iHist].SelectionCuts[iCut].ParamToCutOn);
          EventSelectionVector.push_back(Selection);
        }
      }

      if (histdim == 1) {
        Hist = (TH1*)mySamples[iPDF]->Get1DVarHist(PlotVar_Str[0], EventSelectionVector, SubEventSelectionVector, WeightStyle, &AxisX);
        Hist->GetYaxis()->SetTitle("Events");
      }
      else {
        Hist = (TH1*)mySamples[iPDF]->Get2DVarHist(PlotVar_Str[0], PlotVar_Str[1], EventSelectionVector, SubEventSelectionVector, WeightStyle, &AxisX, &AxisY);
        Hist->GetYaxis()->SetTitle(PlotVar_Str[1].c_str());
      }
      Canv->cd(1);
      Hist->SetTitle(PlotsToDraw[iHist].Name.c_str());
      Hist->GetXaxis()->SetTitle(PlotVar_Str[0].c_str());
      Hist->SetStats(false);
      Hist->Draw("COLZ");
      Canv->Print(OutputName);
      delete Hist;
    }
  }
  Canv->Print(OutputName+"]");
  delete Canv;
  return 0;
}
