// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"
#include "MaCh3Plugins/MCMCTutorialPlugin.hpp"

MACH3_REGISTER_PLUGIN(mach3::MCMCTutorialPlugin)

namespace mach3{
      MaCh3ArgumentParser* MCMCTutorialPlugin::get_parser(){
        m_parser = new MaCh3ArgumentParser("tutorial", "1.0", argparse::default_arguments::help);
        m_parser->add_argument("config")
        .help("Config file.")
        .metavar("CONFIG")
        .required();
        // m_parser->add_argument("root-file")
        // .help("Root file.")
        // .metavar("ROOT_FILE")
        // .required();

        return m_parser;
      }
      int MCMCTutorialPlugin::run(){
        std::string config = m_parser->get<std::string>("config");
        // std::string inputFile = m_parser->get<std::string>("root-file");

        std::vector<std::string> args = { m_parser->name(), config };//, inputFile };

        // Convert to char* array
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (auto& s : args)
            argv.push_back(s.data());

        // Initialise manger responsible for config handling
        auto FitManager = MaCh3ManagerFactory(argv.size(), argv.data());

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
        // Run MCMCM
        MaCh3Fitter->RunMCMC();

        for (size_t i = 0; i < SampleConfig.size(); ++i) {
            delete mySamples[i];
        }
        return 0;
      }



};
