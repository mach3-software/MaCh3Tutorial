// MaCh3 includes
#include "api/plugin.hpp"
#include "Fitters/MaCh3Factory.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"
// #include "Mach3Plugins/MCMCTutorialPlugin.hpp"

namespace M3{

  class MCMCTutorialPlugin: public IPlugin{

    public:
      virtual ~MCMCTutorialPlugin(){
        if (m_parser) { delete m_parser; } 
      }
      MaCh3ArgumentParser* get_parser(){
        m_parser = new MaCh3ArgumentParser("tutorial", "1.0", argparse::default_arguments::help);
        m_parser->add_argument("config")
        .help("Config file.")
        .metavar("CONFIG")
        .required();
        m_parser->add_argument("--MCMCSteps")
        .scan<'d', int>()
        .help("specify the number of steps.");
        m_parser->add_argument("--override")
          .append()
          .help("specify any config overrides.");
        return m_parser;
      }

      int run(){
        std::string config = m_parser->get<std::string>("config");

        std::vector<std::string> args = { m_parser->name(), config };
        
        if (auto fn = m_parser->present<int>("--MCMCSteps")) {
            args.push_back("General:MCMC:NSteps:" + std::to_string(*fn));
        }
        
        std::vector<std::string> overrides = m_parser->get<std::vector<std::string>>("--override");
        for (const std::string& override_str : overrides) {
            args.push_back(override_str);
        }

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


    private:
      MaCh3ArgumentParser* m_parser;
  };

};

MACH3_REGISTER_PLUGIN(M3::MCMCTutorialPlugin)
