// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "Samples/BinningHandler.h"
#include "Samples/HistogramUtils.h"

std::unique_ptr<TH2Poly> MakeDummyTH2Poly() {
  auto poly = std::make_unique<TH2Poly>();

  std::vector<double> BinArray_x = {0., 300., 400., 500., 600., 650., 700., 750., 800., 900., 1000.};
  std::vector<double> BinArray_y = {-1.0, 0.6, 0.7, 0.8, 0.9, 0.95, 1.0};

  for(size_t iy = 0; iy < BinArray_y.size()-1; iy++)
  {
    double ymax = BinArray_y[iy+1];
    double ymin = BinArray_y[iy];
    for(size_t ix = 0; ix < BinArray_x.size()-1; ix++)
    {
      double xmax = BinArray_x[ix+1];
      double xmin = BinArray_x[ix];
      double binofx[] = {xmin, xmax, xmax, xmin};
      double binofy[] = {ymin, ymin, ymax, ymax};
      poly->AddBin(4,binofx,binofy);
    }
  }

  return poly;
}

void BinningHandlerValidationsNonUniform(std::ostream& outFile) {
  auto Binning = std::make_unique<BinningHandler>();
  std::string yamlBinning = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2"]
Bins: [[[0.0, 0.1], [0.0, 0.1]], [[0.0, 0.1], [0.1, 0.2]], [[0.0, 0.1], [0.2, 0.3]],
        [[0.0, 0.1], [0.3, 0.4]], [[0.0, 0.1], [0.4, 0.5]], [[0.0, 0.1], [0.5, 0.6]],
        [[0.0, 0.1], [0.6, 0.7]], [[0.0, 0.1], [0.7, 0.8]], [[0.0, 0.1], [0.8, 0.9]],
        [[0.0, 0.1], [0.9, 1.0]] ]
Uniform: false
)";
  YAML::Node Config = STRINGtoYAML(yamlBinning);
  SampleInfo SingleSample;
  Binning->SetupSampleBinning(Config, SingleSample);


    std::string yamlBinning3D = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2", "CosTheta"]
Bins: [
    [[0.0, 0.5], [0.0, 0.5], [0.0, 0.5]],   # bin 0
    [[0.0, 0.5], [0.5, 1.0], [0.0, 0.5]],   # bin 1
    [[0.5, 1.0], [0.0, 0.5], [0.0, 0.5]],   # bin 2
    [[0.5, 1.0], [0.5, 1.0], [0.0, 0.5]],   # bin 3
    [[0.0, 0.5], [0.0, 0.5], [0.5, 1.0]],   # bin 4
    [[0.0, 0.5], [0.5, 1.0], [0.5, 1.0]],   # bin 5
    [[0.5, 1.0], [0.0, 0.5], [0.5, 1.0]],   # bin 6
    [[0.5, 1.0], [0.5, 1.0], [0.5, 1.0]]    # bin 7
]
Uniform: false
)";
  YAML::Node Config3D = STRINGtoYAML(yamlBinning3D);
  Binning->SetupSampleBinning(Config3D, SingleSample);

  std::vector<int> Samples = {0,1};
  std::vector<double> XVars = {-0.05, 0.0, 0.05, 0.1, 0.15};
  std::vector<double> YVars = {-0.1, 0.0, 0.05, 0.1, 0.15, 0.95, 1.0, 1.05};
  std::vector<int> NomXBins = {-1};
  std::vector<int> NomYBins = {-1};
  // Loop over all combinations
  for (int sample : Samples) {
    for (double xvar : XVars) {
      for (double yvar : YVars) {
        for (int nomXBin : NomXBins) {
          for (int nomYBin : NomYBins) {
            std::vector<const double*> KinVar;
            std::vector<int> NomBin;

            if(sample == 0) {
              KinVar = {&xvar, &yvar};
              NomBin = {nomXBin, nomYBin};
            } else if (sample == 1) {

              KinVar = {&xvar, &yvar, &xvar};
              NomBin = {nomXBin, nomYBin, nomXBin};
            }

            const int GlobalBin = Binning->FindGlobalBin(sample, KinVar, NomBin);
            outFile << "Sample " << sample
            << ", XVar: " << xvar
            << ", YVar: " << yvar
            << ", NomXBin: " << nomXBin
            << ", NomYBin: " << nomYBin
            << ", GlobalBin: " << GlobalBin;

            if (GlobalBin == M3::UnderOverFlowBin) {
              outFile << " (Under/Overflow)";
            } else {
              outFile << ", Name: " << Binning->GetBinName(GlobalBin);
            }

            outFile << '\n';
          }
        }
      }
    }
  }

/////////////Check Loading From External///////////////
  auto Poly = MakeDummyTH2Poly();
  auto BinningYaml = M3::PolyToYaml(Poly.get(), "Sample", __FILE__, __LINE__);
  auto String = YAMLtoSTRING(BinningYaml);
  std::ofstream out("binningTest.yaml");
  out << String;
  out.close();

  std::string yamlBinningExternal = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2"]
