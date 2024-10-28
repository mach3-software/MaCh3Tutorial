#include "samplePDF/samplePDFTutorial.h"

samplePDFTutorial::samplePDFTutorial(std::string mc_version_, covarianceXsec* xsec_cov_) : samplePDFFDBase(mc_version_, xsec_cov_) {
  Initialise();
}

samplePDFTutorial::~samplePDFTutorial() {
}

void samplePDFTutorial::Init() {
  TutorialSamples.resize(nSamples, tutorial_base());

  if (CheckNodeExists(SampleManager->raw(), "POT")) {
    pot = SampleManager->raw()["POT"].as<double>();
  } else{
    MACH3LOG_ERROR("POT not defined in {}, please add this!", SampleManager->GetFileName());
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  MACH3LOG_INFO("-------------------------------------------------------------------");
}

void samplePDFTutorial::SetupSplines() {
  MACH3LOG_WARN("Not using splines");
  splineFile = nullptr;
  
  return;
}

void samplePDFTutorial::SetupWeightPointers() {
  for (int i = 0; i < (int)MCSamples.size(); ++i) {
    for (int j = 0; j < MCSamples[i].nEvents; ++j) {
      MCSamples[i].ntotal_weight_pointers[j] = 2;
      MCSamples[i].total_weight_pointers[j] = new const double*[MCSamples[i].ntotal_weight_pointers[j]];
      MCSamples[i].total_weight_pointers[j][0] = MCSamples[i].osc_w_pointer[j];
      MCSamples[i].total_weight_pointers[j][1] = &(MCSamples[i].xsec_w[j]);
    }
  }
}

int samplePDFTutorial::setupExperimentMC(int iSample) {

  tutorial_base *tutobj = &(TutorialSamples[iSample]);
  int nutype = sample_nutype[iSample];
  int oscnutype = sample_oscnutype[iSample];
  bool signal = sample_signal[iSample];

  tutobj->nutype = nutype;
  tutobj->oscnutype = oscnutype;
  tutobj->signal = signal;

  MACH3LOG_INFO("-------------------------------------------------------------------");
  MACH3LOG_INFO("input file: {}", mc_files[iSample]);

  TFile * _sampleFile = new TFile(mc_files[iSample].c_str(), "READ");
  TTree * _data = (TTree*)_sampleFile->Get("FlatTree_VARS");

  if(_data){
    MACH3LOG_INFO("Found \"FlatTree_VARS\" tree in {}", mc_files[iSample]);
    MACH3LOG_INFO("With number of entries: {}", _data->GetEntries());
  }
  else{
    MACH3LOG_ERROR("Could not find \"FlatTree_VARS\" tree in {}", mc_files[iSample]);
    throw MaCh3Exception(__FILE__, __LINE__);
  }
  tutobj->nEvents = _data->GetEntries();
  tutobj->TrueEnu.resize(tutobj->nEvents);
  tutobj->Q2.resize(tutobj->nEvents);
  tutobj->Mode.resize(tutobj->nEvents);
  tutobj->Target.resize(tutobj->nEvents);
  tutobj->isNC = new bool[tutobj->nEvents];

  //Truth Variables
  float Enu_true;
  float Q2;
  int tgt;
  int Mode;
  int PDGLep;

  /*
  double ELep;
  double CosLep;
  bool flagCC0pi;
  bool flagCC1pip;
  bool flagCC1pim;
  */
  _data->SetBranchStatus("*", false);
  _data->SetBranchStatus("Enu_true", true);
  _data->SetBranchAddress("Enu_true", &Enu_true);
  _data->SetBranchStatus("Q2", true);
  _data->SetBranchAddress("Q2", &Q2);
  _data->SetBranchStatus("tgt", true);
  _data->SetBranchAddress("tgt", &tgt);
  _data->SetBranchStatus("Mode", true);
  _data->SetBranchAddress("Mode", &Mode);
  _data->SetBranchStatus("PDGLep", true);
  _data->SetBranchAddress("PDGLep", &PDGLep);


  /*
  _data->SetBranchStatus("ELep", true);
  _data->SetBranchAddress("ELep", &ELep);
  _data->SetBranchStatus("CosLep", true);
  _data->SetBranchAddress("CosLep", &CosLep);
  _data->SetBranchStatus("flagCC0pi", true);
  _data->SetBranchAddress("flagCC0pi", &flagCC0pi);
  _data->SetBranchStatus("flagCC1pip", true);
  _data->SetBranchAddress("flagCC1pip", &flagCC1pip);
  _data->SetBranchStatus("flagCC1pim", true);
  _data->SetBranchAddress("flagCC1pim", &flagCC1pim);
  */

  _data->GetEntry(0);
  for (int i = 0; i < tutobj->nEvents; ++i) { // Loop through tree
    _data->GetEntry(i);

    tutobj->TrueEnu[i] = Enu_true;
    tutobj->Q2[i]      = Q2;
    tutobj->Target[i]  = tgt;
    tutobj->Mode[i]    = Mode;

    if (std::abs(PDGLep) == 12 || std::abs(PDGLep) == 14 || std::abs(PDGLep) == 16) {
      tutobj->isNC[i] = true;
    } else {
      tutobj->isNC[i] = false;
    }
  }
  _sampleFile->Close();
  delete _sampleFile;
  return tutobj->nEvents;
}


double samplePDFTutorial::ReturnKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent) {
  switch (KinPar) {
    case kTrueNeutrinoEnergy:
      return TutorialSamples[iSample].TrueEnu[iEvent];
    case kTrueQ2:
      return TutorialSamples[iSample].Q2[iEvent];
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Parameter type: {}", static_cast<int>(KinPar));
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

double samplePDFTutorial::ReturnKinematicParameter(double KinematicVariable, int iSample, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(std::round(KinematicVariable));
  return ReturnKinematicParameter(KinPar, iSample, iEvent);
}

double samplePDFTutorial::ReturnKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameter));
  return ReturnKinematicParameter(KinPar, iSample, iEvent);
}

