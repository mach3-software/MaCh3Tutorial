// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "samplePDF/samplePDFTutorial.h"
#include "mcmc/SampleSummary.h"



class samplePDFpValue : virtual public samplePDFTutorial
{
  public:
    samplePDFpValue(std::string mc_version, covarianceXsec* xsec_cov)
    : samplePDFTutorial(mc_version, xsec_cov),
    samplePDFFDBase(mc_version, xsec_cov),
    SampleBlarbTitle({
      "FGD1_numuCC_0pi_0_protons_no_photon",
      "FGD1_numuCC_0pi_N_protons_no_photon",
      "FGD1_numuCC_1pi_no_photon",
      "FGD1_numuCC_other_no_photon",
      "FGD1_numuCC_photon",

      "FGD2_numuCC_0pi_0_protons_no_photon",
      "FGD2_numuCC_0pi_N_protons_no_photon",
      "FGD2_numuCC_1pi_no_photon",
      "FGD2_numuCC_other_no_photon",
      "FGD2_numuCC_photon",

      "FGD1_anti-numuCC_0pi",
      "FGD1_anti-numuCC_1pi",
      "FGD1_anti-numuCC_other",

      "FGD2_anti-numuCC_0pi",
      "FGD2_anti-numuCC_1pi",
      "FGD2_anti-numuCC_other",

      "FGD1_NuMuBkg_CC0pi_in_AntiNu_Mode",
      "FGD1_NuMuBkg_CC1pi_in_AntiNu_Mode",
      "FGD1_NuMuBkg_CCother_in_AntiNu_Mode",

      "FGD2_NuMuBkg_CC0pi_in_AntiNu_Mode",
      "FGD2_NuMuBkg_CC1pi_in_AntiNu_Mode",
      "FGD2_NuMuBkg_CCother_in_AntiNu_Mode"
    }),
    KinemBlarbTitle({"RecoLeptonMomentum", "RecoLeptonCosTheta"}) {}

    inline M3::int_t GetNsamples() override { return 22; };
    std::string GetSampleName(int Sample) override {return SampleBlarbTitle[Sample];};
    inline std::string GetKinVarLabel(const int sample, const int Dimension) override {return KinemBlarbTitle[Dimension];}

     inline void SetupBinning(const M3::int_t Selection, std::vector<double> &BinningX, std::vector<double> &BinningY) override{
      BinningX = {0., 350., 500., 600., 650., 700., 800., 900., 1000., 1150., 1250., 1500., 2000., 5000., 30000.};
      BinningY = {-1.0, 0.6, 0.7, 0.8, 0.85, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0};
    }

    std::vector<std::string> SampleBlarbTitle;
    std::vector<std::string> KinemBlarbTitle;
};




int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  std::vector<std::string> xsecCovMatrixFile = {"Inputs/SystematicModel.yaml"};
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  std::vector<std::string> OscCovMatrixFile = {"Inputs/Osc_Test.yaml"};
  covarianceOsc* osc = new covarianceOsc(OscCovMatrixFile, "osc_cov");
  osc->setParameters();

  std::string SampleConfig = "Inputs/SamplePDF_Tutorial.yaml";
  samplePDFpValue *SampleTutorial = new samplePDFpValue(SampleConfig, xsec);
  SampleTutorial->SetXsecCov(xsec);
  SampleTutorial->SetOscCov(osc);

  std::string name = SampleTutorial->GetName();
  TString NameTString = TString(name.c_str());

  // Reweight and process prior histogram
  SampleTutorial->reweight();
  TH1D *SampleHistogramPrior = (TH1D*)SampleTutorial->get1DHist()->Clone(NameTString + "_Prior");
  SampleTutorial->addData(SampleHistogramPrior);

  bool Posterior = true;
  bool FullLLH = false;
