// MaCh3 includes
#include "MaCh3NuDock/NuDockServer.h"
#include "Manager/Manager.h"
#include "Fitters/MaCh3Factory.h"
#include "MaCh3NuDock/NuDockFactory.h"
#include "nudock.hpp"
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

  // Initialise covariance class reasonable for Systematics
  auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  auto serverObj = std::make_unique<NuDockServer>(FitManager.get());
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
    return serverObj->getLogLikelihood(request);
  });
  nudock_ptr->register_response("/get_parameter_names", [&serverObj](const nlohmann::json &request) {
    return serverObj->getParametersNames(request);
  });
  // nudock_ptr->register_response("/get_parameters", [&serverObj](const nlohmann::json &request) {
  //   return serverObj->getParameters(request);
  // });
  nudock_ptr->register_response("/set_parameters", [&serverObj](const nlohmann::json &request) {
    return serverObj->setParameters(request);
  });
  // nudock_ptr->register_response("/set_asimov_point", [&serverObj](const nlohmann::json &request) {
  //   return serverObj->setAsimovPoint(request);
  // });
  nudock_ptr->start_server();
  for (size_t i = 0; i < SampleConfig.size(); ++i) {
    delete mySamples[i];
  }
  return 0;
}
