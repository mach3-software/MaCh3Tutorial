// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "Utils/Comparison.h"

// TODO add also Adaption and PCA matrix
// TODO add maybe different algorithms?


void FitVal(const std::string& Algo, bool MoreTests)
{
  std::string ManagerInput = "Inputs/ManagerTest.yaml";
  manager *FitManager = new manager(ManagerInput);

  MACH3LOG_INFO("Testing {}", Algo);

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicsTest.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::unique_ptr<FitterBase> MaCh3Fitter = nullptr;
  if(Algo == "MCMC") {
    MaCh3Fitter = std::make_unique<mcmc>(FitManager);
  } else if (Algo == "PSO") {
    MaCh3Fitter = std::make_unique<PSO>(FitManager);
  } else if (Algo == "Minuit2") {
    #ifdef MaCh3_MINUIT2
    MaCh3Fitter = std::make_unique<MinuitFit>(FitManager);
    #else
    MACH3LOG_ERROR("Trying to use Minuit2 however MaCh3 was compiled without Minuit2 support");
    throw MaCh3Exception(__FILE__ , __LINE__ );
    #endif
  } else {
    MACH3LOG_ERROR("You want to use algorithm {}, I don't recognize it, sry", Algo);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  MaCh3Fitter->addSystObj(xsec);

  if(MoreTests)
  {
    MaCh3Fitter->DragRace();
    MaCh3Fitter->RunLLHScan();
  }
  MaCh3Fitter->runMCMC();

  if(MoreTests)
  {
    MCMCProcessor* Processor = new MCMCProcessor(FitManager->raw()["General"]["OutputFile"].as<std::string>());
    Processor->Initialise();

    // Make the postfit
    Processor->MakePostfit();
    Processor->DrawPostfit();

    Processor->CacheSteps();
    //KS: Since we cached let's make fancy violins :)
    Processor->MakeViolin();
    Processor->MakeCovariance_MP();
    Processor->DrawCovariance();
    delete Processor;
  }

  delete FitManager;
  delete xsec;
  MaCh3Fitter.reset();
}

int main(int argc, char *argv[])
{
  std::vector<std::string> Algo = {"MCMC"};
  #ifdef MaCh3_MINUIT2
  Algo.push_back("Minuit2");
  #endif

  bool MoreTests = true;
  for(unsigned int i = 0; i< Algo.size(); i++) {
   FitVal(Algo[i], MoreTests);
   MoreTests = false;
  }

  MACH3LOG_INFO("Everything seems correct");

  return 0;
}
