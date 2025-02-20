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
  std::vector<double> TrueCosZenith;
};

//Internal MaCh3Modes
enum MaCh3Mode{
  kMaCh3_CCQE,
  kMaCh3_2p2h,
  kMaCh3_CC1pipm,
  kMaCh3_CC1pi0,
  kMaCh3_CCMPi,
  kMaCh3_CCDIS,
  kMaCh3_NCQE,
  kMaCh3_NC1pipm,
  kMaCh3_NC1pi0,
  kMaCh3_Other,
  kMaCh3_nModes
};

inline MaCh3Mode NuWroModeToMaCh3Mode(int NuWroMode) {
  MaCh3Mode ReturnMode;
  switch(std::abs(NuWroMode)){
    case 1:
      ReturnMode = kMaCh3_CCQE;
      break;
    case 2:
      ReturnMode = kMaCh3_2p2h;
      break;
    case 11:
    case 12:
    case 13:
      ReturnMode = kMaCh3_CC1pipm;
      break;
    case 14:
      ReturnMode = kMaCh3_CC1pi0;
      break;
    // CC Multi-Pi
    case 21:
      ReturnMode = kMaCh3_CCMPi;
      break;
    // CC DIS
    case 26:
      ReturnMode = kMaCh3_CCDIS;
      break;
    //NC 1pi01n
    case 31:
    //NC 1pi01p
    case 32:
      ReturnMode = kMaCh3_NC1pi0;
      break;
    //NC 1pi-1p
    case 33:
    //NC 1pi+1n
    case 34:
      ReturnMode = kMaCh3_NC1pipm;
      break;
    //NC QE 1p
    case 51:
    //NC QE 1n
    case 52:
      ReturnMode = kMaCh3_NCQE;
      break;
    default:
      ReturnMode = kMaCh3_Other;
      break;
  }

  return ReturnMode;
}

inline std::string ModeToString(int Mode) {
  std::string ModeString;
  switch(Mode){
    case kMaCh3_CCQE:
      ModeString = "ccqe";
      break;
    case kMaCh3_2p2h:
      ModeString = "2p2h";
      break;
    case kMaCh3_CC1pipm:
      ModeString = "cc1pipm";
      break;
    case kMaCh3_CC1pi0:
      ModeString = "cc1pi0";
      break;      
    case kMaCh3_CCMPi:
      ModeString = "ccmultipi";
      break;
    case kMaCh3_CCDIS:
      ModeString = "ccdis";
      break;
    case kMaCh3_NCQE:
      ModeString = "ncqe";
      break;
    case kMaCh3_NC1pi0:
      ModeString = "nc1pi0";
      break;
    case kMaCh3_NC1pipm:
      ModeString = "nc1pipm";
      break;
    case kMaCh3_Other:
      ModeString = "other";
      break;
    default:
      throw;
  }

  return ModeString;
}