///////////////////////////////////////
  std::vector<TH2Poly*> NominalVector;
  std::vector<TH2Poly*> DataVector;
  std::vector<TH2Poly*> W2NomVector;

  std::vector<double> DrawVector;
  std::vector<double> PenaltyVector;
  std::vector<double> WeightVector;

  std::vector< std::vector<TH2Poly*> > MasterSampleVector;
  std::vector< std::vector<TH2Poly*> > MasterW2Vector;

  std::string PredictiveName = "CIValidations/TestOutputs/Toys.root";
  TFile* Outfile = new TFile(PredictiveName.c_str(), "READ");

  std::cout << "Found saved prior/posterior file " << PredictiveName << ", loading toys..." << std::endl;
  TStopwatch timer;
  timer.Start();

  // Know that these guys are going to be exist for every psyche sample, sometimes just NULL though
  NominalVector.resize(SampleTutorial->GetNsamples());
  DataVector.resize(SampleTutorial->GetNsamples());
  W2NomVector.resize(SampleTutorial->GetNsamples());

  // Finally get the TTree branch with the penalty vectors for each of the toy throws
  TTree *PenaltyTree = (TTree*)Outfile->Get("ToySummary");
  //KS:Number of entries in this tree is equal to toys
  int Ntoys = PenaltyTree->GetEntries();
  double Penalty, Weight;
  int Draw;
  int nEntries = 0;
  int NModelParams;
  PenaltyVector.resize(Ntoys);
  WeightVector.resize(Ntoys);
  if(Posterior) DrawVector.resize(Ntoys);

  // If the TTree exists we can read the penalty and draw number from there
  if (PenaltyTree != NULL) {
    PenaltyTree->SetBranchAddress("Penalty", &Penalty);
    PenaltyTree->SetBranchAddress("Weight", &Weight);
    if(Posterior) PenaltyTree->SetBranchAddress("Draw", &Draw);
    PenaltyTree->SetBranchAddress("NModelParams", &NModelParams);
    const int TreeEntries = PenaltyTree->GetEntries();
    if (TreeEntries != Ntoys) {
      std::cerr << "Number of entries in penalty tree != number of generated toys found" << std::endl;
      std::cerr << "TreeEntries: " << TreeEntries << std::endl;
      std::cerr << "nToys: " << Ntoys << std::endl;
      throw;
    }
    // Loop over the file and load entries into the penalty vector
    for (int i = 0; i < Ntoys; ++i) {
      PenaltyTree->GetEntry(i);
      if (FullLLH) {
        PenaltyVector[i] = Penalty;
      } else {
        PenaltyVector[i] = 0.0;
      }
      if(Posterior)
      {
        WeightVector[i] = Weight;
        DrawVector[i] = Draw;
        // Find the highest draw number to make the draw plots
        if (Draw > nEntries) nEntries = Draw;
      }
    }
  } else {
    for (int i = 0; i < Ntoys; ++i) {
      PenaltyVector[i] = 0.;
      WeightVector[i] = 1.;
      if(Posterior) DrawVector[i] = 1;
    }
  }
  delete PenaltyTree;

  //KS: Based on this thread https://root-forum.cern.ch/t/memory-leak-when-reading-th2poly-from-a-tree/50116/4
  Outfile->SetBufferSize(10 * 1024 * 1024);
  //First grab Data and nominal
  for (M3::int_t i = 0; i < SampleTutorial->GetNsamples(); ++i)
  {
    std::string Title = SampleTutorial->GetSampleName(i);
    // Replace the spaces with underscores
    std::replace(Title.begin(), Title.end(), ' ', '_');
    std::string FolderName = "Toys/";
    TH2Poly *DataTemp = (TH2Poly*)Outfile->Get((FolderName + Title + "_data").c_str());
    TH2Poly *NomTemp = (TH2Poly*)Outfile->Get((FolderName + Title+"_nom").c_str());
    TH2Poly *W2Temp = (TH2Poly*)Outfile->Get((FolderName + Title+"_W2nom").c_str());

    TH2Poly *Data = nullptr;
    TH2Poly *Nom = nullptr;
    TH2Poly *W2 = nullptr;

    // Pushing back NULL pointers are fine here, it's what we want
    if (DataTemp != nullptr)
    {
      Data = (TH2Poly*)DataTemp->Clone();
      Data->SetDirectory(0);
      delete DataTemp;
    }
    if (NomTemp != nullptr)
    {
      Nom = (TH2Poly*)NomTemp->Clone();
      Nom->SetDirectory(0);
      delete NomTemp;
    }
    if (W2Temp != nullptr)
    {
      W2 = (TH2Poly*)W2Temp->Clone();
      W2->SetDirectory(0);
      delete W2Temp;
    }
    DataVector[i] = Data;
    NominalVector[i] = Nom;
    W2NomVector[i] = W2;
  }

  //Now grab each toy, keeping in mind that AddThrow requires [toy][sample] which isn't optimal for cache
  MasterSampleVector.resize(Ntoys);
  MasterW2Vector.resize(Ntoys);
  for (int i = 0; i < Ntoys; ++i)
  {
    if (i % 100 == 0) std::cout << "   Loaded toy " << i << std::endl;

    std::vector<TH2Poly*> SampleVector;
    std::vector<TH2Poly*> W2Vector;
    std::vector< std::vector<TH2Poly*> > SampleVector_ByMode;
    SampleVector.resize(SampleTutorial->GetNsamples());
    W2Vector.resize(SampleTutorial->GetNsamples());
    for (M3::int_t j = 0; j < SampleTutorial->GetNsamples(); ++j)
    {
      if(DataVector[j] != NULL)
      {

        std::string FolderName = "Toys/";
        std::string Title = SampleTutorial->GetSampleName(j);
        // Replace the spaces with underscores
        std::replace(Title.begin(), Title.end(), ' ', '_');
        std::stringstream ss;
        ss << "_" << i;
        TH2Poly *ToyTemp = (TH2Poly*)Outfile->Get((FolderName+Title+ss.str()).c_str());
        if(ToyTemp == nullptr) { std::cerr<<"Empty toy, arghhh"<<std::endl; throw;}
        TH2Poly *Toy = (TH2Poly*)ToyTemp->Clone();
        Toy->SetDirectory(0);
        delete ToyTemp;

        TH2Poly *ToyW2Temp = (TH2Poly*)Outfile->Get((FolderName+Title+ss.str()+"w2").c_str());
        if(ToyW2Temp == nullptr) { std::cerr<<"Empty toy, arghhh"<<std::endl; throw;}
        TH2Poly * ToyW2 = (TH2Poly*)ToyW2Temp->Clone();
        ToyW2->SetDirectory(0);
        delete ToyW2Temp;

        SampleVector[j] = Toy;
        W2Vector[j] = ToyW2;
      }
      else
      {
        SampleVector[j] = NULL;
        W2Vector[j] = NULL;
      }
    }

    MasterSampleVector[i] = SampleVector;
    MasterW2Vector[i] = W2Vector;
  }
  std::cout << "Found total " << Ntoys << " pregenerated toys in " << Outfile->GetName() << std::endl;
  timer.Stop();
  std::cout << "Loading it took " << timer.RealTime() << "s" << std::endl;
  Outfile->Close();
  delete Outfile;
  Outfile = NULL;














