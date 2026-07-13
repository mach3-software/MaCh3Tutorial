#include "CLI/API/plugin.hpp"

namespace M3{

  class MCMCTutorialPlugin: public PluginBase{

    public:
      virtual ~MCMCTutorialPlugin() = default;

      MaCh3ArgumentParser* get_parser();

      int Run();

  };

};

MACH3_REGISTER_PLUGIN(M3::MCMCTutorialPlugin)
