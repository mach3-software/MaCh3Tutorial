// MaCh3 includes
#include "Manager/MaCh3Logger.h"
#include "Mach3Plugins/MCMCTutorialPlugin.hpp"

int main(int argc, char const* argv[]) {
    MACH3LOG_WARN("Deprecation Warning: Use of the standalone executable will be deprecated in future releases.");
    MACH3LOG_WARN("                   : you can use 'mach3 tutorial' as a direct replacement instead.");
    argv[0] = "tutorial";
    M3::MCMCTutorialPlugin tutorial;
    ArgumentParser* parser = tutorial.get_parser();
    parser->parse_args(argc, argv);
    tutorial.Run();
    MACH3LOG_WARN("Deprecation Warning: Use of the standalone executable will be deprecated in future releases.");
    MACH3LOG_WARN("                   : you can use 'mach3 tutorial' as a direct replacement instead.");
    return 0;
}
