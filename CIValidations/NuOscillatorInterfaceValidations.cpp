#include <Constants/OscillatorConstants.h>
#include "Utils/Comparison.h"

#include "Oscillator/OscillatorFactory.h"

#include "spdlog/fmt/bundled/core.h"

#include <cmath>
#include <fstream>

int main() {

  std::ofstream configf("NuOscillatorInterfaceValidations.yml");
  configf << "General:\n"
             "  Verbosity: \"NONE\"\n"
             "  CosineZIgnored: true\n"
             "  CalculationType: \"Unbinned\"\n"
             "\n"
             "OscProbCalcerSetup:\n"
             "  ImplementationName: \"NuFASTLinear\"\n"
             "  OscChannelMapping:\n"
             "    - Entry: \"Muon:Electron\"\n"
             "    - Entry: \"Muon:Muon\"\n";
  configf.close();

  auto osc = OscillatorFactory().CreateOscillator(
      "NuOscillatorInterfaceValidations.yml");

  std::vector<FLOAT_T> Energies;
  double start = -3;
  double end = 3;
  size_t nbins = 100;
  double lbw = (end - start) / double(nbins);
  for (size_t i = 0; i < nbins; i++) {
    double epower = start + i * lbw;
    Energies.push_back(std::pow(10.0, epower));
  }
  osc->SetEnergyArrayInCalcer(Energies);
  osc->Setup();

  std::vector<FLOAT_T> OscParProp = {0.3,      0.5, 0.020, 7.53e-5,
                                    2.494e-3, 0.0, 1300,  2.6, 0.5};
  osc->CalculateProbabilities(OscParProp);

  std::ofstream outFile("NewNuOscOut.txt");

  outFile << fmt::format("{:14}, {:14}, {:14}", "E (GeV)", "p(numu->numu)",
                         "p(numu->nue)\n");
  for (auto E : Energies) {
    double pdisp = osc->ReturnOscillationProbability(NuOscillator::kMuon,
                                                     NuOscillator::kMuon, E);
    double papp = osc->ReturnOscillationProbability(NuOscillator::kMuon,
                                                    NuOscillator::kElectron, E);
    outFile << fmt::format("{:14.7g}, {:14.7g}, {:14.7g}\n", E, pdisp, papp);
  }
  outFile.close();

  bool TheSame = CompareTwoFiles("CIValidations/TestOutputs/NuOscOut.txt",
                                 "NewNuOscOut.txt");

  if (!TheSame) {
    MACH3LOG_CRITICAL("Different osc mate");
    throw MaCh3Exception(__FILE__, __LINE__);
  } else {
    MACH3LOG_INFO("Everything is correct");
  }
}