Bins:
  File: binningTest.yaml
  Key: Sample
Uniform: false
)";
  YAML::Node ConfigExternal = STRINGtoYAML(yamlBinningExternal);
  Binning->SetupSampleBinning(ConfigExternal, SingleSample);
}


void BinningHandlerValidations(std::ostream& outFile) {
  auto Binning = std::make_unique<BinningHandler>();

std::string yamlBinning1 = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2"]
VarBins: [ [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1],
           [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1] ]
Uniform: true
)";

  YAML::Node Binning1 = STRINGtoYAML(yamlBinning1);
  SampleInfo SingleSample;
  Binning->SetupSampleBinning(Binning1, SingleSample);
///////
  MACH3LOG_INFO("");
std::string yamlContent = R"(
VarStr : ["RecoNeutrinoEnergy"]
VarBins: [[0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]]
Uniform: true
)";

  YAML::Node Binning2 = STRINGtoYAML(yamlContent);
  Binning->SetupSampleBinning(Binning2, SingleSample);
///////
  MACH3LOG_INFO("");
std::string yamlContent3 = R"(
VarStr : ["RecoNeutrinoEnergy", "TrueQ2", "Wacky"]
VarBins: [ [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1],
           [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1],
           [-1., 0, 1.] ]
Uniform: true
)";

  YAML::Node Binning3 = STRINGtoYAML(yamlContent3);
  Binning->SetupSampleBinning(Binning3, SingleSample);

  std::vector<int> Samples = {0, 1};
  std::vector<double> XVars = {-1, 0, 0.5, 0.7, 1, 10};
  std::vector<double> YVars = {0.05, 0.15, 0.25, 0.35};
  std::vector<int> NomXBins = {0, 1, 2, 3};
  std::vector<int> NomYBins = {0, 1, 2, 3};
  // Loop over all combinations
  for (int sample : Samples) {
    for (double xvar : XVars) {
      for (double yvar : YVars) {
        for (int nomXBin : NomXBins) {
          for (int nomYBin : NomYBins) {
            std::vector<const double*> KinVar;
            std::vector<int> NomBin;
            if(sample == 0) {
              KinVar = {&xvar, &yvar};
              NomBin = {nomXBin, nomYBin};
            } else if (sample == 1){
              KinVar = {&xvar};
              NomBin = {nomXBin};
            }

            const int GlobalBin = Binning->FindGlobalBin(sample, KinVar, NomBin);
            outFile << "Sample " << sample
                    << ", XVar: " << xvar
                    << ", NomXBin: " << nomXBin
                    << ", NomYBin: " << nomYBin
                    << ", GlobalBin: " << GlobalBin
                    << std::endl;
          }
        }
      }
    }
  }

  for(int iSam = 0; iSam <  Samples.size(); iSam++) {
    for (int iBin = 0; iBin < Binning->GetNBins(iSam); iBin++ ) {
      outFile << "Sample " << iSam
              << ", bin: " << iBin
              << ", name: " << Binning->GetBinName(iSam, iBin)
              << std::endl;
    }
  }

  MACH3LOG_INFO("");
  std::string yamlContent_implicit1D = R"(
  VarStr : [ "x" ]
  BinEdges: [ 0,1,2,3 ]
  Uniform: true
  )";

  YAML::Node Binning_implicit1D = STRINGtoYAML(yamlContent_implicit1D);
  Binning->SetupSampleBinning(Binning_implicit1D, SingleSample);

  auto fmtvector = [](std::vector<double> const &v) {
      std::stringstream ss;
      ss << "[";
      for (size_t i = 0; i < v.size(); ++i) {
          ss << fmt::format("{:.3g}", v[i]);
          if (i != v.size() - 1) {
              ss << ", ";
          }
      }
      ss << "]";
      return ss.str();
  };

  outFile << "[Binning_implicit1D]: BinEdges: " << fmtvector(Binning->GetBinEdges(3,0)) << std::endl;

  MACH3LOG_INFO("");
  std::string yamlContent_implicit1DRange = R"(
  VarStr : [ "x" ]
  BinEdges: { linspace: { nb: 10, low: 0, up: 10 } }
  Uniform: true
  )";

  YAML::Node Binning_implicit1DRange = STRINGtoYAML(yamlContent_implicit1DRange);
  Binning->SetupSampleBinning(Binning_implicit1DRange, SingleSample);

  outFile << "[Binning_implicit1DRange]: BinEdges: " << fmtvector(Binning->GetBinEdges(4,0)) << std::endl;

  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRange = R"(
  VarStr : [ "x" ]
  BinEdges: [ { linspace: { nb: 10, low: 0, up: 10 } },
              { linspace: { nb: 10, low: 10, up: 11 } } ]
  Uniform: true
  )";

  YAML::Node Binning_1DRangeRange = STRINGtoYAML(yamlContent_1DRangeRange);
  Binning->SetupSampleBinning(Binning_1DRangeRange, SingleSample);

  outFile << "[Binning_1DRangeRange]: BinEdges: " << fmtvector(Binning->GetBinEdges(5,0)) << std::endl;

  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeScalar = R"(
  VarStr : [ "x" ]
  BinEdges: [ -1, { linspace: { nb: 10, low: 0, up: 10 } },
              { linspace: { nb: 10, low: 10, up: 11 } }, 100 ]
  Uniform: true
  )";

  YAML::Node Binning_1DRangeRangeScalar = STRINGtoYAML(yamlContent_1DRangeRangeScalar);
  Binning->SetupSampleBinning(Binning_1DRangeRangeScalar, SingleSample);

  outFile << "[Binning_1DRangeRangeScalar]: BinEdges: " << fmtvector(Binning->GetBinEdges(6,0)) << std::endl;

  MACH3LOG_INFO("");
  std::string yamlContent_2DRangeRange = R"(
  VarStr : [ "x", "y" ]
  BinEdges: [ [ { linspace: { nb: 10, low: 0, up: 10 } } ],
              [ { linspace: { nb: 10, low: 0, up: 100 } }  ] ]
  Uniform: true
  )";

  YAML::Node Binning_2DRangeRange = STRINGtoYAML(yamlContent_2DRangeRange);
  Binning->SetupSampleBinning(Binning_2DRangeRange, SingleSample);

  outFile << "[Binning_2DRangeRange]: BinEdges[0]: " << fmtvector(Binning->GetBinEdges(7,0)) << std::endl;
  outFile << "[Binning_2DRangeRange]: BinEdges[1]: " << fmtvector(Binning->GetBinEdges(7,1)) << std::endl;
}


