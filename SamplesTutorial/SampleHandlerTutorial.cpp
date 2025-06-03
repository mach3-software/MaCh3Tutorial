#include "SamplesTutorial/SampleHandlerTutorial.h"

// ************************************************
SampleHandlerTutorial::SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                                             ParameterHandlerOsc* oscillation_handler, const std::shared_ptr<OscillationHandler>&  Oscillator_)
                                             : SampleHandlerFD(config_name, parameter_handler, oscillation_handler, Oscillator_) {
// ************************************************
  KinematicParameters = &KinematicParametersTutorial;
  ReversedKinematicParameters = &ReversedKinematicParametersTutorial;

  // === JM assign kinematic vector maps ===
  KinematicVectors = &KinematicVectorsTutorial;
  ReversedKinematicVectors = &ReversedKinematicVectorsTutorial;
  // =======================================

  isATM = false;
  Initialise();
}

// ************************************************
SampleHandlerTutorial::~SampleHandlerTutorial() {
// ************************************************

}

// ************************************************
void SampleHandlerTutorial::Init() {
// ************************************************
  if (CheckNodeExists(SampleManager->raw(), "POT")) {
    pot = SampleManager->raw()["POT"].as<double>();
  } else{
    MACH3LOG_ERROR("POT not defined in {}, please add this!", SampleManager->GetFileName());
    throw MaCh3Exception(__FILE__, __LINE__);
  }
  MACH3LOG_INFO("-------------------------------------------------------------------");
}

void SampleHandlerTutorial::DebugShift(const double * par, std::size_t iEvent) {
  // HH: This is a debug function to shift the reco energy to 4 GeV if the reco energy is less than 2 GeV
  if (TutorialSamples.RecoEnu[iEvent] < 2.0 && *par != 0) {
    TutorialSamples.RecoEnu_shifted[iEvent] = 4;
  }
}

void SampleHandlerTutorial::EResLep(const double * par, std::size_t iEvent) {
  // HH: Lepton energy resolution contribution to reco energy
  TutorialSamples.RecoEnu_shifted[iEvent] += (*par) * TutorialSamples.ELep[iEvent];
}

void SampleHandlerTutorial::EResTot(const double * par, std::size_t iEvent) {
  // HH: Total energy resolution contribution to reco energy
  TutorialSamples.RecoEnu_shifted[iEvent] += (*par) * TutorialSamples.RecoEnu[iEvent];
}

void SampleHandlerTutorial::RegisterFunctionalParameters() {
  MACH3LOG_INFO("Registering functional parameters");
  // This function manually populates the map of functional parameters
  // Maps the name of the functional parameter to the pointer of the function
  
  // This is the part where we manually enter things
  // A lambda function has to be used so we can refer to a non-static member function
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  RegisterIndividualFunctionalParameter("DebugNothing", 
                            kDebugNothing, 
                            [this](const double * par, std::size_t iEvent) {});

  RegisterIndividualFunctionalParameter("DebugShift",
                            kDebugShift, 
                            [this](const double * par, std::size_t iEvent) { this->DebugShift(par, iEvent); });

  RegisterIndividualFunctionalParameter("EResLep",
                            kEResLep, 
                            [this](const double * par, std::size_t iEvent) { this->EResLep(par, iEvent); });

  RegisterIndividualFunctionalParameter("EResTot",
                            kEResTot, 
                            [this](const double * par, std::size_t iEvent) { this->EResTot(par, iEvent); });
  #pragma GCC diagnostic pop
}

void SampleHandlerTutorial::resetShifts(int iEvent) {
  // Reset the shifts to the original values
  TutorialSamples.RecoEnu_shifted[iEvent] = TutorialSamples.RecoEnu[iEvent];
}


// ************************************************
void SampleHandlerTutorial::SetupSplines() {
// ************************************************
  SplineHandler = nullptr;

  if(ParHandler->GetNumParamsFromSampleName(SampleName, SystType::kSpline) > 0){
    SplineHandler = std::make_unique<BinnedSplineTutorial>(ParHandler, Modes);
    InitialiseSplineObject();
  } else {
    MACH3LOG_WARN("Not using splines");
  }
}

// ************************************************
void SampleHandlerTutorial::SetupWeightPointers() {
// ************************************************
  for (unsigned int j = 0; j < GetNEvents(); ++j) {
    MCSamples.ntotal_weight_pointers[j] = 2;
    MCSamples.total_weight_pointers[j].resize(MCSamples.ntotal_weight_pointers[j]);
    MCSamples.total_weight_pointers[j][0] = MCSamples.osc_w_pointer[j];
    MCSamples.total_weight_pointers[j][1] = &(MCSamples.xsec_w[j]);
  }
}

