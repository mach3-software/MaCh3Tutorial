#pragma once

struct tutorial_base {
  int nutype;
  int oscnutype;
  bool signal;
  int nEvents;
  std::vector<int> Target;
  std::vector<double> Mode;
  bool *isNC;
  std::vector<double> TrueEnu;
  std::vector<double> Q2;
};
