#include "BinnedSplinesTutorial.h"
#include "manager/MaCh3Modes.h"
#include "samplePDF/StructsTutorial.h"


//MaCh3 core includes
#include <samplePDF/Structs.h>
#include <manager/MaCh3Logger.h>
//****************************************

//ROOT include
#include "TKey.h"
#include "TROOT.h"



BinnedSplineTutorial::BinnedSplineTutorial(covarianceXsec *xsec_cov) : splineFDBase(xsec_cov)
{
  MACH3LOG_INFO("Created BinnedSplineTutorial object");
}

BinnedSplineTutorial::~BinnedSplineTutorial()
{
  MACH3LOG_INFO("Deleting BinnedSplineTutorial object :'(");
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
    TSpline3* mySpline = new TSpline3();
    TSpline3_red* Spline = nullptr;
    TString Syst, Mode;
    int nKnots, SystNum, ModeNum, Var1Bin, Var2Bin, Var3Bin = _BAD_INT_;
    double x,y, Eval = _BAD_DOUBLE_;
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

      TString FullSplineName = (TString)Key->GetName();
      MACH3LOG_INFO("FillSplineName is {}", FullSplineName);
      // First We split into binning and spline name
      TObjArray *tokens = FullSplineName.Tokenize(".");
      MACH3LOG_INFO("Number of tokens is {}", tokens->GetEntries());
      /*
       A little hacky but lets us grab both old + new splines
      */
      if(tokens->GetEntries()!=7){
        delete tokens;
        tokens = FullSplineName.Tokenize("_");
      }

      Syst = ((TObjString *)(tokens->At(1)))->GetString();
      Mode = ((TObjString *)(tokens->At(2)))->GetString();
      // Skip 3 because it's "sp"
      Var1Bin = (((TObjString *)(tokens->At(4)))->GetString()).Atoi();
      Var2Bin = (((TObjString *)(tokens->At(5)))->GetString()).Atoi();
      Var3Bin = 0;

      if (tokens->GetEntries() == 7)
      {
        Var3Bin = (((TObjString *)(tokens->At(6)))->GetString()).Atoi();
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
        if (strcmp(Mode, ModeToString(SplineModeVecs[iSample][SystNum][iMode]).c_str()) == 0) {
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
        MACH3LOG_INFO("Pushed back monolith for spline {}", FullSplineName);
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

  return;
}

//****************************************
std::vector< std::vector<int> > BinnedSplineTutorial::GetEventSplines(std::string SampleName, int iOscChan, int EventMode, double Var1Val, double Var2Val, double Var3Val)
//****************************************
{
  std::vector<std::vector<int>> ReturnVec;
  int SampleIndex = -1;
  for (unsigned int iSample = 0; iSample < SampleNames.size(); iSample++) {
    if (SampleName == SampleNames[iSample]) {
      SampleIndex = iSample;
    }
  }

  if (SampleIndex == -1)
  {
    MACH3LOG_ERROR("Sample not found: {}", SampleName);
    throw MaCh3Exception(__FILE__, __LINE__);
  }

  int nSplineSysts = (int)indexvec[SampleIndex][iOscChan].size();
  //ETA- this is already a MaCh3 mode
  int Mode = EventMode;

  int Var1Bin = SplineBinning[SampleIndex][iOscChan][0]->FindBin(Var1Val)-1;
  if (Var1Bin < 0 || Var1Bin >= SplineBinning[SampleIndex][iOscChan][0]->GetNbins()){
    return ReturnVec;
  }

  int Var2Bin = SplineBinning[SampleIndex][iOscChan][1]->FindBin(Var2Val)-1;
  if (Var2Bin < 0 || Var2Bin >= SplineBinning[SampleIndex][iOscChan][1]->GetNbins()){
    return ReturnVec;
  }

  int Var3Bin = SplineBinning[SampleIndex][iOscChan][2]->FindBin(Var3Val)-1;

  if (Var3Bin < 0 || Var3Bin >= SplineBinning[SampleIndex][iOscChan][2]->GetNbins()){
    return ReturnVec;
  }

  for(int iSyst=0; iSyst<nSplineSysts; iSyst++){
    std::vector<int> spline_modes = SplineModeVecs[SampleIndex][iSyst];
    int nSampleModes = (int)spline_modes.size();

    //ETA - look here at the length of spline_modes and what you're actually comparing against
    for(int iMode = 0; iMode<nSampleModes ; iMode++){
      //Only consider if the event mode (Mode) matches ones of the spline modes
      if (Mode == spline_modes[iMode]) {
        std::vector<int> event_vec(7);
        event_vec[0]=SampleIndex;
        event_vec[1]=iOscChan;
        event_vec[2]=iSyst;
        event_vec[3]=iMode;
        event_vec[4]=Var1Bin;
        event_vec[5]=Var2Bin;
        event_vec[6]=Var3Bin;
        int splineID=indexvec[SampleIndex][iOscChan][iSyst][iMode][Var1Bin][Var2Bin][Var3Bin];
        //Also check that the spline isn't flat
        if(!isflatarray[splineID]){
          ReturnVec.push_back(event_vec);
        }
      }
    }
  }
  return ReturnVec;
}
