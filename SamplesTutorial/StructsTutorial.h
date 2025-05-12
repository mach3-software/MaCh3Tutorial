#pragma once

/// @brief class holding basic information about tutorial MC
struct tutorial_base {
  /// @brief Constructor
  tutorial_base() {
    isNC = nullptr;
  }
  /// @brief Destructor
  ~tutorial_base() {
    if(isNC) delete[] isNC;
  }
  /// neutrino type
  int nutype;
  /// type of neutrino
  int oscnutype;
  ///signal or not
  bool signal;
  /// total number of events
  int nEvents;
  /// target material
  std::vector<int> Target;
  /// interaction mode
  std::vector<double> Mode;
  /// is Neutral Current or not
  bool *isNC;
  /// true neutrino energy
  std::vector<double> TrueEnu;
  /// Reco neutrino energy
  std::vector<double> RecoEnu;
  /// shifted neutrino energy
  std::vector<double> RecoEnu_shifted;
  /// 4-momentum transfer
  std::vector<double> Q2;
  /// True Cosine zentih only used for atmospheric
  std::vector<double> TrueCosZenith;
  /// Lepton energy
  std::vector<double> ELep;
};
