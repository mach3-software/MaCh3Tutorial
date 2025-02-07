#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"
#include "TObjString.h"

#include "mcmc/MaCh3Factory.h"

int main(int argc, char *argv[])
{
  if (argc != 3) {
    MACH3LOG_CRITICAL("Not enough arguments");
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }

  double t_sin2th_23;
  double t_sin2th_13;
  double t_sin2th_12;
  double t_delm2_23;
  double t_delm2_12;
  double t_delta_cp;

  TChain* myChain = new TChain("posteriors","");
  myChain->Add(argv[1]);

  std::map<std::string, std::string> branch_map = {
    {"sin2th_23", "Theta23"},
    {"sin2th_13", "Theta13"},
    {"sin2th_12", "Theta12"},
    {"delm2_12",  "Deltam2_21"},
    {"delm2_23",  "Deltam2_32"},
    {"delta_cp",  "DeltaCP"}
  };

  // Map for pointers to the original branches
  std::map<std::string, double*> branch_pointers = {
    {"sin2th_23", &t_sin2th_23},
    {"sin2th_13", &t_sin2th_13},
    {"sin2th_12", &t_sin2th_12},
    {"delm2_12",  &t_delm2_12},
    {"delm2_23",  &t_delm2_23},
    {"delta_cp",  &t_delta_cp}
  };

  // Disable all branches by default
  myChain->SetBranchStatus("*", 0);

  // Enable branches based on map and set their addresses
  for (const auto& [old_name, new_name] : branch_map) {
    myChain->SetBranchStatus(old_name.c_str(), 1);
    myChain->SetBranchAddress(old_name.c_str(), branch_pointers[old_name]);
  }

  /// Load cov osc
  TFile *TempFile = new TFile(argv[1], "open");
  TDirectory* CovarianceFolder = (TDirectory*)TempFile->Get("CovarianceFolder");

  TMacro *OscConfig = (TMacro*)(CovarianceFolder->Get("Config_osc_cov"));

  YAML::Node OscSettings = TMacroToYAML(*OscConfig);

  // Create the new file and tree
  TFile* newfile = new TFile(argv[2], "recreate");
  TTree* newtree = new TTree("mcmc", "mcmc");

  // Create new branches in new tree with the mapped names
  for (const auto& [old_name, new_name] : branch_map) {
    newtree->Branch(new_name.c_str(), branch_pointers[old_name], (new_name + "/D").c_str());
  }

  for (int i = 0; i < myChain->GetEntries(); ++i)
  {
    if (i % 10000 == 0) std::cout << i << std::endl;
    myChain->GetEntry(i);

    t_sin2th_23 = std::asin(std::sqrt(t_sin2th_23));
    t_sin2th_13 = std::asin(std::sqrt(t_sin2th_13));
    t_sin2th_12 = std::asin(std::sqrt(t_sin2th_12));

    newtree->Fill();
  }

  newtree->Print();
  newtree->AutoSave();

  // List of parameter names and corresponding titles
  std::vector<std::pair<std::string, std::string>> params;

  // Iterate over each systematic entry in the YAML

  for (auto const &entry : OscSettings["Systematics"])
  {
    std::string param_name = entry["Systematic"]["Names"]["ParameterName"].as<std::string>();
    bool flat_prior = entry["Systematic"]["FlatPrior"].as<bool>();
    std::string mapped_name = branch_map[param_name];  // Get the mapped name
    if(mapped_name == "") continue;
    std::string distribution;

    if (param_name.rfind("sin", 0) == 0) {
      if (flat_prior) {
        distribution = "Uniform:sin^2(" + mapped_name + ")";
      } else {
        double prefit_value = entry["Systematic"]["ParameterValues"]["PreFitValue"].as<double>();
        double error = entry["Systematic"]["Error"].as<double>();
        distribution = "Gaussian(" + std::to_string(prefit_value) + ", " + std::to_string(error) + "):sin^2(" + mapped_name + ")";
      }
    } else {
      if (flat_prior) {
        distribution = "Uniform:" + mapped_name;
      } else {
        double prefit_value = entry["Systematic"]["ParameterValues"]["PreFitValue"].as<double>();
        double error = entry["Systematic"]["Error"].as<double>();
        distribution = "Gaussian(" + std::to_string(prefit_value) + ", " + std::to_string(error) + "):" + mapped_name;
      }
    }
    params.emplace_back(mapped_name, distribution);
  }

  // Create a TList to hold the TNamed objects
  TList *paramList = new TList();

  // Create TNamed objects for each parameter and add them to the list
  for (const auto& param : params) {
    TNamed *namedParam = new TNamed(param.first.c_str(), param.second.c_str());
    paramList->Add(namedParam);  // Add to TList
  }

  // Write the TList to the directory
  paramList->Write("priors", TObject::kSingleKey);


  TObjString doi("https://doi.org/10.5281/zenodo.13642670");

  doi.Write("citation");

  // Close the file
  newfile->Close();

  delete newfile;

  return 0;
}
