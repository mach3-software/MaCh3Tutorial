# Welcome to MaCh3 Tutorial
After this tutorial you should know how to run MCMC, implement systematic uncertainties, new samples, and how to plot standard Bayesian diagnostics.

The current setup includes a single sample with several cross-section and oscillation parameters.

MaCh3 is predominantly C++ software, although some functionality is available through python as well. See the table of contents for more.

[![Code - Documented](https://img.shields.io/badge/Code-Documented-2ea44f)](https://github.com/mach3-software/MaCh3/wiki)
[![Container Image](https://img.shields.io/badge/Container-Image-brightgreen)](https://github.com/mach3-software/MaCh3Tutorial/pkgs/container/mach3tutorial)
[![Code - Doxygen](https://img.shields.io/badge/Code-Doxygen-2ea44f)](https://mach3-software.github.io/MaCh3/index.html)

## Table of contents
1. [How to Start?](#how-to-start)
2. [How to Run MCMC](#how-to-run-mcmc)
    1. [Config Overrides](#config-overrides)
    2. [Processing MCMC Outputs](#processing-mcmc-outputs)
    3. [Plotting MCMC Posteriors](#plotting-mcmc-posteriors)
    4. [Plotting Correlation Matrix](#plotting-correlation-matrix)
3. [Posterior Predictive Analysis](#posterior-predictive-analysis)
    1. [Plotting Posterior Predictive Distributions](#plotting-posterior-predictive-distributions)
    2. [Prior Predictive Distributions](#prior-predictive-distributions)
4. [How to Develop a Model of Systematic Uncertainties](#how-to-develop-a-model-of-systematic-uncertainties)
    1. [How to Compare Chains](#how-to-compare-chains)
    2. [Holding Parameters Fixed](#holding-parameters-fixed)
5. [How to Develop New Samples](#how-to-develop-new-samples)
    1. [Modifying Analysis Samples](#modifying-analysis-samples)
    2. [Adding a New Sample](#adding-a-new-sample)
    3. [Changing Oscillation Engine](#changing-oscillation-engine)
    4. [Atmospheric Sample](#atmospheric-sample)
    5. [Plotting Kinematic Distribution](#plotting-kinematic-distribution)
    6. [More Advanced Development](#more-advanced-development)
6. [MCMC Diagnostic](#mcmc-diagnostic)
    1. [Running Multiple Chains](#running-multiple-chains)
7. [Useful Settings](#useful-settings)
8. [Other Useful Plots](#other-useful-plots)
    1. [LLH scans](#llh-scans)
    2. [Sigma Variations](#sigma-variations)

## How to Start?
To compile simply
```bash
git clone https://github.com/mach3-software/MaCh3Tutorial.git
mkdir build;
cd build;
cmake ../ -DPYTHON_ENABLED=ON [DPYTHON_ENABLED not mandatory]
make -jN [set number of threads]
make install
```
then
```bash
source bin/setup.MaCh3.sh
source bin/setup.MaCh3Tutorial.sh
```
alternatively you can use containers by
```bash
docker pull ghcr.io/mach3-software/mach3tutorial:alma9latest
```
To read more about how to use containers, check our wiki [here](https://github.com/mach3-software/MaCh3/wiki/12.-Containers)

## How to run MCMC
To run MCMC simply
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml
```
Congratulations! 🎉
You have just finished your first MCMC chain. You can view `Test.root` for example in TBrowser like in plot below.

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/6d4c97e2-f36b-456e-8e7b-1751f142d2ac">

A single entry in the tree represents a single MCMC step. Other than debug purposes, it is highly recommended to use MaCh3 processing tools for further visualisation.

**WARNING** Your posterior may look very shaky and slightly different to the one in example. This is because you have run the chain with low number of steps, meaning you don't have enough statistic to build out the posterior distribution.
It is good homework to increase the number of steps and see how much smoother the posterior becomes, but at the cost of having to wait more. You can easily test this by modifying `TutorialConfigs/FitterConfig.yaml` to change the number of MCMC steps:
```yaml
General:
  MCMC:
    NSteps: 10000
```
and then re-running `MCMCTutorial`.

**Warning**: If you modified files in the main `MaCh3Tutorial` folder instead of `build`, you will have to call `make install` for the changes to propagate! Generally speaking, it is good practice to work from the `build` directory and make your config changes there so that local changes do not have to be tracked by git.

### Config Overrides
Instead of changing the config file `TutorialConfigs/FitterConfig.yaml` above directly, you can instead dynamically override your configurations at the command line like this:
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml General:MCMC:NSteps:100000
```
In this way, you can add as many configuration overrides here as you like, as long as the format is `[executable] [config] ([option1] [option2] ...)`.

**Warning** This overriding process is only possible for the "main config" (i.e., configs that respond directly to the `Manager` class in MaCh3 core). This main config is used with the apps in the `Tutorial` folder here;
for the other apps (mainly plotting apps like `PredictivePlotting`) that read from `bin/TutorialDiagConfig.yaml`, this is not possible, as further command line arguments are interpreted as *input ROOT files*, not override directives.

### Processing MCMC Outputs
Being able to visualise and analyse the output of the MCMC is standard procedure after a chain has finished. MaCh3 uses `ProcessMCMC` to transform the raw output from the MCMC into smaller outputs that are more easily digested by downstream plotting macros. You can run it using
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root
```
where `Test.root` is the output of running `MCMCTutorial` as described [here](#how-to-run-mcmc). You can find some quickly-generated plots in `Test_drawCorr.pdf`. One of plots you will encounter is:

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/1073a76e-5d82-4321-8952-e098d1b0717f">

This is the marginalised posterior of a single parameter. This is the main output of the MCMC.

There are many options in `ProcessMCMC` that allow you to make many more analysis plots from the MCMC output; we recommend that you see [here](https://github.com/mach3-software/MaCh3/wiki/09.-Bayesian-Analysis,-Plotting-and-MCMC-Processor) to get better idea what each plot means. In particular, we recommend comparing 2D posteriors with the correlation matrix, and playing with triangle plots.

### Plotting MCMC Posteriors
Once you have the processed MCMC output from `ProcessMCMC`, which will be called something like `<inputName>_Process.root`, you can make fancier analysis plots from it using the `GetPostfitParamPlots` app like:

```bash
./bin/GetPostfitParamPlots Test_drawCorr.root
```

The output should look like plot below. This conveys same information as the individual posteriors from `Test_drawCorr.pdf`, but is more compact. This is useful especially if you have hundreds of parameters to compare.

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/27e3e4c8-629e-4c05-ac64-f6a7bec85331">

### Plotting Correlation Matrix
If you have run `ProcessMCMC` with option "PlotCorr" you will have a correlation matrix in the outputs. This is a handy tool for viewing how correlated different parameters are.
However, mature analyses with hundreds of parameters may run into the problem of having too large of plots to be useful. To combat this, you can plot a subset of parameters using `MatrixPlotter`:
```bash
./bin/MatrixPlotter bin/TutorialDiagConfig.yaml Test_drawCorr.root
```

This macro will give you `MatrixPlot.pdf`, where you should see a plot like this:

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/14471069-27e7-4ea3-9d75-232615aa246a">

In this particular example, you can see only two parameters. Using TutorialDiagConfig.yaml you can easily modify it either by adding more Titles, or more parameters. For example:
```yaml
MatrixPlotter:
  Titles: [
    "Norm",
  ]
  Norm: ["Norm_Param_0", "Norm_Param_1", "BinnedSplineParam1",
  ]
```

## Posterior Predictive Analysis
Since MCMC produces a full posterior distribution rather than a single best-fit value, one needs to use a Posterior Predictive Analysis (PPA) to produce spectra after the fit. The idea is to draw parameter sets from the MCMC chains and generate a toy spectrum for each set.
The final distribution of spectra is then obtained from all these toy spectra, reflecting the full uncertainty encoded in the posterior.

Once you run MCMC you can produce these toy distributions using following command:
```bash
./bin/PredictiveTutorial TutorialConfigs/FitterConfig.yaml General:OutputFile:PredictiveOutputTest.root
```

There are several settings in the configs you can control. Here, be aware that `PosteriorFile` points to the MCMC output, while `OutputFile` points to the output of `PredictiveTutorial` we are trying to generate. So in this case,
```yaml
Predictive:
  PosteriorFile: "Test.root"
```
refers to the specific output from `MCMCTutorial` above.

The ROOT output from this step will look something like:

<img width="350" alt="PostPred" src="https://github.com/user-attachments/assets/d7b70e6d-0802-4816-89ee-6d11ee89047b">

### Plotting Posterior Predictive Distributions
Once you have generated the posterior predictive toy distributions with `PredictiveTutorial`, you can make fancy plots of them using:
```bash
./bin/PredictivePlotting ./bin/TutorialDiagConfig.yaml PredictiveOutputTest.root
```
where `PredictiveOutputTest.root` is the output of `PredictiveTutorial` above.

The output will look like:

<img width="350" alt="PostPredPlot" src="https://github.com/user-attachments/assets/1f9bad86-4b53-453a-919f-4b837495b60c">

### Prior Predictive Distributions
The above steps focused on Posterior Predictive distributions, whereby we see the uncertainty in the spectrum after the parameters are constrained by the MCMC. You can use a similar setup to run Prior Predictve Analysis, where we check the spectrum uncertainty coming from the parameter priors
(i.e., with no constraint from the fit). The main difference here is that we throw from the parameter prior covariance matrix instead of the posterior chain.

To do this, we need to change a single parmater in `TutorialConfigs/FitterConfig.yaml`:
```yaml
# Prior/Posterior predictive settings
Predictive:
  # If false will run posterior predictive
  PriorPredictive: true
```

We can run it with following command:
```bash
./bin/PredictiveTutorial TutorialConfigs/FitterConfig.yaml General:OutputFile:PriorPredictiveOutputTest.root Predictive:PriorPredictive:True
```

Finally, we can compare the prior and posterior predictive spectra with the previously used `PredictivePlotting` macro:
```bash
./bin/PredictivePlotting ./bin/TutorialDiagConfig.yaml PredictiveOutputTest.root PriorPredictiveOutputTest.root
```
<img width="350" alt="PriorPredPlot" src="https://github.com/user-attachments/assets/5308f707-04aa-4c57-bf59-5d000d535463">

Here, you can see that the prior distribution has much larger errors. This gives you some idea how well we constrain parameters during the fitting process.

## How to Develop a Model of Systematic Uncertainties
The systematic uncertainties in the MCMC describe how we are modeling the underlying physics. For example, you might have a cross-section model that describes how neutrinos interact with nuclei.
For such a model, you may have several parameters that help implement the model in code, such as the absolute cross-section normalization, or the relative normalization for neutrino and anti-neutrino interactions.
In this step, we will modify the analysis setup for one of our systematic parameters and repeat the tutorial steps to see the impact.

First let's get a better understanding `TutorialConfigs/CovObjs/SystematicModel.yaml`. This is the main config that controls what systematic uncertainties will be used in the analysis. For example:
```yaml
- Systematic:
    Names:
      FancyName: Norm_Param_0
    Error: 0.11
    FlatPrior: false
    FixParam: false
    Mode: [ 0 ]
    ParameterBounds: [0, 999.]
    ParameterGroup: Xsec
    TargetNuclei: [12, 16]
    KinematicCuts:
      - TrueQ2: [0.25, 0.5]
    ParameterValues:
      Generated: 1.
      PreFitValue: 1.
    SampleNames: ["Tutorial Beam"]
    StepScale:
      MCMC: 0.2
    Type: Norm
```
If you want to read more about the specifics of implementating systematics in the configs, please see [here](https://github.com/mach3-software/MaCh3/wiki/02.-Implementation-of-Systematic)

As a first step let's modify `Error: 0.11` to `Error: 2.0`. This significantly changes the prior uncertainty on the `Norm_Param_0` parameter. Such a change should be very noticeable in the MCMC posterior.

Let's also modify the output file name so we don't overwrite our previous outputs. This is governed by the manager class (read more [here](https://github.com/mach3-software/MaCh3/wiki/01.-Manager-and-config-handling)).
You can modify this in the configs by for example changing `OutputFile: "Test.root"` in `TutorialConfigs/FitterConfig.yaml` to `OutputFile: "Test_Modified.root"` (or you could use a parameter override by adding `General:OutputFile:Test_Modified.root` as a command line argument).

With the modifications, let's run MCMC again:
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml
```
Congratulations, you have sucessfully modified the MCMC! 🎉

### How to Compare Chains
Now that you have two chains you can try comparing them using the following:
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root Default_Chain Test_Modified.root Modified_Chain
```
This will produce a pdf with plots showing overlayed posteriors from both chains. Most should be similarly except modified parameter.

### Holding Parameters Fixed
Sometimes you may want to fix a parameter to some nominal value to stop it from moving during the MCMC. For example, if a parameter is causing some problem to the fitter, or you want to compare what happens with and without some new parameter, this option would be very useful to you.
To fix a parameter, just pass the parameter's name to the `TutorialConfigs/CovObjs/FitterConfig.yaml`:
```yaml
General:
  Systematics:
    XsecFix: [ "Norm_Param_0" ]
```
If you were to run a new MCMC with this configuration, you should see that `Norm_Param_0` does not move from its nominal position during the chain.

## How to Develop New Samples
Analysis samples are where we compare data and simulation to extract our physics results. For example, you might have an analysis sample that is enriched in neutral-current pi0 events to precisely study NC-pi0 cross sections.
In this section, we will modify an existing analysis sample to explore how the inner workings of MaCh3 operate, and then see how to add a new one.

### Modifying Analysis Samples
To begin, let's take a look at `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml`. Each sample has a set of cuts that select events into the sample and reject others. Right now, let's focus on the cut on `TrueNeutrinoEnergy`:
```yaml
SelectionCuts:
  - KinematicStr: "TrueNeutrinoEnergy"
    Bounds: [ 0., 4 ]
```
We can introduce additional cuts to the sample by expanding the `SelectionCuts` in the config. For example, we could add a cut on Q2 like this:
```yaml
SelectionCuts:
  - KinematicStr: "TrueNeutrinoEnergy"
    Bounds: [ 0., 4 ]
  - KinematicStr: "TrueQ2"
    Bounds: [ 0.6, 1 ]
```

You can also easily change what variable the sample is binned in to get slightly different fit results. For example, we could change `RecoNeutrinoEnergy` to `TrueNeutrinoEnergy` in `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml`:
```yaml
Binning:
  XVarStr : "TrueNeutrinoEnergy"
  XVarBins: [0.,  0.5,  1.,  1.25, 1.5, 1.75, 2., 2.25, 2.5, 2.75, 3., 3.25, 3.5, 3.75, 4., 5., 6., 10.]
```
You can run the MCMC again using this new configuration (after changing the output file name again!), and then compare all 3 chains using:
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root Default_Chain Test_Modified.root Modified_Chain Test_Modified_Sample.root ModifiedSameple_Chain
```

### Adding a New Sample
Up to this point we have only modified an existing sample, but how would we add a new one? We can start by first making a copy of the sample config `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml` and calling it `TutorialConfigs/Samples/SampleHandler_User.yaml`.
For the moment feel free to change the sample name, binning, whatever you want. Go wild! Just be sure to keep `TutorialConfigs/CovObjs` the same, or things will break.

Next, go to the main config (`TutorialConfigs/Samples/FitterConfig.yaml`) and add in your newly-implemented sample by expanding the `TutorialSamples` section:
```yaml
General:
  TutorialSamples: ["TutorialConfigs/Samples/SampleHandler_Tutorial.yaml", "TutorialConfigs/Samples/SampleHandler_User.yaml"]
```

<details>
<summary><strong>(Detailed) Samples using the same config</strong></summary>
The above explained adding new samples via separate configs (resulting in the creation of a new sample object when the code runs). It is possible for you to instead add your new sample directly to the existing sample config.
You can find an example of doing so here: `TutorialConfigs/Samples/SampleHandler_Tutorial_ND.yaml`.

The general scheme for doing this is to specify a list of `Samples` in the config, and then flesh out the sample details within different `Sample` blocks in the yaml like this:
```yaml
General Settings like MaCh3 mode, NuOscillator

Samples: ["Sample_1", "Sample_2"]

Sample_1:
  Settings for Sample 1 like binning, osc channels cutc etc

Sample_2:
  Settings for Sample 2 like binning, osc channels cutc etc
```
This approach may give a performance boost, but is especially nice if you are sharing common MC and detector systematics, since then everything can be to store within a single C++ object.
</details>

### Changing Oscillation Engine
MaCh3 has access to many oscillation engines via the `NuOscillator` framework. You can check the features of this using:
```bash
bin/mach3-config --features
MULTITHREAD MR2T2  PSO  Minuit2 Prob3ppLinear NuFast
```
Thus, you can easily access information about MaCh3 features, most importantly the fitter engines (`MR2T2`, `PSO`, `Minuit2`) and neutrino oscillation engines (`Prob2ppLinear`, `NuFast`).

By default, the oscillation engine we use is `NuFast`. However, you can change to another engine (`Prob3++` here) by modifying the sample config `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml`:
```yaml
NuOsc:
  NuOscConfigFile: "TutorialConfigs/NuOscillator/Prob3ppLinear.yaml"
```
This changes the oscillation engine by specifying a new oscillation configuration file. In most cases this is enough, however you have to be aware that different oscillation engines may require a different number of parameters.
In this example, NuFast requires one additional parameter compared with Prob3ppLinear (`Ye` in this case). You will have to remove the `Ye` parameter from `TutorialConfigs/CovObjs/OscillationModel.yaml` complete the switch to `Prob3++`.

Another useful setting is whether you want binned or unbinned oscillations. If you want to change this, simply go to `TutorialConfigs/NuOscillator/Prob3ppLinear.yaml` and modify the following to `Unbinned`.
```yaml
General:
  CalculationType: "Binned"
```

### Atmospheric Sample
Up to this point, we have been working exclusively with Beam neutrino samples and oscillations. In terms of MaCh3, you can switch to atmospheric neutrino oscillations by only changing a few things:

* `rw_truecz` has to be filled. This represents "Cosine Zenith angle", which Atmospheric calculations depends on for their propagation baseline.
* Oscillation calculations must be switched to an engine which supports Atmospheric oscillations. For example, CUDAProb3 supports atmospheric oscillations, while CUDAProb3Linear supports beam only.
* The oscillation systematic yaml must be modified: instead of `density`/`baseline`, (and `Ye`) it requires a neutrino production height.

In this tutorial, you can try poking around `TutorialConfigs/Samples/SampleHandler_Tutorial_ATM.yaml` to see more details.

### Plotting Kinematic Distribution
You can plot kinematic distributions of your new sample using:
```bash
./bin/KinemDistributionTutorial TutorialConfigs/FitterConfig.yaml
```
Notice that we are using the same config as the MCMC tutorial here. At the bottom of the config, you can specify any individual variables you would like to plot with this executable, along with any selection cuts for each plot in the `KinematicDistributionPlots` section.
An example of what you might expect for output can be seen here:

<img width="350" alt="Kinematic example" src="https://github.com/user-attachments/assets/534bcb17-f26c-4fc2-a77a-5d253b0ed241">

### More Advanced Development
Not everything can be done by modifying config in sample implementation. The actual implementation of samples is in `Samples/SampleHandlerTutorial`, which inherits from `SampleHandlerFDBase`.
The latter class deals with actual event reweighting and general heavy lifting. `SampleHandlerTutorial` deals with more specific information, like MC variable loading. This is because each experiment has slightly different MC format and different information available, and so it must be implemented in a custom manner.

## MCMC Diagnostic
A crucial part of MCMC is diagnosing whether a chain has converged or not. You can produce chain diagnostics by running:
```bash
./bin/DiagMCMC Test.root bin/TutorialDiagConfig.yaml
```
This will produce a plethora of diagnostic information. One of most important of these are the autocorrelations for each of the parameters. Autocorrelation indicates how correlated MCMC steps are when they are n-steps apart in the chain.
Generally speaking, we want the autocorrelation to drop to 0 fast and stay around there (like in the plot below). You can read about other diagnostics [here](https://github.com/mach3-software/MaCh3/wiki/11.-Step-size-tuning), but it is sufficient for now to focus on autocorrelation.

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/e146698c-713c-4daf-90df-adeb3051539b">

The best way to reduce autocorrelations is step size tunning. In MaCh3, there are two types of step size parameters:

**Global** step sizes apply to every parameter in the MCMC identically as a proportional scaling factor. In `TutorialConfigs/FitterConfig.yaml`, an example of this is `XsecStepScale`:
```yaml
General:
  Systematics:
    XsecStepScale: 0.05
```
which applies to all `Xsec`-type parameters universally, and can be used to quickly increase or decrease the step sizes of all parameters simultaneously.

**Individual** step sizes affect each parameter independently from one another. The tuning of these is highly parameter-dependent (for example, different parameters have different prior errors, data constraints, and boundary sensitivity).
In general, these can be found in `TutorialConfigs/CovObjs/SystematicModel.yaml`, as `StepScale` parameters:
```yaml
- Systematic:
    Names:
      FancyName: Norm_Param_0
    StepScale:
      MCMC: 0.2
```
Changing this will only affect `Norm_Param_0` during the MCMC (to first order).

For the tutorial here, try changing both scales running the MCMC again, and then checking the autocorrelations. Understanding how autocorrelations change while playing with step-size is an extremely useful skill that is fundamental to understanding MCMC tuning.

You can make plots from the diagnostic output using:
```bash
./bin/PlotMCMCDiag Test_MCMC_Diag.root "mcmc_diagnostics"
```
If you add a second/third arguemnt, the macro can compare several files:
```bash
./bin/PlotMCMCDiag Test_MCMC_Diag.root "first file label" SecondFile_MCMC_Diag.root "second file label"
```

### Running Multiple Chains
At this point, you should be aware that to have a smooth posterior distribution, you may need a lot of steps, which can be time-consuming. A great property of MCMC is that once a chain reaches the stationary distribution (or, in other words, converges), it continues to sample from the same distribution.
We can leverage this by running several chains in parallel. Computing clusters give us the ability to run thousands of MCMC chains in parallel, allowing us to accumulate steps very fast.

The only caveat of this method is that the parallel chains have to converge to the *same* stationary distribution for it to work (it is possible for there to be one stationary distribution, but have chains get stuck in local minima or not converge due to poor step-size tunning).
To validate if chains converged we can use the $\widehat{R}$ metric, which compares the MCMC mixing of different parallel chains to determine if they have converged to the same stationary distribution.
For details on what this metric is more specifically, see [here](https://mc-stan.org/docs/2_18/reference-manual/notation-for-samples-chains-and-draws.html).

The following code will calculate the $\widehat{R}$ metric across four different MCMC chains (which you should run yourself to test! Be sure to use identical configurations for all four chains).
Here, 1000 indicates the number of steps we want to sample from the chains, while the other arguments indicate different MCMC output files. You can pass as many chains as you want.
```bash
./bin/RHat 1000 MCMC_Test_1.root MCMC_Test_2.root MCMC_Test_3.root MCMC_Test_4.root
```

$\widehat{R}$ is essentially the ratio of the variance between different MCMC chains to the variance within a single chain. As such, we expect well-mixed chains to peak at 1, indicating that the two variances are the same.
In the following plot, a single entry in the histogram refers to a single parameter from the MCMC. If your distribution has a tail reaching beyond 1.1 (according to Gellman) then this likely indicates some chains haven't converged to the same distribution.
If this is the case, you MUST investigate, as it indicates something wrong with your MCMC (note: in this tutorial, the most-likely reason you may end up with $\widehat{R}>1.1$ will be a lack of sufficient MCMC steps; run longer chains and see if it helps).

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/eada5efd-ca8f-42a4-a710-4dced5c3e3da">

Once you validated that your parallel chains have converged, you can merge them into one big chain using:
```bash
./bin/CombineMaCh3Chains -o MergedChain.root MCMC_Test_1.root MCMC_Test_2.root MCMC_Test_3.root MCMC_Test_4.root
```
This works very similarly to `hadd`, although this method has some advantages. The main one is that it checks if chains were run with the same settings (if chains have different settings, they are not the same MCMC, and therefore cannot be merged).
If for example one chain was run with different systematic parameters, then this will be caught here.

## Useful Settings
There are plenty of useful settings in the configurations you can change.

**Fitting Algorithm**: Most likely you run MCMC, but what if you want to use a different algorithm like Minuit2? You can do this by changing the following setting in `TutorialConfigs/FitterConfig.yaml` "Minuit2":
```yaml
General:
  FittingAlgorithm: "MCMC"
```

**LLH Type**:
By default we use Barlow-Beeston LLH, however several different LLH types are available in MaCh3. By changing `TutorialConfigs/FitterConfig.yaml`, you can alternatively use Poisson or even IceCube LLH:
```yaml
LikelihoodOptions:
  TestStatistic: "Barlow-Beeston"
```
Read more [here](https://mach3-software.github.io/MaCh3/Structs_8h.html#a960da89e33ac45a56f7bbfac3068dc67).

## Other Useful Plots

There are a number of apps included to make plots from the results of your fits, llh scans etc. You can find more details on them and how they work in the main MaCh3 wiki [here](https://github.com/mach3-software/MaCh3/wiki). There you will also find some instructions on how you can write your own plotting scripts.

The plotting library is configured using yaml files. You can see some examples of such config files in the plotting directory, and a detailed explanation of them is given in [the wiki](https://github.com/mach3-software/MaCh3/wiki).

Some examples on how to make some "standard" plots are given below.

### LLH Scans
An LLH Scan is a procedure where one changes value of single parameter, reweights the MC, and calculates likelihood. This is useful for checking the impact of new parameters, seeing any problematic impacts on the likelihood, and even step size tuning.

For this tutorial, we'll be running in an Asimov setting. In such a case, the LLH should be 0 at prior values (since then, the MC and "data" match perfectly). The rate at which LLH increases as a parameter moves away from the prior indicates how sensitive the MC is to a given parameter.

You can run an LLH scan in a very similar way to how you would an MCMC:
```bash
./bin/LLHScanTutorial TutorialConfigs/FitterConfig.yaml
```

You can plot the results of the LLH scan using the aptly named `PlotLLH` app like so:
```bash
./bin/PlotLLH LLH_Test.root
```
where `LLH_Test.root` is the output of the `LLHScanTutorial` app described above.

It is possible to compare several LLH scan files simply by adding more files as command line arguments:
```bash
./bin/PlotLLH LLH_Test.root LLH_Test_2.root
```

### Sigma Variations
Sigma Variations are conceptually similar to the LLH scan, where we vary individual parameters and reweight the MC correspondingly, however here instead of looking at how the LLH changes, the focus is actually on how the sample spectra themselves respond to the parameter movements.

You can run a sigma variation using the same format as the LLH scan:
```bash
./bin/SigmaVarTutorial TutorialConfigs/FitterConfig.yaml
```
Once it has finished, you can make plots using
```bash
./bin/PlotSigmaVariation SigmaVar_Test.root bin/TutorialDiagConfig.yaml
```
where `SigmaVar_Test.root` is the output file from `SigmaVarTutorial`.

<details>
<summary><strong>(Detailed) Plotting with Python</strong></summary>

If you have installed the python interface for MaCh3 as described
[here](https://github.com/mach3-software/MaCh3?tab=readme-ov-file#python)
then you can also use the provided python plotting module. The details on how
to write custom python scripts using this plotting module are detailed in
[the wiki](https://github.com/mach3-software/MaCh3/wiki/15.-Plotting#custom-plotting-scripts).
Here we will walk you through some example scripts.

For the examples here, we will use matplotlib and numpy. These can be installed using the provided [requirements.txt](requirements.txt) by doing

```bash
pip install -r requirements.txt
```
but note that these are just what is used for an example for the purpose of this tutorial. When making your own plots you are totally free to use whatever plotting libraries you like!

You can use the example script [MCMCPlotting-tutorial.py](plotting/MCMC-plotting-tutorial.py) to plot the raw MCMC chain values that were obtained in the [Other Useful Plots](#other-useful-plots) section above by doing
```bash
python plotting/MCMC-plotting-tutorial.py Test.root
```

This will give you some plots that look something like

<img width="350" alt="MCMC example" src="https://github.com/user-attachments/assets/bdb1792f-3d52-4ea3-8eb8-0e6cf7b9119a">

After you have run this chain through the MCMC processor as described in the [Other Useful Plots](#other-useful-plots) section, you can pass the processed file to [1DPosterior-tutorial.py](plotting/1DPosterior-tutorial.py) like

```bash
python plotting/1DPosterior-tutorial.py Test_Process.root
```

which will plot the projected one dimensional posteriors which should look something like

<img width="350" alt="1dPosterior example" src="https://github.com/user-attachments/assets/4ddf3abb-9794-4f21-ae9b-862718c2ff57">


Similarly you can use [LLHScan-plotting-tutorial.py](plotting/LLHScan-plotting-tutorial.py) to plot the LLH scans you made in the [LLH Scans](#llh-scans) section like

```bash
python plotting/LLHScan-plotting-tutorial.py LLH_Test.root
```

which will give you some plots that look something like

<img width="350" alt="LLH scan example" src="https://github.com/user-attachments/assets/f16ad571-68da-42e3-ae6b-e984d03a58c3">

</details>

## Issues
If you encountered any issues or find something confusing please contact us:
* [Kamil Skwarczynski](mailto:Kamil.Skwarczynski@rhul.ac.uk)<br>
  Royal Holloway, University of London, UK


