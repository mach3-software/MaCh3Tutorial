// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

int main(int argc, char *argv[]) {
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);

  // Initialise covariance class reasonable for Systematics
  auto myParams = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, myParams.get());


  //####################################################################################
  MACH3LOG_INFO("-------------------------------------------------------------------");
  MACH3LOG_INFO("Grouping oscillation channels\n");

  std::vector<double> oscpars_un(myParams->GetNumParFromGroup("Osc"));
  int idx = 0;
  for (int i = 0; i < myParams->GetNParameters(); ++i) {
    if (myParams->IsParFromGroup(i, "Osc")) {
      oscpars_un[idx] = static_cast<double>(myParams->GetParProp(i));
      ++idx;
    }
  }
  auto oscpars = oscpars_un;
  for (int i = 0;i < 3; i++) {
    oscpars_un[i] = 0.;
  }

  std::vector<int> NeutrinoOscChan = {0, 1, 4};
  std::vector<int> AntiNeutrinoOscChan = {2, 3, 5};
  std::vector<std::vector<int>> OscChanBreakdown = {NeutrinoOscChan, AntiNeutrinoOscChan};
  std::vector<TString> OscChanBreakdown_Name = {"#nu", "#bar{#nu}"};

  if (OscChanBreakdown.size()!=OscChanBreakdown_Name.size()) {
    MACH3LOG_ERROR("Mismatch in Oscillation Channel sizing - Quitting");
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  unsigned int nOscChanBreakdown = OscChanBreakdown.size();

  for (unsigned int iOscChanBreakdown=0;iOscChanBreakdown<OscChanBreakdown.size();iOscChanBreakdown++) {
    std::string oscchan_logger = fmt::format("Grouping following oscillation channels as {:<10}:", OscChanBreakdown_Name[iOscChanBreakdown]);
    for (unsigned int iOscChan=0;iOscChan<OscChanBreakdown[iOscChanBreakdown].size();iOscChan++) {
      oscchan_logger += fmt::format("{} ", OscChanBreakdown[iOscChanBreakdown][iOscChan]);
    }
    MACH3LOG_INFO("{}",oscchan_logger);
  }
  std::vector<std::vector<TString>> PDFNames(mySamples.size());
  std::vector<std::vector<std::unique_ptr<TH1>>> UnOscillated_Hist(mySamples.size());
  std::vector<std::vector<std::unique_ptr<TH1>>> Oscillated_Hist(mySamples.size());

  for (unsigned int iPDF = 0; iPDF < mySamples.size(); iPDF++) {
    PDFNames[iPDF].resize(mySamples[iPDF]->GetNSamples());

    UnOscillated_Hist[iPDF].resize(mySamples[iPDF]->GetNSamples());
    Oscillated_Hist[iPDF].resize(mySamples[iPDF]->GetNSamples());
    for (int iSample = 0; iSample < mySamples[iPDF]->GetNSamples(); ++iSample) {
      PDFNames[iPDF][iSample] = mySamples[iPDF]->GetSampleTitle(iSample);

      myParams->SetGroupOnlyParameters("Osc", oscpars_un);
      mySamples[iPDF]->Reweight();
      UnOscillated_Hist[iPDF][iSample] = M3::Clone(mySamples[iPDF]->GetMCHist(iSample));

      myParams->SetGroupOnlyParameters("Osc", oscpars);
      mySamples[iPDF]->Reweight();

      Oscillated_Hist[iPDF][iSample] = M3::Clone(mySamples[iPDF]->GetMCHist(iSample));
    }
  }

  // Print event rates for all tutorial samples
  MACH3LOG_INFO("┌─────────────────────────┬────────────────────┬────────────────────┐");
  MACH3LOG_INFO("│{0:25}│{1:20}│{2:20}│", "Sample", "Oscillated", "UnOscillated");
  MACH3LOG_INFO("├─────────────────────────┼────────────────────┼────────────────────┤");

  for (unsigned int iPDF = 0; iPDF < mySamples.size(); iPDF++) {
    for (int iSample = 0; iSample < mySamples[iPDF]->GetNSamples(); ++iSample) {
      MACH3LOG_INFO("│{: <25}│{: <20.4f}│{: <20.4f}│", PDFNames[iPDF][iSample], Oscillated_Hist[iPDF][iSample]->Integral(), UnOscillated_Hist[iPDF][iSample]->Integral());
    }
  }

  MACH3LOG_INFO("└─────────────────────────┴────────────────────┴────────────────────┘");
  return 0;
}
