// MaCh3 spline includes
#include "Utils/SplineMonoUtils.h"
#include "Utils/Comparison.h"

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  std::string Input = "Inputs/SplineFile.root";
  std::vector<std::string> Dials = {"Spline_0", "Spline_1", "Spline_2", "Spline_3"};
  std::vector<double> Dial_Values = {1.21, 1, 1, 1};
  std::vector< std::vector<TSpline3_red*> > MasterSpline = GetMasterSpline(Input, Dials);
  const unsigned int Nevents = MasterSpline.size();
  SMonolith* Splines = new SMonolith(MasterSpline);

  std::vector< const double* > splineParsPointer(Dials.size());
  for (unsigned int i = 0; i < Dials.size(); ++i) {
    splineParsPointer[i] = &Dial_Values[i];
  }
  Splines->setSplinePointers(splineParsPointer);

  Splines->Evaluate();

  //KS: If using CPU this does nothing, if on GPU need to make sure we finished copying memory from
  Splines->SynchroniseMemTransfer();

  // Open a file in write mode
  std::ofstream outFile("NewSplineMonoOut.txt");
  for(unsigned int i = 0; i < Nevents; i++) {
    outFile << "Event " << i << " weight = " << Splines->cpu_total_weights[i] << std::endl;
  }
  outFile.close();
  delete Splines;

  bool TheSame = CompareTwoFiles("Inputs/SplineMonoOut.txt", "NewSplineMonoOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different weights mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }
  return 0;
}
