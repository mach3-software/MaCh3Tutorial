#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include "mcmc/StatisticalUtils.h"

// Test for Jeffreys Scale
TEST_CASE("Jeffreys Scale", "[StatisticalUtils]") {
  REQUIRE(GetJeffreysScale(8) == "Substantial");
}

// Test for Dunne-Kaboth Interpretation
TEST_CASE("Dunne-Kaboth Interpretation", "[StatisticalUtils]") {
  REQUIRE(GetDunneKaboth(8) == "> 1 #sigma");
}

// Test for Bayesian Information Criterion (BIC)
TEST_CASE("Bayesian Information Criterion", "[StatisticalUtils]") {
  REQUIRE_THAT(GetBIC(3000, 200, 700), Catch::Matchers::WithinAbs(6708.8221, 1e-2));
}

// Test for Neffective
TEST_CASE("Effective Sample Size (Neffective)", "[StatisticalUtils]") {
  REQUIRE_THAT(GetNeffective(70, 60), Catch::Matchers::WithinAbs(0.030952, 1e-2));
}

// Test for Anderson-Darling Test Statistic
TEST_CASE("Anderson-Darling Test Statistic", "[StatisticalUtils]") {
  REQUIRE_THAT(GetAndersonDarlingTestStat(0.5, 0.4, 0.7), Catch::Matchers::WithinAbs(0.218217, 1e-2));
}

// Test for Number of Runs
TEST_CASE("Number of Runs", "[StatisticalUtils]") {
  REQUIRE(GetNumberOfRuns({1, 2, 1, 2, 1}) == 5);
  REQUIRE(GetNumberOfRuns({1, 1, 2, 2, 1, 1}) == 3);
}

// Test for Beta Parameter
TEST_CASE("Beta Parameter Calculation", "[StatisticalUtils]") {
  REQUIRE_THAT(GetBetaParameter(10, 12, 0.5, kBarlowBeeston), Catch::Matchers::WithinAbs(0.993290111, 1e-2));
}

// Test for Sub-Optimality
TEST_CASE("Sub-Optimality", "[StatisticalUtils]") {
  REQUIRE_THAT(GetSubOptimality({1.0, 1.0, 1.0}, 3), Catch::Matchers::WithinAbs(1.0, 1e-2));
  REQUIRE_THAT(GetSubOptimality({1.0, 2.0, 3.0}, 3), Catch::Matchers::WithinAbs(1.2148760, 1e-2));
}

// Test for Fisher Combined P-Value
TEST_CASE("Fisher Combined P-Value", "[StatisticalUtils]") {
  REQUIRE_THAT(FisherCombinedPValue({0.05, 0.05}), Catch::Matchers::WithinAbs(0.01747, 1e-2));
  REQUIRE_THAT(FisherCombinedPValue({0.1, 0.2, 0.3}), Catch::Matchers::WithinAbs(0.1152, 1e-2));
}
