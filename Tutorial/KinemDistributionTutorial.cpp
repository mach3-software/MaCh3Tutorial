// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

int main(int argc, char **argv) {
  int Selection = 0; //0 to loop modes, 1 to loop osc channels

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
  if(vecParams.size() == 2)
  {
    for (size_t iPDF = 0;iPDF < mySamples.size(); iPDF++) {
      TH2D* Hist = static_cast<TH2D*>(mySamples[iPDF]->Get2DVarHist(vecParams[0], vecParams[1]));

      Canv->cd(1);
      Hist->SetTitle(mySamples[iPDF]->GetTitle().c_str());
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