const double* samplePDFTutorial::GetPointerToKinematicParameter(KinematicTypes KinPar, int iSample, int iEvent) {
  switch (KinPar) {
    case kTrueNeutrinoEnergy:
      return &TutorialSamples[iSample].TrueEnu[iEvent];
    case kTrueQ2:
      return &TutorialSamples[iSample].Q2[iEvent];
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Parameter type: {}", static_cast<int>(KinPar));
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

const double* samplePDFTutorial::GetPointerToKinematicParameter(double KinematicVariable, int iSample, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(std::round(KinematicVariable));
  return GetPointerToKinematicParameter(KinPar, iSample, iEvent);
}

const double* samplePDFTutorial::GetPointerToKinematicParameter(std::string KinematicParameter, int iSample, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameter));
  return GetPointerToKinematicParameter(KinPar, iSample, iEvent);
}

int samplePDFTutorial::ReturnKinematicParameterFromString(std::string KinematicParameterStr){
  if (KinematicParameterStr.find("TrueNeutrinoEnergy") != std::string::npos) {return kTrueNeutrinoEnergy;}
  if (KinematicParameterStr.find("TrueQ2") != std::string::npos) {return kTrueQ2;}

  MACH3LOG_ERROR("Did not recognise Kinematic Parameter type...");
  throw MaCh3Exception(__FILE__, __LINE__);

  return -999;
}

inline std::string samplePDFTutorial::ReturnStringFromKinematicParameter(int KinematicParameter) {
  std::string KinematicString = "";
  switch(KinematicParameter){
   case kTrueNeutrinoEnergy:
     KinematicString = "TrueNeutrinoEnergy";
    break;
   case kTrueQ2:
     KinematicString = "TrueQ2";
     break;
   default:
    break;
  }

  return KinematicString;
}

void samplePDFTutorial::setupFDMC(int iSample) {
  tutorial_base *tutobj = &(TutorialSamples[iSample]);
  fdmc_base *fdobj = &(MCSamples[iSample]);  
  
  fdobj->nutype = tutobj->nutype;
  fdobj->oscnutype = tutobj->oscnutype;
  fdobj->signal = tutobj->signal;
  fdobj->SampleDetID = SampleDetID;
  
  for(int iEvent = 0 ;iEvent < fdobj->nEvents ; ++iEvent) {
    fdobj->rw_etru[iEvent] = &(tutobj->TrueEnu[iEvent]);
    fdobj->mode[iEvent] = &(tutobj->Mode[iEvent]);
    fdobj->Target[iEvent] = &(tutobj->Target[iEvent]);
    fdobj->isNC[iEvent] = tutobj->isNC[iEvent];
  }
}

std::vector<double> samplePDFTutorial::ReturnKinematicParameterBinning(std::string KinematicParameterStr) {
  std::vector<double> binningVector;
  KinematicTypes KinematicParameter = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameterStr));

  int nBins = 0;
  double bin_width = 0;
  switch(KinematicParameter){
    case(kTrueNeutrinoEnergy):
      nBins = 20;
      bin_width = 0.5; //GeV
      break;
    default:
      nBins = 10;
      bin_width = 1.0;
      break;
  }

  for(int bin_i = 0 ; bin_i < nBins ; bin_i++){
    binningVector.push_back(bin_i*bin_width);
  }

  return binningVector;
}
