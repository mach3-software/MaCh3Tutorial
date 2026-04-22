#include "SamplesTutorial/SampleHandlerTutorial.h"

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RDataFrame.hxx>

// ************************************************
SampleHandlerTutorial::SampleHandlerTutorial(const std::string& config_name, ParameterHandlerGeneric* parameter_handler,
                                             const std::shared_ptr<OscillationHandler>&  Oscillator_)
                                             : SampleHandlerBase(config_name, parameter_handler, Oscillator_) {
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

void SampleHandlerTutorial::DebugShift(const M3::float_t* par, std::size_t iEvent) {
  // HH: This is a debug function to shift the reco energy to 4 GeV if the reco energy is less than 2 GeV
  if (TutorialSamples[iEvent].RecoEnu < 2.0 && *par != 0) {
    TutorialSamples[iEvent].RecoEnu_shifted = 4;
  }
}

void SampleHandlerTutorial::EResLep(const M3::float_t* par, std::size_t iEvent) {
  // HH: Lepton energy resolution contribution to reco energy
  TutorialSamples[iEvent].RecoEnu_shifted += (*par) * TutorialSamples[iEvent].ELep;
}

void SampleHandlerTutorial::EResTot(const M3::float_t* par, std::size_t iEvent) {
  // HH: Total energy resolution contribution to reco energy
  TutorialSamples[iEvent].RecoEnu_shifted += (*par) * TutorialSamples[iEvent].RecoEnu;
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
                            [this](const M3::float_t* par, std::size_t iEvent) {});

  RegisterIndividualFunctionalParameter("DebugShift",
                            kDebugShift, 
                            [this](const M3::float_t* par, std::size_t iEvent) { this->DebugShift(par, iEvent); });

  RegisterIndividualFunctionalParameter("EResLep",
                            kEResLep, 
                            [this](const M3::float_t* par, std::size_t iEvent) { this->EResLep(par, iEvent); });

  RegisterIndividualFunctionalParameter("EResTot",
                            kEResTot, 
                            [this](const M3::float_t* par, std::size_t iEvent) { this->EResTot(par, iEvent); });
  #pragma GCC diagnostic pop
}

void SampleHandlerTutorial::ResetShifts(const int iEvent) {
  // Reset the shifts to the original values
  TutorialSamples[iEvent].RecoEnu_shifted = TutorialSamples[iEvent].RecoEnu;
}


// ************************************************
void SampleHandlerTutorial::SetupSplines() {
// ************************************************
  if(ParHandler == nullptr) return;

  SplineHandler = nullptr;
  if(ParHandler->GetNumParamsFromSampleName(GetName(), SystType::kSpline) > 0){
    SplineHandler = std::make_unique<BinnedSplineTutorial>(ParHandler, Modes.get());
    InitialiseSplineObject();
  } else {
    MACH3LOG_WARN("Not using splines");
  }
}

// ************************************************
void SampleHandlerTutorial::AddAdditionalWeightPointers() {
// ************************************************

}

// ************************************************
void SampleHandlerTutorial::CleanMemoryBeforeFit() {
// ************************************************
  CleanVector(TutorialPlottingSamples);
}

