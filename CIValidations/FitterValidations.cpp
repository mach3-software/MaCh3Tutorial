// MaCh3 spline includes
#include "Fitters/MaCh3Factory.h"
#include "Parameters/ParameterHandlerGeneric.h"
#include "Utils/Comparison.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

void FitVal(const std::string& Algo, bool MoreTests)
{
  std::string ManagerInput = "TutorialConfigs/FitterConfig.yaml";
  auto FitManager = std::make_unique<manager>(ManagerInput);

  MACH3LOG_INFO("Testing {}", Algo);

  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");
  auto osc  = MaCh3CovarianceFactory<ParameterHandlerOsc>(FitManager.get(), "Osc");
  std::unique_ptr<FitterBase> MaCh3Fitter = nullptr;
  if(Algo == "MCMC") {
    FitManager->OverrideSettings("General", "OutputFile", "MCMC_Test.root");
    MaCh3Fitter = std::make_unique<mcmc>(FitManager.get());
  } else if (Algo == "PSO") {
    FitManager->OverrideSettings("General", "OutputFile", "PSO_Test.root");
    FitManager->OverrideSettings("General", "Fitter", "FitTestLikelihood", "true");
    MaCh3Fitter = std::make_unique<PSO>(FitManager.get());
  } else if (Algo == "Minuit2") {
    #ifdef MaCh3_MINUIT2
    FitManager->OverrideSettings("General", "OutputFile", "MINUIT2_Test.root");
    MaCh3Fitter = std::make_unique<MinuitFit>(FitManager.get());
    #else
    MACH3LOG_ERROR("Trying to use Minuit2 however MaCh3 was compiled without Minuit2 support");
    throw MaCh3Exception(__FILE__ , __LINE__ );
    #endif
  } else {
    MACH3LOG_ERROR("You want to use algorithm {}, I don't recognize it, sry", Algo);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::string SampleConfig = {"TutorialConfigs/Samples/SampleHandler_Tutorial.yaml"};
  auto Sample = std::make_unique<SampleHandlerTutorial>(SampleConfig, xsec.get(), osc.get());
  Sample->Reweight();
  std::string name = Sample->GetTitle();
  TString NameTString = TString(name.c_str());
  TH1D *SampleHistogramPrior = (TH1D*)Sample->Get1DHist()->Clone(NameTString+"_Prior");
  Sample->AddData(SampleHistogramPrior);

  MaCh3Fitter->AddSystObj(xsec.get());
  MaCh3Fitter->AddSampleHandler(Sample.get());
  if(MoreTests)
  {
    MaCh3Fitter->DragRace();
    MaCh3Fitter->RunLLHScan();
    MaCh3Fitter->Run2DLLHScan();
    MaCh3Fitter->GetStepScaleBasedOnLLHScan();
  }
  MaCh3Fitter->RunMCMC();
}

void StartFromPosteriorTest(const std::string& PreviousName)
{
  std::string ManagerInput = "TutorialConfigs/FitterConfig.yaml";
  auto FitManager = std::make_unique<manager>(ManagerInput);

  FitManager->OverrideSettings("General", "OutputFile", "MCMC_Test_Start.root");

  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");
  std::unique_ptr<mcmc> MarkovChain = std::make_unique<mcmc>(FitManager.get());
  MarkovChain->AddSystObj(xsec.get());

  MarkovChain->StartFromPreviousFit(PreviousName);

  MarkovChain->RunMCMC();
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();
  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::vector<std::string> Algo = {"MCMC"};
  #ifdef MaCh3_MINUIT2
  Algo.push_back("Minuit2");
  Algo.push_back("PSO");
  #endif

  bool MoreTests = true;
  for(unsigned int i = 0; i < Algo.size(); i++) {
   FitVal(Algo[i], MoreTests);
   MoreTests = false;
  }

  StartFromPosteriorTest("MCMC_Test.root");

  MACH3LOG_INFO("Everything seems correct");

  return 0;
}
