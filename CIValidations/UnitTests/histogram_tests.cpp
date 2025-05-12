#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include "Samples/SampleHandlerBase.h"

void GetBinning(std::vector<double>& BinningX, std::vector<double>&  BinningY)
{
  const double BinArray_x[] = {0., 300., 400., 500., 600., 650., 700., 750., 800., 900., 1000., 1100., 1250., 1500., 1600., 1750., 2000., 2500., 3000., 5000., 30000.};
  const double BinArray_y[] = {-1.0, 0.6, 0.7, 0.76, 0.8, 0.84, 0.86, 0.88, 0.89, 0.9, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 0.995, 1.0};
  BinningX = MakeVector(BinArray_x);
  BinningY = MakeVector(BinArray_y);
}

TH2Poly* GetPolyHist()
{
  std::vector<double> BinArray_x;
  std::vector<double> BinArray_y;
  GetBinning(BinArray_x, BinArray_y);

  TH2Poly* poly = new TH2Poly();
  poly->SetName("MaCh3");
  poly->SetTitle("MaCh3");
  double xmax, xmin, ymax, ymin;
  for(unsigned int iy = 0; iy < BinArray_y.size()-1; iy++)
  {
    ymax = BinArray_y[iy+1];
    ymin = BinArray_y[iy];
    for(unsigned int ix = 0; ix < BinArray_x.size()-1; ix++)
    {
      xmax = BinArray_x[ix+1];
      xmin = BinArray_x[ix];
      double binofx[] = {xmin, xmax, xmax, xmin};
      double binofy[] = {ymin, ymin, ymax, ymax};
      poly->AddBin(4,binofx,binofy);
    }
  }
  // Fixed seed for reproducibility
  TRandom3 rng(42);
  for (int i = 1; i <= poly->GetNumberOfBins(); i++) {
    const double value = rng.Uniform(0.0, 1.0);
    poly->SetBinContent(i, value);
  }
  for (int i = 1; i <= poly->GetNumberOfBins(); i++) {
    const double value = rng.Uniform(0.0, 1.0);
    poly->SetBinError(i, value);
  }

  return poly;
}

TEST_CASE("MakeVector", "[HistogramUtils]") {
  std::vector<double> BinningX;
  std::vector<double> BinningY;
  GetBinning(BinningX, BinningY);

  REQUIRE(BinningX.size() == 21);
  REQUIRE(BinningY.size() == 21);

  REQUIRE(BinningX.back() == 30000);
  REQUIRE(BinningY.back() == 1.0);
}

TEST_CASE("MakeTH2Poly", "[HistogramUtils]") {
  TH2Poly* Hist = GetPolyHist();
  REQUIRE(Hist->GetNumberOfBins() == 400);

  // KS: Based on https://github.com/catchorg/Catch2/blob/devel/docs/comparing-floating-point-numbers.md
  REQUIRE_THAT(Hist->GetBinContent(0),  Catch::Matchers::WithinAbs(0, 1e-6));
  REQUIRE_THAT(Hist->GetBinContent(10), Catch::Matchers::WithinAbs(0.44583275, 1e-6));
  REQUIRE_THAT(Hist->GetBinContent(20), Catch::Matchers::WithinAbs(0.65088847, 1e-6));
  REQUIRE_THAT(Hist->GetBinContent(50), Catch::Matchers::WithinAbs(0.09060643, 1e-6));
  REQUIRE_THAT(Hist->GetBinContent(1000),  Catch::Matchers::WithinAbs(0, 1e-6));
  // ROOT fixed terrible bug since 6.18, this is just in case for more see: https://t2k-experiment.slack.com/archives/CU9CBG6NS/p1714551365661589
  REQUIRE_THAT(Hist->GetBinError(10), Catch::Matchers::WithinAbs(0.58068662, 1e-6));
  REQUIRE_THAT(Hist->GetBinError(400), Catch::Matchers::WithinAbs(0.2382361521, 1e-6));

  REQUIRE_THAT(OverflowIntegral(Hist),   Catch::Matchers::WithinAbs(196.17789500, 1e-6));
  REQUIRE_THAT(NoOverflowIntegral(Hist), Catch::Matchers::WithinAbs(196.17789500, 1e-6));

  delete Hist;
}

TEST_CASE("ConvertTH2PolyToTH2D", "[HistogramUtils]") {
  TH2Poly* Poly = GetPolyHist();
  std::vector<double> BinningX;
  std::vector<double> BinningY;
  GetBinning(BinningX, BinningY);

  TH2D* Hist = new TH2D("Hist", "Hist;X-axis;Y-axis", BinningX.size()-1, BinningX.data(), BinningY.size()-1, BinningY.data());
  TH2D* HistConverted = ConvertTH2PolyToTH2D(Poly, Hist);

  REQUIRE_THAT(HistConverted->Integral(),   Catch::Matchers::WithinAbs(196.17789500, 1e-6));

  delete Poly;
  delete Hist;
  delete HistConverted;
}

TEST_CASE("PolyProjection", "[HistogramUtils]") {
  TH2Poly* Poly = GetPolyHist();
  std::vector<double> BinningX;
  std::vector<double> BinningY;
  GetBinning(BinningX, BinningY);

  TH1D* ProjectionX = PolyProjectionX(Poly, "HistX", BinningX, true);
  TH1D* ProjectionY = PolyProjectionY(Poly, "HistY", BinningY, true);

  REQUIRE_THAT(ProjectionX->GetBinContent(10), Catch::Matchers::WithinAbs(9.25811633, 1e-6));
  REQUIRE_THAT(ProjectionX->GetBinError(10),   Catch::Matchers::WithinAbs(2.29812238, 1e-6));
  REQUIRE_THAT(ProjectionX->Integral(),        Catch::Matchers::WithinAbs(196.17789500, 1e-6));

  REQUIRE_THAT(ProjectionY->GetBinContent(10), Catch::Matchers::WithinAbs(9.04070014, 1e-6));
  REQUIRE_THAT(ProjectionY->GetBinError(10),   Catch::Matchers::WithinAbs(2.30055047, 1e-6));
  REQUIRE_THAT(ProjectionY->Integral(),        Catch::Matchers::WithinAbs(196.17789500, 1e-6));

  delete Poly;
  delete ProjectionX;
  delete ProjectionY;
}

TEST_CASE("NormalisePoly", "[HistogramUtils]") {
  TH2Poly* Poly = GetPolyHist();
  TH2Poly* Norm = NormalisePoly(Poly);

  REQUIRE_THAT(Norm->GetBinContent(10), Catch::Matchers::WithinAbs(0.0000000763, 1e-6));
  REQUIRE_THAT(OverflowIntegral(Norm),  Catch::Matchers::WithinAbs(0.0042403182, 1e-6));

  delete Poly;
  delete Norm;
}


TEST_CASE("ConvertTH2DToTH2Poly", "[HistogramUtils]") {
  std::vector<double> BinningX;
  std::vector<double> BinningY;
  GetBinning(BinningX, BinningY);

  TH2D* Hist = new TH2D("Hist", "Hist;X-axis;Y-axis", BinningX.size()-1, BinningX.data(), BinningY.size()-1, BinningY.data());
  TH2Poly* Poly = ConvertTH2DtoTH2Poly(Hist);
  REQUIRE(Poly->GetNumberOfBins() == 400);
  delete Poly;

  delete Hist;
}