// ************************************************
int SampleHandlerTutorial::SetupExperimentMC() {
// ************************************************
  std::vector<std::string> all_filenames;
  for(size_t iSample = 0; iSample < SampleDetails.size(); iSample++) {
    for (const std::vector<std::string>& files : SampleDetails[iSample].mc_files) {
      for (const std::string& filename : files) {
        all_filenames.push_back(filename);
      }
    }
  }
  ROOT::RDataFrame df("Events", all_filenames);
  int nEntries = static_cast<int>(df.Count().GetValue() );

  TutorialSamples.resize(nEntries);
  TutorialPlottingSamples.resize(nEntries);

  int TotalEventCounter = 0.;
  for(size_t iSample = 0; iSample < SampleDetails.size(); iSample++)
  {
    auto& OscillationChannels = SampleDetails[iSample].OscChannels;
    for(int iChannel = 0 ; iChannel < static_cast<int>(OscillationChannels.size()); iChannel++)
    {
      int nutype_ = OscillationChannels[iChannel].InitPDG;
      int oscnutype_ = OscillationChannels[iChannel].FinalPDG;


      auto FileNames = SampleDetails[iSample].mc_files[iChannel];
      for(int iFile = 0 ; iFile < static_cast<int>(FileNames.size()); iFile++)
      {
        auto FileName = FileNames[iFile];
        MACH3LOG_INFO("-------------------------------------------------------------------");
        MACH3LOG_INFO("input file: {}", FileName);

        if (iChannel == 0) {
          auto test_reader = ROOT::RNTupleReader::Open("Events", FileName);
          if (test_reader->GetDescriptor().FindFieldId("CosineZenith") != ROOT::kInvalidDescriptorId) {
            MACH3LOG_INFO("Enabling Atmospheric");
            isATM = true;
          }
        }

        auto model = ROOT::RNTupleModel::Create();
        auto Enu_true = model->MakeField<float>("Enu_true");
        auto Q2 = model->MakeField<float>("Q2");
        auto trueCZ = isATM ? model->MakeField<float>("CosineZenith") : nullptr;
        auto ELep = model->MakeField<float>("ELep");
        auto tgt = model->MakeField<int>("tgt");
        auto Mode = model->MakeField<int>("Mode");
        auto PDGLep = model->MakeField<int>("PDGLep");

        auto reader = ROOT::RNTupleReader::Open(std::move(model), "Events", FileName);
        MACH3LOG_INFO("Found \"Events\" RNTuple in {}", FileName);
        MACH3LOG_INFO("With number of entries: {}", reader->GetNEntries());

        std::random_device rd;
        std::mt19937 gen(rd());
        for (auto entryId: *reader) {
          reader->LoadEntry(entryId);

          // === JM resize particle-level vectors ===
          // JM: We don't have particle-level info in the tutorial sample, so will fake it
          constexpr int nParticles = 5; //fake number of particles in event
          TutorialPlottingSamples[TotalEventCounter].particle_energy.resize(nParticles);
          TutorialPlottingSamples[TotalEventCounter].particle_pdg.resize(nParticles);
          TutorialPlottingSamples[TotalEventCounter].particle_beamangle.resize(nParticles);
          // ========================================

          TutorialSamples[TotalEventCounter].TrueEnu = *Enu_true;
          // HH: We don't have Erec in the tutorial sample, so we set it to the true energy
          TutorialSamples[TotalEventCounter].RecoEnu = *Enu_true;
          TutorialSamples[TotalEventCounter].RecoEnu_shifted = *Enu_true;
          TutorialSamples[TotalEventCounter].ELep = *ELep;
          TutorialSamples[TotalEventCounter].Q2     = *Q2;
          // KS: Currently we store target as 1000060120, therefore we hardcode it to 12
          TutorialSamples[TotalEventCounter].Target = 12;
          TutorialSamples[TotalEventCounter].Mode   = Modes->GetModeFromGenerator(std::abs(*Mode));
          TutorialSamples[TotalEventCounter].nutype = nutype_;
          TutorialSamples[TotalEventCounter].oscnutype = oscnutype_;
          TutorialSamples[TotalEventCounter].Sample = static_cast<int>(iSample);

          if (std::abs(*PDGLep) == 12 || std::abs(*PDGLep) == 14 || std::abs(*PDGLep) == 16) {
            TutorialSamples[TotalEventCounter].isNC = true;
          } else {
            TutorialSamples[TotalEventCounter].isNC = false;
          }

          if(isATM) {
            TutorialSamples[TotalEventCounter].TrueCosZenith = *trueCZ;
          }

          FillParticles(TotalEventCounter, nParticles, *PDGLep, *ELep, gen);
          // ==========================================
          TotalEventCounter++;
        }
      }
      MACH3LOG_INFO("Initialised channel: {}/{}", iChannel, GetNOscChannels(static_cast<int>(iSample)));
    }
  }
  return nEntries;
}

