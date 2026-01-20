// MaCh3 spline includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"
#include "MaCh3NuDock/SampleHandlerNuDock.h"

int main(int argc, char *argv[]){
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  std::string OutputName = "LLH_" + FitManager->raw()["General"]["OutputFile"].as<std::string>();
  FitManager->OverrideSettings("General", "OutputFile", OutputName);
  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  // Initialise NuDock samples
  auto NuDockSampleConfigs = FitManager->raw()["General"]["NuDockSamples"].as<std::vector<std::string>>();
  bool useNuDock = GetFromManager(FitManager->raw()["General"]["UseNuDock"], false, __FILE__, __LINE__);
  std::vector<SampleHandlerNuDock*> samplesNuDock;
  if (useNuDock){
    for(size_t Sample_i = 0; Sample_i < NuDockSampleConfigs.size(); Sample_i++){
      samplesNuDock.push_back(new SampleHandlerNuDock(NuDockSampleConfigs[Sample_i].c_str(), xsec.get())); 
    }
  }

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(xsec.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    MaCh3Fitter->AddSampleHandler(mySamples[i]);
  }
  if (useNuDock) {
    for(size_t i = 0; i < NuDockSampleConfigs.size(); i++){
      MaCh3Fitter->AddSampleHandler(samplesNuDock[i]);
    }
  }
  // Run LLH scan
  MaCh3Fitter->RunLLHScan();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  for(size_t i = 0; i < NuDockSampleConfigs.size(); i++){
    delete samplesNuDock[i];
  }
  return 0;
}