void UniformValidations(std::ostream& outFile) {
  auto Binning = std::make_unique<SampleBinningInfo>();
  std::vector<std::vector<double>> Edges = { {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1},
  {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1} };
  Binning->InitUniform(Edges);
  std::vector<std::pair<double, int>> testPairs = {
    {-1.0, -1},
    {-1.0, 0},
    {0.0, 0},
    {1.0, 9},

    {0.5, 1},
    {0.5, 2},
    {0.5, 3},
    {0.5, 4},
    {0.5, 5},
    {0.5, 6},
    {0.5, 7},
    {0.5, 8},
    {0.5, 9},

    {0.65, 1},
    {0.65, 2},
    {0.65, 3},
    {0.65, 4},
    {0.65, 5},
    {0.65, 6},
    {0.65, 7},
    {0.65, 8},
    {0.65, 9},

    {10.0, 4},
    {10.0, 5},
    {10.0, 6},

    {-10.0, 0},
  };

  for (const auto& [XVar, NomXBin] : testPairs) {
    constexpr int dim = 0; // XVar is dimension 0
    int binIndex = Binning->FindBin(dim, XVar, NomXBin);
    outFile << "XVar: " << XVar << ", NomXBin: " << NomXBin << ", Bin: " << binIndex << std::endl;
  }

  for (int yBin = 0; yBin < Binning->AxisNBins[1]; ++yBin) {
    for (int xBin = 0; xBin < Binning->AxisNBins[0]; ++xBin) {
      const int binIndex = Binning->GetBinSafe({xBin, yBin});
      outFile << "yBin: " << yBin << ", xBin: " << xBin << ", Bin: " << binIndex << std::endl;

    }
  }
}

void NonUniformValidations(std::ostream& outFile) {
  auto Binning = std::make_unique<SampleBinningInfo>();
  std::vector<std::vector<std::vector<double>>> Bins = {
    {{0.0, 0.1}, {0.0, 0.1}},
    {{0.0, 0.1}, {0.1, 0.2}},
    {{0.0, 0.1}, {0.2, 0.3}},
    {{0.0, 0.1}, {0.3, 0.4}},
    {{0.0, 0.1}, {0.4, 0.5}},
    {{0.0, 0.1}, {0.5, 0.6}},
    {{0.0, 0.1}, {0.6, 0.7}},
    {{0.0, 0.1}, {0.7, 0.8}},
    {{0.0, 0.1}, {0.8, 0.9}},
    {{0.0, 0.1}, {0.9, 1.0}},
  };
  Binning->InitNonUniform(Bins);
}

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  // Open a file in write mode
  std::ofstream outFile("NewBinningOut.txt");
////////////////////////////
  UniformValidations(outFile);
  NonUniformValidations(outFile);
  BinningHandlerValidations(outFile);
  BinningHandlerValidationsNonUniform(outFile);
////////////// The End //////////////
  outFile.close();
  bool TheSame = CompareTwoFiles(TutorialPath + + "/CIValidations/TestOutputs/BinningOut.txt", "NewBinningOut.txt");

  if(!TheSame) {
    MACH3LOG_CRITICAL("Different likelihood mate");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  } else {
    MACH3LOG_INFO("Everything is correct");
  }

  return 0;
}

