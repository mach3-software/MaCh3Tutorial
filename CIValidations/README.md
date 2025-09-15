# CIValidations

## CovarianceValidations
* Testing LLH for cov xsc and osc
* Checking PCA initialisation and eigen values etc
* Checking PCA with not all paramters being eigen decomposed
* Checking Adaptive initialisation and mean values
* Checking if throwing works
* Checking if dumping matrix to root file works
* Checking if DetID operation return same number of params
* Checking if we can reproduce step proposal (fixed Random Number)
* Checking if tune correctly set values
* Checking if modifying YAML file with tune or correlation matrix works

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

## SigmaVarValidations
* Checking if Sigma Var works

## PredictiveValidations
* Checking if Predictive Thrower produces same format and posterior predictive value and errors are identical using stored toys

## SplineValidations
* Initialisation of SplineMonolith
* Checking dumping flat tree works
* Checking if returned weights agree with previously stored weighs

## BinningValidations
* Checks if event migration works as intended

### TODO
* add test with different interpolations

## SamplePDFValidations
* Dump and compare event weights
* Dump and compare weight for each event
* Compare LLH using different test statistics
* Test shared Oscillator
* Test Sample PDF without Splines and Oscillator

## UnitTests

### manager_tests
* Test for YamlHelpers

### histogram_tests
* Test fo Histogram Utils and TH2Poly related stuff

### statistical_tests
* Test stuff like Bayesian statistics, pvalue etc.

## Python Tests

### test_fitter_sample_pdf
* Fit with sample pdf

### test_fitter_stat_only
* Fit with cov only

### test_splines
* Tesing spline interpolation
