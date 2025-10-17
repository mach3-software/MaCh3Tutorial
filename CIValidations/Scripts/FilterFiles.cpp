#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <vector>
#include <functional>
#include <iostream>

// Define a struct to hold all branch variables
struct EventData {
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
  bool flagCC1pi0;
  bool flagNC0pi;
  bool flagNC1pi0;
  float CosineZenith;
};

// Helper function to create and configure a TTree with all branches
TTree* CreateAndConfigureTree(TFile* file, const char* treeName, bool doATM, EventData& data) {
  TTree* tree = new TTree(treeName, treeName);
  tree->Branch("Enu_true", &data.Enu_true, "Enu_true/F");
  tree->Branch("Q2", &data.Q2, "Q2/F");
  tree->Branch("Mode", &data.Mode, "Mode/I");
  tree->Branch("tgt", &data.tgt, "tgt/I");
  tree->Branch("PDGLep", &data.PDGLep, "PDGLep/I");
  tree->Branch("ELep", &data.ELep, "ELep/F");
  tree->Branch("CosLep", &data.CosLep, "CosLep/F");
  tree->Branch("flagCC0pi", &data.flagCC0pi, "flagCC0pi/O");
  tree->Branch("flagCC1pip", &data.flagCC1pip, "flagCC1pip/O");
  tree->Branch("flagCC1pim", &data.flagCC1pim, "flagCC1pim/O");
  tree->Branch("flagCC1pi0", &data.flagCC1pi0, "flagCC1pi0/O");
  tree->Branch("flagNC0pi", &data.flagNC0pi, "flagNC0pi/O");
  tree->Branch("flagNC1pi0", &data.flagNC1pi0, "flagNC1pi0/O");
  if (doATM) tree->Branch("CosineZenith", &data.CosineZenith, "CosineZenith/F");
  return tree;
}

// Helper function to fill a tree with entries that match a condition
void FillTreeWithCondition(
  TTree* inputTree, TTree* outputTree,
  std::function<bool(const EventData&)> condition,
                           bool doATM, TRandom3& randGen, EventData& data
) {
  inputTree->SetBranchAddress("Enu_true", &data.Enu_true);
  inputTree->SetBranchAddress("Q2", &data.Q2);
  inputTree->SetBranchAddress("Mode", &data.Mode);
  inputTree->SetBranchAddress("tgt", &data.tgt);
  inputTree->SetBranchAddress("PDGLep", &data.PDGLep);
  inputTree->SetBranchAddress("ELep", &data.ELep);
  inputTree->SetBranchAddress("CosLep", &data.CosLep);
  inputTree->SetBranchAddress("flagCC0pi", &data.flagCC0pi);
  inputTree->SetBranchAddress("flagCC1pip", &data.flagCC1pip);
  inputTree->SetBranchAddress("flagCC1pim", &data.flagCC1pim);
  inputTree->SetBranchAddress("flagCC1pi0", &data.flagCC1pi0);
  inputTree->SetBranchAddress("flagNC0pi", &data.flagNC0pi);
  inputTree->SetBranchAddress("flagNC1pi0", &data.flagNC1pi0);

  Long64_t nEntries = inputTree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    inputTree->GetEntry(i);
    if (condition(data)) {
      if (doATM) data.CosineZenith = randGen.Uniform(-1.0, 1.0);
      outputTree->Fill();
    }
  }
}

void FilterFiles(bool doATM = false, bool doND = false) {
  // Open input file
  TFile* inputFile = TFile::Open("nuwro.flat.root", "READ");
  if (!inputFile || inputFile->IsZombie()) {
    std::cerr << "Error: Cannot open input file!" << std::endl;
    return;
  }
  TTree* inputTree = dynamic_cast<TTree*>(inputFile->Get("FlatTree_VARS"));
  if (!inputTree) {
    std::cerr << "Error: Cannot find the TTree 'FlatTree_VARS'!" << std::endl;
    inputFile->Close();
    return;
  }

  EventData data;
  TRandom3 randGen;

  // Create main output file and tree
  TFile* outputFile = TFile::Open("output.root", "RECREATE");
  TTree* outputTree = CreateAndConfigureTree(outputFile, "FlatTree_VARS", doATM, data);

  // Fill main output tree (all events)
  FillTreeWithCondition(
    inputTree, outputTree,
    [](const EventData&) { return true; },
                        doATM, randGen, data
  );

  // Create and fill ND-specific trees if doND is true
  std::vector<std::pair<TFile*, TTree*>> ndFiles;
  if (doND) {
    auto createNDTree = [&](const char* filename, auto condition) {
      TFile* f = TFile::Open(filename, "RECREATE");
      TTree* t = CreateAndConfigureTree(f, "FlatTree_VARS", doATM, data);
      FillTreeWithCondition(inputTree, t, condition, doATM, randGen, data);
      ndFiles.emplace_back(f, t);
    };

    // CC samples
    createNDTree("output_CC0pi.root", [](const EventData& d) { return d.flagCC0pi; });
    createNDTree("output_CC1pip.root", [](const EventData& d) { return d.flagCC1pip; });
    createNDTree("output_CCPi0.root", [](const EventData& d) { return d.flagCC1pi0; });

    // NC samples
    createNDTree("output_NC0pi.root", [](const EventData& d) { return d.flagNC0pi; });
    createNDTree("output_NC1pi0.root", [](const EventData& d) { return d.flagNC1pi0; });
  }

  // Write and close all files
  outputFile->cd();
  outputTree->Write();
  outputFile->Close();

  for (auto& [file, tree] : ndFiles) {
    file->cd();
    tree->Write();
    file->Close();
  }

  inputFile->Close();

  std::cout << "Filtered trees saved to output.root";
  if (doND)
    std::cout << ", plus ND-specific files: output_CC0pi.root, output_CC1pip.root, "
    "output_CCPi0.root, output_NC0pi.root, and output_NC1pi0.root";
  std::cout << std::endl;
}
