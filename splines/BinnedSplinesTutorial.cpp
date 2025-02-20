#include "BinnedSplinesTutorial.h"
#include "manager/MaCh3Modes.h"
#include "samplePDF/StructsTutorial.h"


//MaCh3 core includes
#include <samplePDF/Structs.h>
#include <manager/MaCh3Logger.h>
//****************************************

_MaCh3_Safe_Include_Start_ //{
//ROOT include
#include "TKey.h"
#include "TROOT.h"
_MaCh3_Safe_Include_End_ //}

BinnedSplineTutorial::BinnedSplineTutorial(covarianceXsec *xsec_cov, MaCh3Modes* Modes_) : splineFDBase(xsec_cov, Modes_) {
}

BinnedSplineTutorial::~BinnedSplineTutorial() {
}

/*! ETA - moved this into experiment specific code since it relies on experimental MC format and also the experiment spline name mapping
 * Performs two jobs
 *  1. Fills indexing/each sample
 *  2. Creates the big spline vector
 */
void BinnedSplineTutorial::FillSampleArray(std::string SampleName, std::vector<std::string> OscChanFileNames)
{
  int iSample = getSampleIndex(SampleName);

  int nOscChannels = nOscChans[iSample];
  
  for (int iOscChan = 0; iOscChan < nOscChannels; iOscChan++) {
    MACH3LOG_INFO("Processing: {}", OscChanFileNames[iOscChan]);
    TSpline3* mySpline = nullptr;
    TSpline3_red* Spline = nullptr;
    TString Syst, Mode;
    int nKnots, SystNum, ModeNum, Var1Bin, Var2Bin, Var3Bin = M3::_BAD_INT_;
    double x,y, Eval = M3::_BAD_DOUBLE_;
    bool isFlat = true;

    auto File = std::unique_ptr<TFile>(TFile::Open(OscChanFileNames[iOscChan].c_str()));

    if (!File || File->IsZombie()) {
      MACH3LOG_ERROR("File {} not found", OscChanFileNames[iOscChan]);
      throw MaCh3Exception(__FILE__, __LINE__);
    }

    //This is the MC specific part of the code
    //i.e. we always assume that the splines are just store in  single TDirectory and they're all in there as single objects   
    for (auto k : *File->GetListOfKeys()) {
      auto Key = static_cast<TKey*>(k);
      TClass *Class = gROOT->GetClass(Key->GetClassName(), false);
      if(!Class->InheritsFrom("TSpline3")) {
        continue;
      }

      TString FullSplineName = static_cast<TString>(Key->GetName());
      MACH3LOG_DEBUG("FillSplineName is {}", FullSplineName);
      // First We split into binning and spline name
      TObjArray *tokens = FullSplineName.Tokenize(".");
      MACH3LOG_DEBUG("Number of tokens is {}", tokens->GetEntries());
      /*
       A little hacky but lets us grab both old + new splines
      */
      if(tokens->GetEntries()!=7){
        delete tokens;
        tokens = FullSplineName.Tokenize("_");
      }

      Syst = static_cast<TObjString*>(tokens->At(1))->GetString();
      Mode = static_cast<TObjString*>(tokens->At(2))->GetString();
      // Skip 3 because it's "sp"
      Var1Bin = static_cast<TObjString*>(tokens->At(4))->GetString().Atoi();
      Var2Bin = static_cast<TObjString*>(tokens->At(5))->GetString().Atoi();
      Var3Bin = 0;

      if (tokens->GetEntries() == 7)
      {
        Var3Bin = static_cast<TObjString*>(tokens->At(6))->GetString().Atoi();
      }

      SystNum = -1;
      for (unsigned iSyst = 0; iSyst < SplineFileParPrefixNames[iSample].size(); iSyst++) {
        if (strcmp(Syst, SplineFileParPrefixNames[iSample][iSyst].c_str()) == 0) {
          SystNum = iSyst;
          break;
        }
      }

      // If the syst doesn't match any of the spline names then skip it
      if (SystNum == -1){
        MACH3LOG_WARN("Couldn't match!!");
        MACH3LOG_DEBUG("Couldn't Match any systematic name in xsec yaml with spline name: {}" , FullSplineName.Data());
        continue;
      }

      ModeNum = -1;
      for (unsigned int iMode = 0; iMode < SplineModeVecs[iSample][SystNum].size(); iMode++) {
        if (strcmp(Mode, Modes->GetSplineSuffixFromMaCh3Mode(SplineModeVecs[iSample][SystNum][iMode]).c_str()) == 0) {
          ModeNum = iMode;
          break;
        }
      }

      if (ModeNum == -1) {
        MACH3LOG_ERROR("Couldn't find mode for {} in {}. Problem Spline is : {} ", Mode, Syst, FullSplineName);
        throw;
      }


      mySpline = Key->ReadObject<TSpline3>();

      if (isValidSplineIndex(SampleName, iOscChan, SystNum, ModeNum, Var1Bin, Var2Bin, Var3Bin))
      { // loop over all the spline knots and check their value
        MACH3LOG_DEBUG("Pushed back monolith for spline {}", FullSplineName);
        // if the value is 1 then set the flat bool to false
        nKnots = mySpline->GetNp();
        isFlat = true;
        for (int iKnot = 0; iKnot < nKnots; iKnot++)
        {
          mySpline->GetKnot(iKnot, x, y);

          Eval = mySpline->Eval(x);
          if (Eval < 0.99999 || Eval > 1.00001)
          {
            isFlat = false;
            break;
          }
        }

        //Rather than keeping a mega vector of splines then converting, this should just keep everything nice in memory!
        indexvec[iSample][iOscChan][SystNum][ModeNum][Var1Bin][Var2Bin][Var3Bin]=MonolithIndex;
        coeffindexvec.push_back(CoeffIndex);
        // Should save memory rather saving [x_i_0 ,... x_i_maxknots] for every spline!
        if (isFlat) {
          splinevec_Monolith.push_back(nullptr);
          delete mySpline;
        } else {
          Spline = new TSpline3_red(mySpline);//, SplineInterpolationTypes[iSample][SystNum]);
          delete mySpline;

          splinevec_Monolith.push_back(Spline);
          uniquecoeffindices.push_back(MonolithIndex); //So we can get the unique coefficients and skip flat splines later on!
          CoeffIndex+=nKnots;
        }
        //Incrementing MonolithIndex to keep track of number of valid spline indices
        MonolithIndex+=1;
      } else {
        //Potentially you are not a valid spline index
        delete mySpline;
      }
    }//End of loop over all TKeys in file

    //A bit of clean up
    File->Delete("*");
    File->Close();
  } //End of oscillation channel loop
}
