// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "Utils/Comparison.h"


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
    MaCh3Fitter->Run2DLLHScan();
    MaCh3Fitter->GetStepScaleBasedOnLLHScan();
  }
  MaCh3Fitter->runMCMC();

  MaCh3Fitter.reset();
  delete xsec;
  //KS: Let's rename MCMC file so we can use it for some additional tests
  if(MoreTests)
  {
    std::string oldName = FitManager->raw()["General"]["OutputFile"].as<std::string>();
    std::string newName = "MCMC_Test.root";

    // Rename file
    if (std::rename(oldName.c_str(), newName.c_str()) != 0) {
      MACH3LOG_CRITICAL("Error renaming file");
      throw MaCh3Exception(__FILE__ , __LINE__ );
    }
  }
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
  //Algo.push_back("PSO");
  #endif

  bool MoreTests = true;
  for(unsigned int i = 0; i< Algo.size(); i++) {
   FitVal(Algo[i], MoreTests);
   MoreTests = false;
  }

  MACH3LOG_INFO("Everything seems correct");

  return 0;
}
