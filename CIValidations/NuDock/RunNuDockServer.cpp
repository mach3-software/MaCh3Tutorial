// MaCh3 includes
#include "Manager/Manager.h"
#include "Fitters/MaCh3Factory.h"

#include "NuDock/NuDockFactory.h"
#include "NuDock/NuDockServerBase.h"

#include "SamplesTutorial/SampleHandlerTutorial.h"

nlohmann::json ping(nlohmann::json _request) 
{
  std::cout << "Received request from client: " << _request.dump() << std::endl;
  nlohmann::json response;
  // response["message"] = "ping";
  response="pong";
  return response;
};

int main(int argc, char **argv){
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  FitManager->OverrideSettings("NuDock", "Debug", false);
  FitManager->OverrideSettings("NuDock", "Verbose", false);
  FitManager->OverrideSettings("NuDock", "AddPriorLLH", false);
  FitManager->OverrideSettings("NuDock", "NuDockVerbosity", "INFO");
  FitManager->OverrideSettings("NuDock", "CommunicationType", "UNIX_DOMAIN_SOCKET");
  FitManager->OverrideSettings("NuDock", "Port", 50051);

  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  auto serverObj = std::make_unique<NuDockServerBase>(FitManager.get());
  // Add covariance to MCM
  serverObj->AddSystObj(xsec.get());
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    serverObj->AddSampleHandler(mySamples[i]);
  }
  serverObj->setup();

  MACH3LOG_INFO("Testing log_likelihood calculation...");
  MACH3LOG_INFO("Initial log_likelihood: {}", serverObj->getLogLikelihood());

  MACH3LOG_INFO("Starting NuDock server...");
  std::unique_ptr<NuDock> nudock_ptr = nullptr;
  InitialiseNuDockObj(FitManager.get(), nudock_ptr);
  nudock_ptr->register_response("/ping", std::bind(ping, std::placeholders::_1));
  nudock_ptr->register_response("/log_likelihood", [&serverObj](const nlohmann::json &request) {
    return serverObj->getLogLikelihood();
  });

  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
