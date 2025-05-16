#pragma once

/// @brief class holding basic information about tutorial MC
struct TutorialMCInfo {
  /// @brief Constructor
  TutorialMCInfo() {
  }
  /// @brief Destructor
  ~TutorialMCInfo() {
  }
  /// neutrino type
  std::vector<int> nutype;
  /// type of neutrino
  std::vector<int> oscnutype;
  /// target material
  std::vector<int> Target;
  /// interaction mode
  std::vector<double> Mode;
  /// is Neutral Current or not
  std::vector<bool> isNC;
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
