#pragma once
#include "api/plugin.hpp"

namespace mach3{

  class MCMCTutorialPlugin: public IPlugin{

    public:
      virtual ~MCMCTutorialPlugin(){
        if (m_parser) { delete m_parser; } 
      }
      MaCh3ArgumentParser* get_parser() override;
      int run() override;


    private:
      MaCh3ArgumentParser* m_parser;
  };
};
