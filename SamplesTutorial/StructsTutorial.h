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
  int nutype = M3::_BAD_INT_;
  /// type of neutrino
  int oscnutype = M3::_BAD_INT_;
  /// target material
  int Target = M3::_BAD_INT_;
  /// interaction mode
  double Mode = M3::_BAD_DOUBLE_;
  /// is Neutral Current or not
  bool isNC = false;
  /// true neutrino energy
  double TrueEnu = M3::_BAD_DOUBLE_;
  /// Reco neutrino energy
  double RecoEnu = M3::_BAD_DOUBLE_;
  /// shifted neutrino energy
  double RecoEnu_shifted = M3::_BAD_DOUBLE_;
  /// 4-momentum transfer
  double Q2 = M3::_BAD_DOUBLE_;
  /// True Cosine zentih only used for atmospheric
  double TrueCosZenith = M3::_BAD_DOUBLE_;
  /// Lepton energy
  double ELep = M3::_BAD_DOUBLE_;
};
