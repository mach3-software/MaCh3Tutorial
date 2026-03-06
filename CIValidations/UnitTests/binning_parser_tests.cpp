#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include "Samples/BinningHandler.h"

TEST_CASE("Parse 1D implicit outerbrace", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : x
  BinEdges: [0,1,2,3]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {0, 1, 2, 3};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Parse 1D explicit outerbrace", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x ]
  BinEdges: [ [0,1,2,3] ]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {0, 1, 2, 3};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Parse 2D scalar binning", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x, y ]
  BinEdges: [ [0,1,2,3], [4,5,6,7] ]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref_dim0 = {0, 1, 2, 3};
  std::vector<double> ref_dim1 = {4,5,6,7};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  std::vector<double> parsed_dim1 = Binning->GetBinEdges(0, 1);
  REQUIRE(Binning->GetNDim(0) == 2);
  REQUIRE(ref_dim0.size() == parsed_dim0.size());
  REQUIRE(ref_dim1.size() == parsed_dim1.size());
  for (size_t i = 0; i < ref_dim0.size(); ++i) {
    REQUIRE(ref_dim0[i] == parsed_dim0[i]);
    REQUIRE(ref_dim1[i] == parsed_dim1[i]);
  }
}

TEST_CASE("Parse 1D linspace implicit outerbrace", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x ]
  BinEdges: { linspace: {nb: 3, low: 0, up: 3} }
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {0, 1, 2, 3};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Parse 1D linspace explicit outerbrace", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x ]
  BinEdges: [ { linspace: {nb: 3, low: 0, up: 3} } ]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {0, 1, 2, 3};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Parse 1D linspace & scalar", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x ]
  BinEdges: [ -1, { linspace: {nb: 3, low: 0, up: 3} }, 4, 5, 6 ]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {-1, 0, 1, 2, 3, 4, 5, 6};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Parse 1D logspace", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x ]
  BinEdges: { logspace: {nb: 2, low: 1, up: 100} }
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);

  std::vector<double> ref = {1, 10, 100};
  std::vector<double> parsed_dim0 = Binning->GetBinEdges(0, 0);
  REQUIRE(Binning->GetNDim(0) == 1);
  REQUIRE(ref.size() == parsed_dim0.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    REQUIRE(ref[i] == parsed_dim0[i]);
  }
}

TEST_CASE("Throw on ambiguous 1D range specifier", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ "x", "y" ]
  BinEdges: [ { linspace: { nb: 10, low: 0, up: 10 } }, { linspace: { nb: 10, low: 10, up: 100 } }  ]
  Uniform: true
  )";

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  REQUIRE_THROWS(
      Binning->SetupSampleBinning(BinningNode, SingleSample));
}

TEST_CASE("Non-uniform External Binning File", "[BinningHandler]") {
  MACH3LOG_INFO("");
  std::string yamlContent_1DRangeRangeBug = R"(
  VarStr : [ x, y ]
  Bins: { File: testbinning.yml, Key: mybins }
  Uniform: false
  )";

  std::ofstream testbinning("testbinning.yml");
  testbinning << R"(---
mybins: [ [[0.0, 0.1], [0.0, 0.1]],
          [[0.0, 0.1], [0.1, 0.2]],
          [[0.0, 0.1], [0.2, 0.3]],
          [[0.0, 0.1], [0.3, 0.4]],
          [[0.0, 0.1], [0.4, 0.5]],
          [[0.1, 0.3], [0.0, 0.5]],
          [[0.3, 0.5], [0.0, 0.5]],
          [[0.5, 0.7], [0.0, 0.5]]
]
)";
  testbinning.close();

  auto Binning = std::make_unique<BinningHandler>();
  SampleInfo SingleSample;
  YAML::Node BinningNode =
      STRINGtoYAML(yamlContent_1DRangeRangeBug);
  Binning->SetupSampleBinning(BinningNode, SingleSample);
  REQUIRE(Binning->GetNBins() == 8);
}
