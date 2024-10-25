#include <TFile.h>
#include <TObjArray.h>
#include <TGraph.h>
#include <TString.h>
#include <iostream>

void CreateSplineFile() {
  // Open the input file containing the graphs
  TFile *inputFile = TFile::Open("run2wMCsplines.root", "READ");
  if (!inputFile || inputFile->IsZombie()) {
    std::cerr << "Error: Cannot open input file!" << std::endl;
    return;
  }

  // Get the tree from the input file
  TTree *tree = (TTree*)inputFile->Get("sample_sum");
  if (!tree) {
    std::cerr << "Error: Cannot find tree 'sample_sum' in input file!" << std::endl;
    inputFile->Close();
    return;
  }

  // Names of the branches in the tree containing the graphs
  const char* graphNames[] = {
    "MAQEGraph",
    "PionFSI_AbsProbGraph",
    "DIS_BY_corrGraph",
    "MEC_lowEnuGraph"
  };
  const int nGraphs = sizeof(graphNames) / sizeof(graphNames[0]);

  // Array to hold the TObjArray pointers
  TObjArray* graphArrays[nGraphs] = {nullptr};

  // Set branch addresses
  for (int i = 0; i < nGraphs; ++i) {
    tree->SetBranchAddress(graphNames[i], &graphArrays[i]);
  }

  // Get the entry containing the graphs (assuming there's one entry)
  tree->GetEntry(0);

  // Open the output file where you will save the renamed graphs
  TFile *outputFile = TFile::Open("output_file.root", "RECREATE");
  if (!outputFile || outputFile->IsZombie()) {
    std::cerr << "Error: Cannot open output file!" << std::endl;
    inputFile->Close();
    return;
  }

  TClonesArray* OutgraphArrays[nGraphs] = {nullptr};

  TTree* ssum_tree = new TTree("sample_sum", "sample_sum");
  for (int i = 0; i < nGraphs; ++i) {
    OutgraphArrays[i] = new TClonesArray("TGraph", 1);
    TString newName = TString::Format("Spline_%i", i);
    ssum_tree->Branch(newName.Data(), "TClonesArray",
         &(OutgraphArrays[i]), 256000, 0);
  }
  // Loop over the TObjArray to save the graphs with new names
  for (int i = 0; i < 10000; ++i) {
    tree->GetEntry(i);

    // Loop over the graphs in the TObjArray
    for (int j = 0; j < nGraphs; ++j) {
      OutgraphArrays[j] = (TClonesArray*)graphArrays[j]->Clone();
    }

    // Write the graph to the output file
    ssum_tree->Fill();
  }

  outputFile->cd();
  ssum_tree->Write();

  // Clean up and close files
  outputFile->Close();
  inputFile->Close();

  std::cout << "Graphs saved successfully to output file!" << std::endl;
}