///////////////////////////////////////
  SampleSummary* SamplesSumm = nullptr;
  if(Posterior) SamplesSumm = new SampleSummary(SampleTutorial->GetNsamples(), PredictiveName, SampleTutorial, nEntries);
  else SamplesSumm = new SampleSummary(SampleTutorial->GetNsamples(), PredictiveName, SampleTutorial, 0);
  SamplesSumm->SetLikelihood(kBarlowBeeston);
  SamplesSumm->SetNModelParams(NModelParams);

  // Add in the Data and nominal to the SampleSummary class
  SamplesSumm->AddData(DataVector);
  SamplesSumm->AddNominal(NominalVector,W2NomVector);

  // Now we have the MasterSampleVector filled with our toys, add the throws
  //KS: Be careful with multithreading here as AddThrow is multithreading
  for (int i = 0; i < Ntoys; ++i)
  {
    if(Posterior) SamplesSumm->AddThrow(MasterSampleVector[i], MasterW2Vector[i], PenaltyVector[i], WeightVector[i], DrawVector[i]);
    else SamplesSumm->AddThrow(MasterSampleVector[i], MasterW2Vector[i], PenaltyVector[i], WeightVector[i], 0);
  }
  timer.Stop();
  std::cout << "Finished adding throws to SampleSummary, it took " << timer.RealTime() << "s" <<", now writing..." << std::endl;

  // Calculate the posterior predictive p-values and write the samples to file
  SamplesSumm->Write();

  // Clean up dynamically allocated Sample if needed
  delete SamplesSumm;
  delete SampleTutorial;
  delete xsec;
  delete osc;

  return 0;
}