// ************************************************
int SampleHandlerTutorial::SetupExperimentMC() {
// ************************************************
  TChain* _Chain = new TChain("FlatTree_VARS");
  for (const std::string& filename : mc_files) {
    _Chain->Add(filename.c_str());
  }
  // To loop over all events:
  int nEntries = static_cast<int>(_Chain->GetEntries());
  delete _Chain;

  TutorialMCInfo *tutobj = &(TutorialSamples);
  tutobj->TrueEnu.resize(nEntries);
  tutobj->RecoEnu.resize(nEntries);
  tutobj->RecoEnu_shifted.resize(nEntries);
  tutobj->Q2.resize(nEntries);
  tutobj->Mode.resize(nEntries);
  tutobj->Target.resize(nEntries);
  tutobj->isNC.resize(nEntries);
  tutobj->ELep.resize(nEntries);
  tutobj->nutype.resize(nEntries);
  tutobj->oscnutype.resize(nEntries);

  // === JM resize particle-level vectors ===
  tutobj->particle_energy.resize(nEntries);
  tutobj->particle_pdg.resize(nEntries);
  tutobj->particle_beamangle.resize(nEntries);
  // ========================================

  int TotalEventCounter = 0.;
  for(int iChannel = 0 ; iChannel < static_cast<int>(OscChannels.size()); iChannel++) {

    int nutype_ = OscChannels[iChannel].InitPDG;
    int oscnutype_ = OscChannels[iChannel].FinalPDG;

    MACH3LOG_INFO("-------------------------------------------------------------------");
    MACH3LOG_INFO("input file: {}", mc_files[iChannel]);

    TFile * _sampleFile = new TFile(mc_files[iChannel].c_str(), "READ");
    TTree* _data = static_cast<TTree*>(_sampleFile->Get("FlatTree_VARS"));

    if(_data){
      MACH3LOG_INFO("Found \"FlatTree_VARS\" tree in {}", mc_files[iChannel]);
      MACH3LOG_INFO("With number of entries: {}", _data->GetEntries());
    } else{
      MACH3LOG_ERROR("Could not find \"FlatTree_VARS\" tree in {}", mc_files[iChannel]);
      throw MaCh3Exception(__FILE__, __LINE__);
    }

    //Truth Variables
    float Enu_true, Q2, trueCZ, ELep;
    int tgt, Mode, PDGLep;

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
    _data->SetBranchStatus("ELep", true);
    _data->SetBranchAddress("ELep", &ELep);
    // KS: If we have CosineZenith branch this must mean Atmospheric sample
    if (_data->GetBranch("CosineZenith")) {
      if(iChannel == 0) {
        MACH3LOG_INFO("Enabling Atmospheric");
        isATM = true;
        tutobj->TrueCosZenith.resize(nEntries);
        MCSamples.rw_truecz.resize(nEntries);
      }
      _data->SetBranchStatus("CosineZenith", true);
      _data->SetBranchAddress("CosineZenith", &trueCZ);
    }

    /*
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> unif(0,3);
    std::normal_distribution<> mu_angle(0,M_PI/8);
    std::normal_distribution<> pi_angle(0,M_PI/2);
    std::uniform_real_distribution<> nucl_angle(-M_PI,M_PI);
    std::exponential_distribution<> pi_energy(1./0.5);
    std::exponential_distribution<> nucl_energy(1./2);

    for (int i = 0; i < nEntries; ++i) { // Loop through tree
      _data->GetEntry(i);
      
      // === JM resize particle-level vectors ===
      // JM: We don't have particle-level info in the tutorial sample, so will fake it
      int nParticles = static_cast<int>(Enu_true+3); //fake number of particles in event
      tutobj->particle_energy[TotalEventCounter].resize(nParticles);
      tutobj->particle_pdg[TotalEventCounter].resize(nParticles);
      tutobj->particle_beamangle[TotalEventCounter].resize(nParticles);
      // ========================================

      tutobj->TrueEnu[TotalEventCounter] = Enu_true;
      // HH: We don't have Erec in the tutorial sample, so we set it to the true energy
      tutobj->RecoEnu[TotalEventCounter] = Enu_true;
      tutobj->RecoEnu_shifted[TotalEventCounter] = Enu_true;
      tutobj->ELep[TotalEventCounter] = ELep;
      tutobj->Q2[TotalEventCounter]      = Q2;
      // KS: Currently we store target as 1000060120, therefore we hardcode it to 12
      tutobj->Target[TotalEventCounter] = 12;
      tutobj->Mode[TotalEventCounter]   = Modes->GetModeFromGenerator(std::abs(Mode));
      tutobj->nutype[TotalEventCounter] = nutype_;
      tutobj->oscnutype[TotalEventCounter] = oscnutype_;

      if (std::abs(PDGLep) == 12 || std::abs(PDGLep) == 14 || std::abs(PDGLep) == 16) {
        tutobj->isNC[TotalEventCounter] = true;
      } else {
        tutobj->isNC[TotalEventCounter] = false;
      }

      if(isATM) {
        tutobj->TrueCosZenith[TotalEventCounter] = trueCZ;
      }

      // === JM loop through particles in event ===
      for (int iParticle = 0; iParticle < nParticles; ++iParticle) {
        //JM: No particle-level data in sample, so fake it
        if (iParticle==0) {
          if (ELep >= Enu_true) MACH3LOG_ERROR("ELep ({} GeV) > Enu ({} GeV)", ELep, Enu_true);
          tutobj->particle_pdg[TotalEventCounter][iParticle] = PDGLep;
          tutobj->particle_energy[TotalEventCounter][iParticle] = ELep;
          tutobj->particle_beamangle[TotalEventCounter][iParticle] = mu_angle(gen);
        }
        else {
          int particle_seed = unif(gen);
          double angle, energy;
          int pdg;
          switch (particle_seed) {
            case 0: 
              pdg = 211;
              energy = pi_energy(gen);
              angle = pi_angle(gen);
              while (angle>M_PI || angle<-M_PI) angle = pi_angle(gen);
              break;
            case 1:
              pdg = -211;
              energy = pi_energy(gen);
              angle = pi_angle(gen);
              while (angle>M_PI || angle<-M_PI) angle = pi_angle(gen);
              break;
            case 2: 
              pdg = 2212;
              energy = nucl_energy(gen);
              angle = nucl_angle(gen);
              break;
            case 3:
              pdg = 2112;
              energy = nucl_energy(gen);
              angle = nucl_angle(gen);
              break;
            default:
              break;
          }
          tutobj->particle_energy[TotalEventCounter][iParticle] = energy;
          tutobj->particle_beamangle[TotalEventCounter][iParticle] = angle;
          tutobj->particle_pdg[TotalEventCounter][iParticle] = pdg;
        }
      }
      // ==========================================
      TotalEventCounter++;
    }
    _sampleFile->Close();
    delete _sampleFile;
    MACH3LOG_INFO("Initialised channel: {}/{}", iChannel, OscChannels.size());
  }
  return nEntries;
}

double SampleHandlerTutorial::ReturnKinematicParameter(KinematicTypes KinPar, int iEvent) {
  const double* paramPointer = GetPointerToKinematicParameter(KinPar, iEvent);
  return *paramPointer;
}

double SampleHandlerTutorial::ReturnKinematicParameter(int KinematicVariable, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(std::round(KinematicVariable));
  return ReturnKinematicParameter(KinPar, iEvent);
}

double SampleHandlerTutorial::ReturnKinematicParameter(std::string KinematicParameter, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameter));
  return ReturnKinematicParameter(KinPar, iEvent);
}

// === JM Define ReturnKinematicVector functions === 
std::vector<double> SampleHandlerTutorial::ReturnKinematicVector(KinematicParticleVecs KinVec, int iEvent) {
  switch (KinVec) {
    case kParticleEnergy:
      return TutorialSamples.particle_energy[iEvent];
    case kParticlePDG:
      return TutorialSamples.particle_pdg[iEvent];
    case kParticleBeamAngle:
      return TutorialSamples.particle_beamangle[iEvent];
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Vector: {}", static_cast<int>(KinVec));
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

std::vector<double> SampleHandlerTutorial::ReturnKinematicVector(int KinematicVector, int iEvent) {
  KinematicParticleVecs KinVec = static_cast<KinematicParticleVecs>(std::round(KinematicVector));
  return ReturnKinematicVector(KinVec, iEvent);
}

std::vector<double> SampleHandlerTutorial::ReturnKinematicVector(std::string KinematicVector, int iEvent) {
  KinematicParticleVecs KinVec = static_cast<KinematicParticleVecs>(ReturnKinematicVectorFromString(KinematicVector));
  return ReturnKinematicVector(KinVec, iEvent);
}
// =================================================

const double* SampleHandlerTutorial::GetPointerToKinematicParameter(KinematicTypes KinPar, int iEvent) {
  switch (KinPar) {
    case kTrueNeutrinoEnergy:
      return &TutorialSamples.TrueEnu[iEvent];
    case kRecoNeutrinoEnergy:
      // HH - here we return the shifted energy in case of detector systematics
      return &TutorialSamples.RecoEnu_shifted[iEvent];
    case kTrueQ2:
      return &TutorialSamples.Q2[iEvent];
    case kM3Mode:
      return &TutorialSamples.Mode[iEvent];
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Parameter type: {}", static_cast<int>(KinPar));
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

const double* SampleHandlerTutorial::GetPointerToKinematicParameter(double KinematicVariable, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(std::round(KinematicVariable));
  return GetPointerToKinematicParameter(KinPar, iEvent);
}

const double* SampleHandlerTutorial::GetPointerToKinematicParameter(std::string KinematicParameter, int iEvent) {
  KinematicTypes KinPar = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameter));
  return GetPointerToKinematicParameter(KinPar, iEvent);
}

void SampleHandlerTutorial::SetupFDMC() {
  TutorialMCInfo *tutobj = &(TutorialSamples);
  auto &fdobj = MCSamples;

  for(unsigned int iEvent = 0 ;iEvent < GetNEvents(); ++iEvent) {
    fdobj.rw_etru[iEvent] = &(tutobj->TrueEnu[iEvent]);
    fdobj.mode[iEvent] = &(tutobj->Mode[iEvent]);
    fdobj.Target[iEvent] = &(tutobj->Target[iEvent]);
    fdobj.isNC[iEvent] = tutobj->isNC[iEvent];
    fdobj.nupdgUnosc[iEvent] = &(tutobj->nutype[iEvent]);
    fdobj.nupdg[iEvent] = &(tutobj->oscnutype[iEvent]);
    if(isATM) fdobj.rw_truecz[iEvent] = &(tutobj->TrueCosZenith[iEvent]);
  }
}

std::vector<double> SampleHandlerTutorial::ReturnKinematicParameterBinning(std::string KinematicParameterStr) {
  if (IsParticleVarString(KinematicParameterStr)) return ReturnKinematicVectorBinning(KinematicParameterStr);

  std::vector<double> binningVector;
  KinematicTypes KinematicParameter = static_cast<KinematicTypes>(ReturnKinematicParameterFromString(KinematicParameterStr));

  int nBins = 0;
  double bin_width = 0;
  switch(KinematicParameter){
    case(kTrueNeutrinoEnergy):
      nBins = 20;
      bin_width = 0.5; //GeV
      break;
    case(kRecoNeutrinoEnergy):
      nBins = 20;
      bin_width = 0.5; //GeV
      break;
    case(kTrueQ2):
      nBins = 10;
      bin_width = 1.0;
      break;
    case(kM3Mode):
      nBins = Modes->GetNModes();
      bin_width = 1.0;
      break;
  }

  for(int bin_i = 0 ; bin_i < nBins ; bin_i++){
    binningVector.push_back(bin_i*bin_width);
  }

  return binningVector;
}
std::vector<double> SampleHandlerTutorial::ReturnKinematicVectorBinning(std::string KinematicVectorStr) {
  std::vector<double> binningVector;
  KinematicParticleVecs KinematicVector = static_cast<KinematicParticleVecs>(ReturnKinematicVectorFromString(KinematicVectorStr));

  int nBins = 0;
  double bin_start = 0;
  double bin_end = 0;

  switch(KinematicVector){
    case(kParticleEnergy):
      nBins = 20;
      bin_start = 0;
      bin_end = 6;
      break;
    case(kParticlePDG):
      nBins = 100;
      bin_start = -2000;
      bin_end = 2000;
      break;
    case(kParticleBeamAngle):
      nBins = 20;
      bin_start = -M_PI;
      bin_end = M_PI;
      break;
    default:
      MACH3LOG_ERROR("Binning for {} not found", KinematicVectorStr);
      throw MaCh3Exception(__FILE__,__LINE__);
      break;
  }
  double step = (bin_end - bin_start)/nBins;

  for(double bin_i = bin_start; bin_i <= bin_end; bin_i+=step){
    binningVector.push_back(bin_i);
  }

  return binningVector;
}
