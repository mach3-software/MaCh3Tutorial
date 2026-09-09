// MaCh3 spline includes
#include "Algorithms/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"

int main(int argc, char *argv[]){

  std::string filename = "";
  // Initialise manger responsible for config handling
  std::unique_ptr<Manager> FitManager;
  if (argc == 2)
    FitManager = MaCh3ManagerFactory(argc, argv);
  else
    FitManager = MaCh3ManagerFactory(argc - 1, argv);

  if (argc > 2)
    filename = argv[2];

  std::string OutputName = "LLH_" + FitManager->raw()["General"]["OutputFile"].as<std::string>();
  
  FitManager->OverrideSettings("General", "OutputFile", OutputName);
  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(xsec.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->AddSampleHandler(mySamples[i]);
  }
  // Run Step Scale Tuning from LLH scan
  MaCh3Fitter->GetStepScaleBasedOnLLHScan(filename);
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
