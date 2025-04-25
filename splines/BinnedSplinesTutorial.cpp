#include "BinnedSplinesTutorial.h"

BinnedSplineTutorial::BinnedSplineTutorial(covarianceXsec *xsec_cov, MaCh3Modes* Modes_) : splineFDBase(xsec_cov, Modes_) {
}

BinnedSplineTutorial::~BinnedSplineTutorial() {
}

std::vector<std::string> BinnedSplineTutorial::GetTokensFromSplineName(std::string FullSplineName) {
  std::vector<std::string> ReturnVec(TokenOrdering::kNTokens);

  TObjArray *tokens = TString(FullSplineName).Tokenize(".");
  /*
    A little hacky but lets us grab both old + new splines
  */
  if(tokens->GetEntries()!=7){
    delete tokens;
    tokens = TString(FullSplineName).Tokenize("_");
  }
  
  ReturnVec[TokenOrdering::kSystToken] = (static_cast<TObjString*>(tokens->At(1)))->GetString();
  ReturnVec[TokenOrdering::kModeToken] = (static_cast<TObjString*>(tokens->At(2)))->GetString();
  // Skip 3 because it's "sp"
  ReturnVec[TokenOrdering::kVar1BinToken] = (static_cast<TObjString*>(tokens->At(4)))->GetString();
  ReturnVec[TokenOrdering::kVar2BinToken] = (static_cast<TObjString*>(tokens->At(5)))->GetString();
  ReturnVec[TokenOrdering::kVar3BinToken] = "0";
  
  if (tokens->GetEntries() == 7) {
    ReturnVec[TokenOrdering::kVar3BinToken] = (static_cast<TObjString*>(tokens->At(6)))->GetString();
  }
  tokens->Delete();
  delete tokens;
  
  return ReturnVec;
}
