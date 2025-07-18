#include "Samples/SampleHandlerBase.h"
#include "Splines/BinnedSplineHandler.h"
#include "TSpline.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"

struct KnotSet {
  std::vector<double> x;
  std::vector<double> y;
};

TSpline3* MakeSpline(double x[], double y[], int N,
                     SplineInterpolation SplineInterpolationType,
                     Color_t Color,
                     Style_t LineStyle) {
  // Create the cubic spline
  TSpline3* mySpline = new TSpline3("spline", x, y, N);
  TSpline3_red* Spline = new TSpline3_red(mySpline, SplineInterpolationType);
  if(mySpline) delete mySpline;

  mySpline = Spline->ConstructTSpline3();
  delete Spline;

  // Style and draw the original spline
  mySpline->SetLineColor(Color);
  mySpline->SetLineStyle(LineStyle);

  return mySpline;
}

void MakeSplinePlot(double x[], double y[], int N, TCanvas* c) {
  constexpr int nInterpolations = 4;
  constexpr SplineInterpolation interpolationTypes[nInterpolations] = {kTSpline3, kMonotonic, kLinear, kAkima};
  constexpr Color_t colors[nInterpolations] = {kBlue, kRed, kGreen, kViolet};
  constexpr Style_t lineStyles[nInterpolations] = {kSolid, kDotted, kDashed, kDashDotted};

  std::vector<TSpline3*> splines;

  // Create and draw TGraph to show original points (knots)
  TGraph* knotsGraph = new TGraph(N, x, y);
  knotsGraph->SetMarkerStyle(kFullCircle);
  knotsGraph->SetMarkerSize(1.2);
  knotsGraph->SetMarkerColor(kBlack);
  knotsGraph->Draw("AP");  // "A" creates new axis, "P" draws points

  // Draw each spline
  for (int i = 0; i < nInterpolations; ++i) {
    TSpline3* spline = MakeSpline(x, y, N, interpolationTypes[i], colors[i], lineStyles[i]);
    splines.push_back(spline);

    spline->Draw("C SAME");
  }

  // Add legend
  TLegend* legend = new TLegend(0.15, 0.75, 0.45, 0.88);
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->AddEntry(knotsGraph, "Knots", "p");

  for (int i = 0; i < nInterpolations; ++i) {
    legend->AddEntry(splines[i], SplineInterpolation_ToString(interpolationTypes[i]).c_str(), "l");
  }

  legend->Draw();
  c->Print("SplinePlot.pdf");

  for (int i = 0; i < nInterpolations; ++i) {
    delete splines[i];
  }
  delete legend;
  delete knotsGraph;
}


void TSpline3_Plot(TCanvas* c) {
  std::vector<KnotSet> knotSets = {
    { {0, 1, 2, 3, 4},     {0, 0.5, 0.75, 1, 1.6} },
    { {0, 1},              {1, 3} },
    { {0, 1, 2},           {1, 3, 5} },
    { {-1, 0, 1},          {1.6, 1, 1.5} },
    { {0, 2, 3, 4.5, 6},   {0, 0.6, 0.8, 1.3, 10} },
    { {-2, -1, 0, 1.0, 2}, {1, 1, 1, 0, 0} }
  };

  // Loop over each set of knots and plot
  for (auto& knots : knotSets) {
    MakeSplinePlot(knots.x.data(), knots.y.data(), knots.x.size(), c);
  }
}


TF1* MakeTF1(double x[], double y[], int N,
                     SplineInterpolation SplineInterpolationType,
                     Color_t Color,
                     Style_t LineStyle) {
  // Create the cubic spline
  TF1* mySpline = new TF1("spline", GetTF1(SplineInterpolationType).c_str(), x[0], x[N-1]);
  TGraph* graph = new TGraph(N, x, y);
  graph->Fit(mySpline, "Q0");  // "Q0" = quiet, no draw

  TF1_red* Spline = new TF1_red(mySpline);
  if(mySpline) delete mySpline;

  mySpline = Spline->ConstructTF1(GetTF1(SplineInterpolationType), x[0], x[N-1]);
  delete Spline;
  delete graph;

  // Style and draw the original spline
  mySpline->SetLineColor(Color);
  mySpline->SetLineStyle(LineStyle);

  return mySpline;
}


void MakeTF1Plot(double x[], double y[], int N, TCanvas* c) {
  constexpr int nInterpolations = 1;
  constexpr SplineInterpolation interpolationTypes[nInterpolations] = {kLinearFunc};
  constexpr Color_t colors[nInterpolations] = {kBlue};
  constexpr Style_t lineStyles[nInterpolations] = {kSolid};

  std::vector<TF1*> splines;

  // Create and draw TGraph to show original points (knots)
  TGraph* knotsGraph = new TGraph(N, x, y);
  knotsGraph->SetMarkerStyle(kFullCircle);
  knotsGraph->SetMarkerSize(1.2);
  knotsGraph->SetMarkerColor(kBlack);
  knotsGraph->Draw("AP");  // "A" creates new axis, "P" draws points

  // Draw each spline
  for (int i = 0; i < nInterpolations; ++i) {
    TF1* spline = MakeTF1(x, y, N, interpolationTypes[i], colors[i], lineStyles[i]);
    splines.push_back(spline);

    spline->Draw("C SAME");
  }

  // Add legend
  TLegend* legend = new TLegend(0.15, 0.75, 0.45, 0.88);
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->AddEntry(knotsGraph, "Knots", "p");

  for (int i = 0; i < nInterpolations; ++i) {
    legend->AddEntry(splines[i],
                     (SplineInterpolation_ToString(interpolationTypes[i]) + " " + GetTF1(interpolationTypes[i])).c_str(), "l");
  }

  legend->Draw();
  c->Print("SplinePlot.pdf");

  for (int i = 0; i < nInterpolations; ++i) {
    delete splines[i];
  }
  delete legend;
  delete knotsGraph;
}

void TF1_Plot(TCanvas* c) {
  std::vector<KnotSet> knotSets = {
    { {0, 1},              {1, 2} },
    { {0, 2},              {1, 3} }
  };

  // Loop over each set of knots and plot
  for (auto& knots : knotSets) {
    MakeTF1Plot(knots.x.data(), knots.y.data(), knots.x.size(), c);
  }
}

int main() {
  // Create canvas
  TCanvas* Canvas = new TCanvas("c", "Spline Test", 800, 600);
  Canvas->Print("SplinePlot.pdf[");

  TSpline3_Plot(Canvas);
  TF1_Plot(Canvas);

  Canvas->Print("SplinePlot.pdf]");
  delete Canvas;

  return 0;
}

