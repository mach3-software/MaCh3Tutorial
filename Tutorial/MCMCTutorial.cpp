// MaCh3 spline includes
#include "mcmc/MaCh3Factory.h"
#include "covariance/covarianceXsec.h"
#include "samplePDF/samplePDFTutorial.h"

int main(int argc, char *argv[]){

  MaCh3Utils::MaCh3Usage(argc, argv);
  // Initialise manger responsible for config handling
  manager *FitManager = new manager(argv[1]);

  auto xsecCovMatrixFile = FitManager->raw()["General"]["Systematics"]["XsecCovFile"].as<std::vector<std::string>>();
  // Initialise covariance class reasonable for Systematics
  covarianceXsec* xsec = new covarianceXsec(xsecCovMatrixFile, "xsec_cov");

  auto OscMatrixFile = FitManager->raw()["General"]["Systematics"]["OscCovFile"].as<std::vector<std::string>>();
  covarianceOsc* osc = new covarianceOsc(OscMatrixFile, "osc_cov");

  // Create MCMC Class
  std::unique_ptr<FitterBase> MaCh3Fitter = std::make_unique<mcmc>(FitManager);
  // Add covariance to MCM
  MaCh3Fitter->addSystObj(xsec);
  MaCh3Fitter->addSystObj(osc);

  auto SampleConfig = FitManager->raw()["General"]["TutorialSamples"].as<std::vector<std::string>>();
  for (size_t i = 0; i < SampleConfig.size(); ++i)
  {
    samplePDFTutorial* Sample = new samplePDFTutorial(SampleConfig[i], xsec);
    Sample->SetXsecCov(xsec);
    Sample->SetOscCov(osc);
    Sample->reweight();

    // Obtain sample name and create a TString version for histogram naming
    std::string name = Sample->GetName();
    TString NameTString = TString(name.c_str());

    // Clone the 1D histogram with a modified name
    TH1D* SampleHistogramPrior = static_cast<TH1D*>(Sample->get1DHist()->Clone(NameTString + "_Prior"));
    Sample->addData(SampleHistogramPrior);

    // Add the cloned histogram to the Sample object
    MaCh3Fitter->addSamplePDF(Sample);
  }

  // Run MCMCM
  MaCh3Fitter->runMCMC();

  delete FitManager;
  delete xsec;
  MaCh3Fitter.reset();
  return 0;
}