// ************************************************
void SampleHandlerTutorial::FillParticles(int eventIndex, int nParticles, int PDGLep, double ELep, std::mt19937& gen) {
// ************************************************
  std::uniform_int_distribution<> unif(0,3);
  std::normal_distribution<> mu_angle(0,M_PI/8);
  std::normal_distribution<> pi_angle(0,M_PI/2);
  std::uniform_real_distribution<> nucl_angle(-M_PI,M_PI);
  std::exponential_distribution<> pi_energy(1./0.5);
  std::exponential_distribution<> nucl_energy(1./2);

  // === JM loop through particles in event ===
  for (int iParticle = 0; iParticle < nParticles; ++iParticle) {
    //JM: No particle-level data in sample, so fake it
    if (iParticle==0) {
      TutorialPlottingSamples[eventIndex].particle_pdg[iParticle] = PDGLep;
      TutorialPlottingSamples[eventIndex].particle_energy[iParticle] = ELep;
      TutorialPlottingSamples[eventIndex].particle_beamangle[iParticle] = mu_angle(gen);
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
      TutorialPlottingSamples[eventIndex].particle_energy[iParticle] = energy;
      TutorialPlottingSamples[eventIndex].particle_beamangle[iParticle] = angle;
      TutorialPlottingSamples[eventIndex].particle_pdg[iParticle] = pdg;
    }
  }
}


// ************************************************
void SampleHandlerTutorial::InititialiseData() {
// ************************************************
  // Reweight MC to match
  Reweight();
  // set asimov data
  for(int iSample = 0; iSample < GetNSamples(); iSample++) {
    AddData(iSample, GetMCArray(iSample));
  }
}

double SampleHandlerTutorial::ReturnKinematicParameter(const int KinematicVariable, const int iEvent) const {
  const double* paramPointer = GetPointerToKinematicParameter(KinematicVariable, iEvent);
  return *paramPointer;
}

// === JM Define ReturnKinematicVector functions === 
std::vector<double> SampleHandlerTutorial::ReturnKinematicVector(const int KinematicVector, const int iEvent) const {
  switch (KinematicVector) {
    case kParticleEnergy:
      return TutorialPlottingSamples[iEvent].particle_energy;
    case kParticlePDG:
      return TutorialPlottingSamples[iEvent].particle_pdg;
    case kParticleBeamAngle:
      return TutorialPlottingSamples[iEvent].particle_beamangle;
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Vector: {}", KinematicVector);
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

// =================================================

const double* SampleHandlerTutorial::GetPointerToKinematicParameter(const int KinPar, const int iEvent) const {
  switch (KinPar) {
    case kTrueNeutrinoEnergy:
      return &TutorialSamples[iEvent].TrueEnu;
    case kRecoNeutrinoEnergy:
      // HH - here we return the shifted energy in case of detector systematics
      return &TutorialSamples[iEvent].RecoEnu_shifted;
    case kTrueQ2:
      return &TutorialSamples[iEvent].Q2;
    case kM3Mode:
      return &TutorialSamples[iEvent].Mode;
    case kOscChannel:
      return GetPointerToOscChannel(iEvent);
    case kTargetNucleus:
      return &TutorialSamples[iEvent].Target;
    default:
      MACH3LOG_ERROR("Unrecognized Kinematic Parameter type: {}", KinPar);
      throw MaCh3Exception(__FILE__, __LINE__);
  }
}

void SampleHandlerTutorial::SetupMC() {
  for(unsigned int iEvent = 0 ;iEvent < GetNEvents(); ++iEvent) {
    MCEvents[iEvent].enu_true = TutorialSamples[iEvent].TrueEnu;
    MCEvents[iEvent].isNC = TutorialSamples[iEvent].isNC;
    MCEvents[iEvent].nupdgUnosc = TutorialSamples[iEvent].nutype;
    MCEvents[iEvent].nupdg = TutorialSamples[iEvent].oscnutype;
    MCEvents[iEvent].NominalSample = TutorialSamples[iEvent].Sample;
    if(isATM) MCEvents[iEvent].coszenith_true = TutorialSamples[iEvent].TrueCosZenith;
  }
}
