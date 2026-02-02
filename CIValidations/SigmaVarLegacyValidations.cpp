// MaCh3 spline includes
#include "Utils/Comparison.h"
#include "SamplesTutorial/SampleHandlerTutorial.h"
#include "Fitters/SampleSummary.h"
#include "Fitters/MaCh3Factory.h"

class samplePDFSigmaVar : public SampleHandlerBase
{
  public:
    samplePDFSigmaVar() :
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
    KinemBlarbTitle({"RecoLeptonMomentum", "RecoLeptonCosTheta"})
    {
      TRandom3 rng(42);
      PolyHist.resize(GetNsamples());
      for(int iSam = 0; iSam < GetNsamples(); iSam++)
      {
        auto BinArray_x = ReturnKinematicParameterBinning(0, GetKinVarName(0, 0));
        auto BinArray_y = ReturnKinematicParameterBinning(0, GetKinVarName(0, 1));

        PolyHist[iSam] = new TH2Poly();
        PolyHist[iSam]->SetName(SampleBlarbTitle[iSam].c_str());
        PolyHist[iSam]->SetTitle(SampleBlarbTitle[iSam].c_str());
        PolyHist[iSam]->SetDirectory(nullptr);
        double xmax, xmin, ymax, ymin;
        for(unsigned int iy = 0; iy < BinArray_y.size()-1; iy++)
        {
          ymax = BinArray_y[iy+1];
          ymin = BinArray_y[iy];
          for(unsigned int ix = 0; ix < BinArray_x.size()-1; ix++)
          {
            xmax = BinArray_x[ix+1];
            xmin = BinArray_x[ix];
            double binofx[] = {xmin, xmax, xmax, xmin};
            double binofy[] = {ymin, ymin, ymax, ymax};
            PolyHist[iSam]->AddBin(4,binofx,binofy);
          }
        }
        // Fixed seed for reproducibility

        for (int i = 1; i <= PolyHist[iSam]->GetNumberOfBins(); i++) {
          const double value = rng.Uniform(0.0, 1.0);
          PolyHist[iSam]->SetBinContent(i, value);
        }
        for (int i = 1; i <= PolyHist[iSam]->GetNumberOfBins(); i++) {
          const double value = rng.Uniform(0.0, 1.0);
          PolyHist[iSam]->SetBinError(i, value);
        }
      }
    } // end constructor

    inline M3::int_t GetNsamples() override { return 22; };
    std::string GetSampleTitle(int Sample) const override {return SampleBlarbTitle[Sample];};
    std::string GetKinVarName(const int sample, const int Dimension) const override {return KinemBlarbTitle[Dimension];}

    std::vector<double> ReturnKinematicParameterBinning(const int Sample, const std::string &KinematicParameter) const override {
      if(KinematicParameter == "RecoLeptonMomentum") {
        return {0., 350., 500., 600., 650., 700., 800., 900., 1000., 1150., 1250., 1500., 2000., 5000., 30000.};
      } else if(KinematicParameter == "RecoLeptonCosTheta") {
        return {-1.0, 0.6, 0.7, 0.8, 0.85, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0};
      }
    }

    void CleanMemoryBeforeFit() override {};
    std::string GetName() const override {return "samplePDFSigmaVar";};
    double GetLikelihood() const override {return 666;};
    double GetSampleLikelihood(const int iSample) const override {return iSample;};
    int GetNOscChannels(const int iSample) const override {return iSample;};
    void Reweight() override {return;};

    void PrintRates(const bool DataOnly = false) override {return;};

    TH1* GetDataHist(const int Selection) override   {return PolyHist[Selection];}
    TH1* GetMCHist(const int Selection) override    {return PolyHist[Selection];}
    TH1* GetW2Hist(const int Selection) override {return PolyHist[Selection];}
    TH1* GetPDFMode(const int Selection, const int Mode) override {return PolyHist[Selection];}

    std::vector<std::string> SampleBlarbTitle;
    std::vector<std::string> KinemBlarbTitle;
    std::vector<TH2Poly*> PolyHist;
};

int main(int argc, char *argv[])
{
  SetMaCh3LoggerFormat();

  if (argc != 1) {
    MACH3LOG_CRITICAL("You specified arguments, but none are needed. (Program name: {})", argv[0]);
    throw MaCh3Exception(__FILE__ , __LINE__ );
  }
  std::string TutorialPath = std::getenv("MaCh3Tutorial_ROOT");

  std::vector<std::string> xsecCovMatrixFile = {TutorialPath + "/TutorialConfigs/CovObjs/SystematicModel.yaml",
                                                TutorialPath + "/TutorialConfigs/CovObjs/OscillationModel.yaml"
                                                };
  auto xsec = std::make_unique<ParameterHandlerGeneric>(xsecCovMatrixFile, "xsec_cov");

  auto SampleTutorial = std::make_unique<samplePDFSigmaVar>();

  std::string ManagerInput = TutorialPath + "/TutorialConfigs/FitterConfig.yaml";
  auto FitManager = std::make_unique<Manager>(ManagerInput);

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(xsec.get());
  MaCh3Fitter->AddSampleHandler(SampleTutorial.get());

  MaCh3Fitter->RunSigmaVarLegacy();

  return 0;
}
