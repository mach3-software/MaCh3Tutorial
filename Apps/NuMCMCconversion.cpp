#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"

/// @todo actually save gaussian
/// @todo make sure delta_cp is properly initialised

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return 1;
  }

  double t_sin2th_23;
  double t_sin2th_13;
  double t_sin2th_12;
  double t_delm2_23;
  double t_delm2_12;
  double t_delta_cp;

  TChain* myChain = new TChain("posteriors","");
  myChain->Add(argv[1]);

  myChain->SetBranchStatus("*",0);
  myChain->SetBranchStatus("sin2th_23",1);
  myChain->SetBranchAddress("sin2th_23", &t_sin2th_23);
  myChain->SetBranchStatus("sin2th_13",1);
  myChain->SetBranchAddress("sin2th_13", &t_sin2th_13);
  myChain->SetBranchStatus("sin2th_12",1);
  myChain->SetBranchAddress("sin2th_12", &t_sin2th_12);
  myChain->SetBranchStatus("delm2_12",1);
  myChain->SetBranchAddress("delm2_12", &t_delm2_12);
  myChain->SetBranchAddress("delm2_23", &t_delm2_23);
  myChain->SetBranchStatus("delm2_23",1);
  myChain->SetBranchStatus("delta_cp",1);
  myChain->SetBranchAddress("delta_cp", &t_delta_cp);

  
  TFile *newfile = new TFile(argv[2],"recreate");
  TTree *newtree = new TTree("mcmc", "mcmc");

  newtree->Branch("Theta23",    &t_sin2th_23, "Theta23/D");
  newtree->Branch("Theta13",    &t_sin2th_13, "Theta13/D");
  newtree->Branch("Theta12",    &t_sin2th_12, "Theta12/D");
  newtree->Branch("Deltam2_32", &t_delm2_23,  "Deltam2_32/D");
  newtree->Branch("Deltam2_21", &t_delm2_12,  "Deltam2_21/D");
  newtree->Branch("DeltaCP",    &t_delta_cp, "DeltaCP/D");

  for (int i = 0; i < myChain->GetEntries(); ++i)
  {
    if (i % 10000 == 0)
    std::cout << i << std::endl;
    myChain->GetEntry(i);
    newtree->Fill();
  }

  newtree->Print();
  newtree->AutoSave();

  // List of parameter names and their corresponding titles
  std::vector<std::pair<std::string, std::string>> params = {
    {"Theta23", "Uniform:Theta23"},
    {"Theta13", "Uniform:Theta13"},
    {"Theta12", "Uniform:Theta12"},
    {"Deltam2_32", "Uniform:Deltam2_32"},
    {"Deltam2_21", "Uniform:Deltam2_21"},
    {"DeltaCP", "Uniform:DeltaCP"}
  };

  // Create a TList to hold the TNamed objects
  TList *paramList = new TList();

  // Create TNamed objects for each parameter and add them to the list
  for (const auto& param : params) {
    TNamed *namedParam = new TNamed(param.first.c_str(), param.second.c_str());
    paramList->Add(namedParam);  // Add to TList
  }

  // Write the TList to the directory
  paramList->Write("priors", TObject::kSingleKey);

  // Close the file
  newfile->Close();

  delete newfile;

  return 0;
}
