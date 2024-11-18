// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "Utils/Comparison.h"
#include "samplePDF/samplePDFTutorial.h"

/// @todo add sample PDF object!
///
///
/// Current tests include
/// Running MCMC
/// Running MinuitFit
/// Running LLH scan and 2D LLH scan
/// Running Drag Race
/// Running GetStepScaleBasedOnLLHScan
void FitVal(const std::string& Algo, bool MoreTests)
{
  std::string ManagerInput = "Inputs/ManagerTest.yaml";
  manager *FitManager = new manager(ManagerInput);

  MACH3LOG_INFO("Testing {}", Algo);

  covarianceXsec* xsec = MaCh3CovarianceFactory(FitManager, "Xsec");
  covarianceOsc* osc = MaCh3CovarianceFactory<covarianceOsc>(FitManager, "Osc");
  std::unique_ptr<FitterBase> MaCh3Fitter = nullptr;
  if(Algo == "MCMC") {
    FitManager->OverrideSettings("General", "OutputFile", "MCMC_Test.root");
    MaCh3Fitter = std::make_unique<mcmc>(FitManager);
  } else if (Algo == "PSO") {
    FitManager->OverrideSettings("General", "OutputFile", "PSO_Test.root");
    FitManager->OverrideSettings("General", "Fitter", "FitTestLikelihood", "true");
    MaCh3Fitter = std::make_unique<PSO>(FitManager);
  } else if (Algo == "Minuit2") {
    #ifdef MaCh3_MINUIT2
    FitManager->OverrideSettings("General", "OutputFile", "MINUIT2_Test.root");
    MaCh3Fitter = std::make_unique<MinuitFit>(FitManager);
    #else
    MACH3LOG_ERROR("Trying to use Minuit2 however MaCh3 was compiled without Minuit2 support");
    throw MaCh3Exception(__FILE__ , __LINE__ );
    #endif
  } else {
    MACH3LOG_ERROR("You want to use algorithm {}, I don't recognize it, sry", Algo);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::string SampleConfig = {"Inputs/SamplePDF_Tutorial.yaml"};
  samplePDFTutorial *Sample = new samplePDFTutorial(SampleConfig, xsec);
  Sample->SetXsecCov(xsec);
  Sample->SetOscCov(osc);
  Sample->reweight();
  std::string name = Sample->GetName();
  TString NameTString = TString(name.c_str());
  TH1D *SampleHistogramPrior = (TH1D*)Sample->get1DHist()->Clone(NameTString+"_Prior");
  Sample->addData(SampleHistogramPrior);

  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSamplePDF(Sample);
  if(MoreTests)
  {
    MaCh3Fitter->DragRace();
    MaCh3Fitter->RunLLHScan();
    MaCh3Fitter->Run2DLLHScan();
    MaCh3Fitter->GetStepScaleBasedOnLLHScan();
  }
  MaCh3Fitter->runMCMC();

  MaCh3Fitter.reset();
  delete xsec;
  delete osc;
  delete FitManager;
  delete Sample;
}

void StartFromPosteriorTest(const std::string& PreviousName)
{
  std::string ManagerInput = "Inputs/ManagerTest.yaml";
  manager *FitManager = new manager(ManagerInput);

  FitManager->OverrideSettings("General", "OutputFile", "MCMC_Test_Start.root");

  covarianceXsec* xsec = MaCh3CovarianceFactory(FitManager, "Xsec");
  std::unique_ptr<mcmc> MarkovChain = std::make_unique<mcmc>(FitManager);
  MarkovChain->addSystObj(xsec);

  MarkovChain->StartFromPreviousFit(PreviousName);

  MarkovChain->runMCMC();

  MarkovChain.reset();
  delete xsec;
  delete FitManager;
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
  for(unsigned int i = 0; i< Algo.size(); i++) {
   FitVal(Algo[i], MoreTests);
   MoreTests = false;
  }

  StartFromPosteriorTest("MCMC_Test.root");

  MACH3LOG_INFO("Everything seems correct");

  return 0;
}
