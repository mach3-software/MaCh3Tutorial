# CIValidations

## CovarianceValidations
* testing LLH for cov xsc and osc
* checking PCA initialisation and eigen values etc
* checking Adaptive initialisation
* checking if throwing works
* checking if dumping matrix to root file works
* checking if DetID operation return same number of params
* Checking if we can reproduce step proposal (fixed Random Number)

## FitterValidations
* Running MCMC, MinuitFit, PSO
* Running LLH scan and 2D LLH scan
* Running Drag Race
* Running GetStepScaleBasedOnLLHScan
* Start From Posterior Test

## LLHValidation
* Checking if LLH scan is identical to one stored

## MaCh3ModeValidations
* Checking if return MaCh3 modes are consistent etc

## NuMCMCvalidations
* Checking if MaCh3 chain can be converted to NuMCMC format

## NuOscillatorInterfaceValidations
* Checking MaCh3 interface to NuOscillator

## pValueValidations
* Checking if SampleSummary class works

## SplineValidations
* Initialisation of SplineMonolith
* Checking dumping flat tree works
* Checking if returned weights agree with previously stored weighs

### TODO
* add test with different interpolations

## SamplePDFValidations
* Dump and compare event weights
* Dump and compare weight for each event
* Compare LLH using different test statistics

## UnitTests

### manager_tests
* Test for YamlHelpers

### histogram_tests
* Test fo Histogram Utils and TH2Poly related stuff

## Python Tests

### test_fitter_sample_pdf
* Fit with sample pdf

### test_fitter_stat_only
* Fit with cov only

### test_splines
* Tesing spline interpolation
