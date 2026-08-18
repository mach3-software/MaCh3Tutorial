#include "BinnedSplinesTutorial.h"
#include "Parameters/ParameterHandlerGeneric.h"
#include "Splines/BinnedSplineHandler.h"

BinnedSplineTutorial::BinnedSplineTutorial(ParameterHandlerGeneric *parameter_handler, MaCh3Modes* Modes_) : BinnedSplineHandler(parameter_handler, Modes_) {
}

BinnedSplineTutorial::~BinnedSplineTutorial() {
}

std::vector<std::string> BinnedSplineTutorial::GetTokensFromSplineName(const std::string& FullSplineName) {
  std::vector<std::string> ReturnVec(5);

  TObjArray *tokens = TString(FullSplineName).Tokenize(".");

  /// A little hacky but lets us grab both old + new splines
  if(tokens->GetEntries() != 7){
    delete tokens;
    tokens = TString(FullSplineName).Tokenize("_");
  }
  
  ReturnVec[TokenOrdering::kSystToken] = (static_cast<TObjString*>(tokens->At(1)))->GetString();
  ReturnVec[TokenOrdering::kModeToken] = (static_cast<TObjString*>(tokens->At(2)))->GetString();
  // Skip 3 because it's "sp"
  ReturnVec[TokenOrdering::kVarBinToken + 0] = (static_cast<TObjString*>(tokens->At(4)))->GetString();
  ReturnVec[TokenOrdering::kVarBinToken + 1] = (static_cast<TObjString*>(tokens->At(5)))->GetString();
  ReturnVec[TokenOrdering::kVarBinToken + 2] = "0";
  
  if (tokens->GetEntries() == 7) {
    ReturnVec[TokenOrdering::kVarBinToken + 2] = (static_cast<TObjString*>(tokens->At(6)))->GetString();
  }
  tokens->Delete();
  delete tokens;
  
  return ReturnVec;
}
