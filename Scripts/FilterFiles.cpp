#include <TFile.h>
#include <TTree.h>

void FilterFiles() {
  // Open the existing file and get the TTree
  TFile *inputFile = TFile::Open("Nuwro_RHC_NUISFLAT.root", "READ");
  if (!inputFile || inputFile->IsZombie()) {
    std::cerr << "Error: Cannot open input file!" << std::endl;
    return;
  }

  TTree *inputTree = dynamic_cast<TTree*>(inputFile->Get("FlatTree_VARS"));
  if (!inputTree) {
    std::cerr << "Error: Cannot find the TTree 'FlatTree_VARS'!" << std::endl;
    inputFile->Close();
    return;
  }

  // Variables to hold the values
  float Enu_true;
  float Q2;
  int Mode;
  int tgt;
  int PDGLep;
  float ELep;
  float CosLep;
  bool flagCC0pi;
  bool flagCC1pip;
  bool flagCC1pim;

  // Set branch addresses
  inputTree->SetBranchAddress("Enu_true", &Enu_true);
  inputTree->SetBranchAddress("Q2", &Q2);
  inputTree->SetBranchAddress("Mode", &Mode);
  inputTree->SetBranchAddress("tgt", &tgt);
  inputTree->SetBranchAddress("PDGLep", &PDGLep);
  inputTree->SetBranchAddress("ELep", &ELep);
  inputTree->SetBranchAddress("CosLep", &CosLep);
  inputTree->SetBranchAddress("flagCC0pi", &flagCC0pi);
  inputTree->SetBranchAddress("flagCC1pip", &flagCC1pip);
  inputTree->SetBranchAddress("flagCC1pim", &flagCC1pim);

  // Create a new file and new TTree
  TFile *outputFile = TFile::Open("output.root", "RECREATE");
  TTree *outputTree = new TTree("FlatTree_VARS", "FlatTree_VARS");

  // Create new branches for all the variables
  outputTree->Branch("Enu_true", &Enu_true, "Enu_true/F");
  outputTree->Branch("Q2", &Q2, "Q2/F");
  outputTree->Branch("Mode", &Mode, "Mode/I");
  outputTree->Branch("tgt", &tgt, "tgt/I");
  outputTree->Branch("PDGLep", &PDGLep, "PDGLep/I");
  outputTree->Branch("ELep", &ELep, "ELep/F");
  outputTree->Branch("CosLep", &CosLep, "CosLep/F");
  outputTree->Branch("flagCC0pi", &flagCC0pi, "flagCC0pi/O");
  outputTree->Branch("flagCC1pip", &flagCC1pip, "flagCC1pip/O");
  outputTree->Branch("flagCC1pim", &flagCC1pim, "flagCC1pim/O");

  // Loop over the entries and fill the new tree
  Long64_t nEntries = inputTree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    inputTree->GetEntry(i);
    outputTree->Fill();
  }

  // Write the output tree and close the files
  outputTree->Write();
  outputFile->Close();
  inputFile->Close();

  std::cout << "Filtered tree saved to output.root" << std::endl;
}
