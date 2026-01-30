// MaCh3 spline includes
#include "Utils/SplineMonoUtils.h"
#include "Utils/Comparison.h"
#include "SplinesTutorial/BinnedSplinesTutorial.h"

void SplineMonolithValidations(std::ostream& outFile) {
  MACH3LOG_INFO("Testing Spline Monolith");
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  std::string Input = TutorialPath + "/TutorialConfigs/MC/SplineFile.root";
  std::vector<std::string> Dials = {"Spline_0", "Spline_1", "Spline_2", "Spline_3"};
  std::vector<double> Dial_Values = {1.21, 1, 1, 1};
  std::vector< std::vector<TResponseFunction_red*> > MasterSpline = GetMasterSpline(Input, Dials);

  std::vector<RespFuncType> SplineType = {kTSpline3_red, kTSpline3_red, kTSpline3_red, kTSpline3_red};
  const unsigned int Nevents = MasterSpline.size();
  auto Splines = std::make_unique<SMonolith>(MasterSpline, SplineType, true);

  std::vector< const double* > splineParsPointer(Dials.size());
  for (unsigned int i = 0; i < Dials.size(); ++i) {
    splineParsPointer[i] = &Dial_Values[i];
  }
  Splines->setSplinePointers(splineParsPointer);

  Splines->Evaluate();

  //KS: If using CPU this does nothing, if on GPU need to make sure we finished copying memory from
  Splines->SynchroniseMemTransfer();

  for(unsigned int i = 0; i < Nevents; i++) {
    outFile << "Event " << i << " weight = " << Splines->cpu_total_weights[i] << std::endl;
  }

////// Testing Pre Computed Spline
  MACH3LOG_INFO("Testing Spline Monolith with Flattened ROOT inputs");

  auto SplinesFlat = std::make_unique<SMonolith>("SplineFile.root");
  for (unsigned int i = 0; i < Dials.size(); ++i) {
    splineParsPointer[i] = &Dial_Values[i];
  }
  SplinesFlat->setSplinePointers(splineParsPointer);

  SplinesFlat->Evaluate();

  //KS: If using CPU this does nothing, if on GPU need to make sure we finished copying memory from
  SplinesFlat->SynchroniseMemTransfer();
  for(unsigned int i = 0; i < Nevents; i++) {
    outFile << "(Flat) Event " << i << " weight = " << SplinesFlat->cpu_total_weights[i] << std::endl;
  }
}

void SplineBinnedValidations(std::ostream& outFile){
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  std::vector<std::string> xsecCovMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml"};
  auto ParHandler = std::make_unique<ParameterHandlerGeneric>(xsecCovMatrixFile, "xsec_cov");

  std::string ModeName = TutorialPath + "/TutorialConfigs/MaCh3Modes.yaml";
  auto Modes = std::make_unique<MaCh3Modes>(ModeName);

  auto SplineHandler = std::make_unique<BinnedSplineTutorial>(ParHandler.get(), Modes.get());


  std::vector<std::string> SampleTittles = {"FHC_1Rmu", "FHC_1Re", "RHC_1Rmu", "RHC_1Re"};
  std::vector<std::string> spline_filepaths = {"TutorialConfigs/MC/BinnedSplinesTutorialInputs2D.root"};
  std::vector<std::string> SplineVarNames = {"TrueNeutrinoEnergy", "TrueNeutrinoEnergy"};
  for(int i = 0; i < SampleTittles.size(); i++) {
    SplineHandler->AddSample("Tutorial_Blarb", SampleTittles[i], spline_filepaths, SplineVarNames);
  }
  SplineHandler->CountNumberOfLoadedSplines(false, 1);
  SplineHandler->TransferToMonolith();

  std::vector<const M3::float_t*> xsec_spline_pointers;

  constexpr int OscIndex = 0;
  constexpr int ModeIndex = 0;
  constexpr int TrueEnu = 0.6;
  auto EventSplines = SplineHandler->GetEventSplines("FHC_1Re", OscIndex, ModeIndex, TrueEnu, TrueEnu, 0.);

  xsec_spline_pointers.resize(EventSplines.size());
  for(size_t spline = 0; spline < xsec_spline_pointers.size(); spline++) {
    //Event Splines indexed as: sample name, oscillation channel, syst, mode, etrue, var1, var2 (var2 is a dummy 0 for 1D splines)
    xsec_spline_pointers[spline] = SplineHandler->retPointer(EventSplines[spline][0], EventSplines[spline][1],
                                                             EventSplines[spline][2], EventSplines[spline][3],
                                                             EventSplines[spline][4], EventSplines[spline][5],
                                                             EventSplines[spline][6]);

    outFile << "Binned Spline " << spline << " sample = " << EventSplines[spline][0] << std::endl;
  }
  SplineHandler->cleanUpMemory();
  SplineHandler->Evaluate();

  for(size_t iSpline = 0; iSpline < xsec_spline_pointers.size(); iSpline++) {
    outFile << "Binned Spline " << iSpline << " weight = " << *(xsec_spline_pointers[iSpline]) << std::endl;
  }
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: ", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  // Open a file in write mode
  std::ofstream outFile("NewSplineMonoOut.txt");

  SplineMonolithValidations(outFile);
  SplineBinnedValidations(outFile);

  outFile.close();
  bool TheSame = CompareTwoFiles(TutorialPath + "/CIValidations/TestOutputs/SplineMonoOut.txt", "NewSplineMonoOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different weights mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}
