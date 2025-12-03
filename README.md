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
    1. [Processing MCMC Outputs](#processing-mcmc-outputs)
    2. [Plotting MCMC Posteriors](#plotting-mcmc-posteriors)
    3. [Plotting Correlation Matrix](#plotting-correlation-matrix)
3. [Posterior Predictive Analysis](#posterior-predictive-analysis)
    1. [Plotting Posterior Predictive Distributions](#plotting-posterior-predictive-distributions)
    2. [Prior Predictive Distributions](#prior-predictive-distributions)
4. [How to Develop Model of Systematic Uncertainties](#how-to-develop-model-of-systematic-uncertainties)
    1. [How to Plot Comparisons?](#how-to-plot-comparisons)
    2. [More Advanced Systematic Development](#more-advanced-systematic-development)
5. [How to Develop New Samples](#how-to-develop-new-samples)
    1. [Changing Oscillation Engine](#changing-oscillation-engine)
    2. [Atmospheric Sample](#atmospheric-sample)
    3. [Plotting Kinematic Distribution](#plotting-kinematic-distribution)
    4. [More Advanced Development](#more-advanced-development)
6. [MCMC Diagnostic](#mcmc-diagnostic)
    1. [Running Multiple Chains](#running-multiple-chains)
7. [Useful Settings](#useful-settings)
8. [How to Plot?](#how-to-plot)
    1. [How to run LLH scan](#how-to-run-llh-scan)
    2. [How to run Sigma Variation](#how-to-run-sigma-variation)

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

**WARNING** Your posterior may look very shaky and slightly different to the one in example. This is because you have run the chain with low number of steps, meaning you don't have enough statistic to build out the posterior distribution. It is good homework to increase the number of steps and see how much smoother the posterior becomes, but at the cost of having to wait more. You can easily test this by modifying `TutorialConfigs/FitterConfig.yaml` to change the number of MCMC steps:
```yaml
General:
  MCMC:
    NSteps: 10000
```
and then re-running `MCMCTutorial`.

**Warning**: If you modified files in the main `MaCh3Tutorial` folder instead of `build`, you will have to call `make install` for the changes to propagate! Generally speaking, it is good practice to work from the `build` directory and make your config changes there so that local changes do not have to be tracked by git.

Alternatively, you can dynamically override your configurations at the command line like this:
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml General:MCMC:NSteps:100000
```
You can add as many configuration overrides here as you like, as long as the format is `[executable] [config] ([option1] [option2] ...)`

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
If you have run `ProcessMCMC` with option "PlotCorr" you will have a correlation matrix in the outputs. This is a handy tool for viewing how correlated different parameters are. However, mature analyses with hundreds of parameters may run into the problem of having too large of plots to be useful. To combat this, you can plot a subset of parameters using `MatrixPlotter`:
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
Since MCMC produces a full posterior distribution rather than a single best-fit value, one needs to use a Posterior Predictive Analysis (PPA) to produce spectra after the fit. The idea is to draw parameter sets from the MCMC chains and generate a toy spectrum for each set. The final distribution of spectra is then obtained from all these toy spectra, reflecting the full uncertainty encoded in the posterior.

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
The above steps focused on Posterior Predictive distributions, whereby we see the uncertainty in the spectrum after the parameters are constrained by the MCMC. You can use a similar setup to run Prior Predictve Analysis, where we check the spectrum uncertainty coming from the parameter priors (i.e., with no constraint from the fit). The main difference here is that we throw from the parameter prior covariance matrix instead of the posterior chain.

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

## How to Develop Model of Systematic Uncertainties
In the next step you gonna modify analysis setup and repeat steps.
First let's better understand `TutorialConfigs/CovObjs/SystematicModel.yaml`. This config controls what systematic uncertainties will be used in the analysis for example like this:
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
If you want to read more about implementation please go [here](https://github.com/mach3-software/MaCh3/wiki/02.-Implementation-of-Systematic)

As first step let's modify `Error: 0.11` to `Error: 2.0`, this should significantly modify error which should be noticeable in MCMC.

Lastly we need to modify name of output file. This is governed by manager class (read more [here](https://github.com/mach3-software/MaCh3/wiki/01.-Manager-and-config-handling)) modify `OutputFile: "Test.root"` in `TutorialConfigs/FitterConfig.yaml` to for example
`OutputFile: "Test_Modified.root"`.

Now let's run MCMC again
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml
```
Congratulations! 🎉
Next step is to compare both chains.

### How to Plot Comparisons?
Now that you have two chains you can try comparing them using following.
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root Default_Chain Test_Modified.root Modified_Chain
```
This will produce pdf file with overlayed posteriors. Most should be similarly except modified parameter.

### More Advanced Systematic Development
Sometimes you may want to fix parameter, for example if it causing problem to the fitter or you want to run fit with and without parameters to compare results. To fix parameter just pass name in the `TutorialConfigs/CovObjs/FitterConfig.yaml`
```yaml
General:
  Systematics:
    XsecFix: [ "Norm_Param_0" ]
```

## How to Develop New Samples
First we gonna investigate how to modify sample, let's take a look at `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml`. Each sample has set of cuts right now we only introduce cut on `TrueNeutrinoEnergy`.
```yaml
SelectionCuts:
  - KinematicStr: "TrueNeutrinoEnergy"
    Bounds: [ 0., 4 ]
```
We can introduce additional cut for example on Q2 by expanding config like this:
```yaml
SelectionCuts:
  - KinematicStr: "TrueNeutrinoEnergy"
    Bounds: [ 0., 4 ]
  - KinematicStr: "TrueQ2"
    Bounds: [ 0.6, 1 ]
```

You can also easily switch variable in which you bin sample.
```yaml
Binning:
  XVarStr : "TrueNeutrinoEnergy"
  XVarBins: [0.,  0.5,  1.,  1.25, 1.5, 1.75, 2., 2.25, 2.5, 2.75, 3., 3.25, 3.5, 3.75, 4., 5., 6., 10.]
```
You can try again to run MCMC and compare all 3 chains
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root Default_Chain Test_Modified.root Modified_Chain Test_Modified_Sample.root ModifiedSameple_Chain
```
Up to this point we only modified sample but how to add new one? First make copy of sample config `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml` and call it `TutorialConfigs/Samples/SampleHandler_User.yaml`. For the moment feel free to change name, binning etc but keep TutorialConfigs/CovObjs the same. Go wild! Next go to `TutorialConfigs/Samples/FitterConfig.yaml`
```yaml
General:
  TutorialSamples: ["TutorialConfigs/Samples/SampleHandler_Tutorial.yaml"]
```
To add you newly implemented sample you will have to expand config to for example:
```yaml
General:
  TutorialSamples: ["TutorialConfigs/Samples/SampleHandler_Tutorial.yaml", "TutorialConfigs/Samples/SampleHandler_User.yaml"]
```

<details>
<summary><strong>(Detailed) Samples using same config</strong></summary>
Above explained adding new samples via separate configs (resulting in creation of new object). However it may be simpler to add new sample using same config. You can find example of such config here: `TutorialConfigs/Samples/SampleHandler_Tutorial_ND.yaml`.

With general scheme being:
```yaml
General Settings like MaCh3 mode, NuOscillator

Samples: ["Sample_1", "Sample_2"]

Sample_1:
  Settings for Sample 1 like binning, osc channels cutc etc

Sample_2:
  Settings for Sample 2 like binning, osc channels cutc etc
```
Such approach may be more beneficial performance wise but especially if you are sharing common MC and detector it can be more appealing to store it within single C++ object.
</details>

### Changing Oscillation Engine
MaCh3 has access to many oscillation engines via NuOscillator framework. First you can check features using following command
```bash
bin/mach3-config --features
MULTITHREAD MR2T2  PSO  Minuit2 Prob3ppLinear NuFast
```
This way you can easily access information about MaCh3 features, fitter engines and most importantly oscillation engines.

By default we use **NuFast**, however to change to for example **Prob3++** one have to modify sample config `TutorialConfigs/Samples/SampleHandler_Tutorial.yaml`:
```yaml
NuOsc:
  NuOscConfigFile: "TutorialConfigs/NuOscillator/Prob3ppLinear.yaml"
```
In most cases this is enough. However you have to be aware that some engines require different number of parameters. In this example NuFast requires one additional parameter compared with Prob3ppLinear which is **Ye**. You will have to remove **Ye** from `TutorialConfigs/CovObjs/OscillationModel.yaml`

Another useful setting is whether you want binned or unbinned oscillations, if you want to do this simple got to `TutorialConfigs/NuOscillator/Prob3ppLinear.yaml` and modify following path to **Unbinned**.
```yaml
General:
  CalculationType: "Binned"
```

### Atmospheric Sample
Up to this point Beam oscitation calculations have been discussed. In terms of MaCh3 to switch to atmospheric only a few things have to be changed.

* **rw_truecz** have to be filled. This is Cosine Zenith angle which Atmospheric calculations depends on.
* Switch oscillation calculations to engine which supports Atmospheric for example CUDAProb3 (not to be confused with CUDAProb3Linear which supports beam only).
* Modify Oscillation systematic yaml, instead of density/baseline (and **Ye**) it requires production height.

In tutorial you can try uncommenting `TutorialConfigs/Samples/SampleHandler_Tutorial_ATM.yaml`.

### Plotting Kinematic Distribution
You can plot kinematic distribution of your sample using
```bash
./bin/KinemDistributionTutorial TutorialConfigs/FitterConfig.yaml
```
Notice same config being used. In other words you can add or disable sample, modify cuts in same way as was discussed. At the bottom of the config, you can specify any individual variables you would like to plot with this executable, along with any selection cuts for each plot.
Example of plot you can see here:

<img width="350" alt="Kinematic example" src="https://github.com/user-attachments/assets/534bcb17-f26c-4fc2-a77a-5d253b0ed241">

### More Advanced Development
Not everything can be done by modifying config in sample implementation. Actual implementation is in`Samples/SampleHandlerTutorial` this class inherits from `SampleHandlerFDBase`. The latter class deals with actual reweighting and all heavy lifting. while SampleHandlerTutorial deals with MC loading etc. This is because each experiment has slightly different MC format and different information available.

## MCMC Diagnostic
Crucial part of MCMC is diagnostic whether chain converged or not. You can produce diagnostic by running.

```bash
./bin/DiagMCMC Test.root bin/TutorialDiagConfig.yaml
```

This will produce plethora of diagnostic however one most often checked are autocreation's which indicate how correlated are MCMC steps which are n-steps apart. We want autocreation's to drop fast.
You can read about other diagnostic here on [here](https://github.com/mach3-software/MaCh3/wiki/11.-Step-size-tuning)

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/e146698c-713c-4daf-90df-adeb3051539b">

Best way to reduce auto-corelations is by step size tunning. There are two step-scale available.

Global which affect identically every parameter and it is proportional to all parameters can be found in `TutorialConfigs/FitterConfig.yaml`:
```yaml
General:
  Systematics:
    XsecStepScale: 0.05
```
or individual step scale affecting single parameters which highly depend on parameters boundary sensitivity etc can be found in `TutorialConfigs/CovObjs/SystematicModel.yaml`.
```yaml
- Systematic:
    Names:
      FancyName: Norm_Param_0
    StepScale:
      MCMC: 0.2
```
We recommend chasing both scales running MCMC again and later producing auto-corelations. Understanding how auto-corelations change while playing with step-size is very useful skill.

You can make plots using:

```bash
PlotMCMCDiag Test_MCMC_Diag.root "mcmc_diagnostics"
```
If you add second/third arguemnt it will compare several files:

```bash
PlotMCMCDiag Test_MCMC_Diag.root "first file label" SecondFile_MCMC_Diag.root "second file label"
```

### Running Multiple Chains
At this point, you should be aware that to have a smooth posterior distribution,
you may need a lot of steps, which can be time-consuming. A great property of MCMC
is that once a chain reaches the stationary distribution (or, in other words, converges),
it samples the same distribution. This means we can run several chains in parallel.
Computing clusters give us the ability to run thousands of MCMC chains in parallel,
allowing us to accumulate steps very fast.

The only caveat of this method is that chains have to converge to the same stationary distribution (there can only be one stationary distribution but chains can stuck in local minima or not converge due to wrong step-size tunning). To validate if chains converged we can use **RHat**.

In the following example 1000 indicate number of toys we want to sample while other arguments indicate different chains. You can pass as many chains as you want
```bash
./bin/RHat 1000 MCMC_Test_1.root MCMC_Test_2.root MCMC_Test_3.root MCMC_Test_4.root
```

**RHat** is estimator of variance between chains, in other words it should be peaking at 1. Single entry in histogram refers to single parameters. If your distribution has a tail reaching beyond 1.1 (according to Gellman) then this maybe indicate some chains haven't converged to the same distribution. Which MUST be investigated in analysis (in this tutorial main culprit will be number of steps)

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/eada5efd-ca8f-42a4-a710-4dced5c3e3da">

Once you validated that chains converged you may need to merge them
```bash
./bin/CombineMaCh3Chains -o MergedChain.root MCMC_Test_1.root MCMC_Test_2.root MCMC_Test_3.root MCMC_Test_4.root
```
This works very similarly to **hadd** although has some advantages, main one being it checks if chains were run with the same settings. If for example one chains was run with different systematic parameters then this should be caught and raised.

## Useful Settings
There are plenty of useful settings in
**Fitting Algorithm**: Most likely you run MCMC but what if you want to use algorithm like Minuit2?
```yaml
General:
  FittingAlgorithm: "MCMC"
```
In `TutorialConfigs/FitterConfig.yaml` you should switch following setting to "Minuit2"

**LLH Type**:
By default we use Barlow-Beeston LLH, however several are implemented. For example by changing config you can use Poisson or maybe IceCube.
```yaml
LikelihoodOptions:
  TestStatistic: "Barlow-Beeston"
```
Read more [here](https://mach3-software.github.io/MaCh3/Structs_8h.html#a960da89e33ac45a56f7bbfac3068dc67)

## How to Plot?

There are a number of apps included to make plots from the results of your fits, llh scans etc. You can find more details on them and how they work in the main MaCh3 wiki [here](https://github.com/mach3-software/MaCh3/wiki). There you will also find some instructions on how you can write your own plotting scripts.

The plotting library is configured using yaml files. You can see some examples of such config files in the plotting directory, and a detailed explanation of them is given in [the wiki](https://github.com/mach3-software/MaCh3/wiki).

Some examples on how to make some "standard" plots are given below.

### How to run LLH scan
LLH scan is a procedure where one changes value of single parameter, reweight MC and calculates likelihood.
If running with Asimov setting at prior value LLH should be 0. Rate at which LLH is increasing with increase of value indiceate how sensitive your MC is to give parameter.

You can run LLH scan in very similar way as MCMC
```bash
./bin/LLHScanTutorial TutorialConfigs/FitterConfig.yaml
```
You can plot the results of an LLH scan using the aptly named PlotLLH app like so

```bash
PlotLLH LLH_Test.root
```
where LLH_Test.root is the result of running the LLH scan as described [here](#how-to-run-llh-scan).

It is possible to compare several files simply by:

```bash
PlotLLH LLH_Test.root LLH_Test_2.root
```

### How to run Sigma Variation
Sigma Var is conceptually similar to LLH scan however here instead of looking how LLH changes one investigate actual sample spectra.

```bash
./bin/SigmaVarTutorial TutorialConfigs/FitterConfig.yaml
```

Once it finished you can make plots using
```bash
./bin/PlotSigmaVariation SigmaVar_Test.root bin/TutorialDiagConfig.yaml
```

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

You can use the example script [MCMCPlotting-tutorial.py](plotting/MCMC-plotting-tutorial.py) to plot the raw MCMC chain values that were obtained in the [how to run MCMC](#how-to-run-mcmc) section above by doing

```bash
python plotting/MCMC-plotting-tutorial.py Test.root
```

This will give you some plots that look something like

<img width="350" alt="MCMC example" src="https://github.com/user-attachments/assets/bdb1792f-3d52-4ea3-8eb8-0e6cf7b9119a">

After you have run this chain through the MCMC processor as described in the [How To Plot?](#how-to-plot) section, you can pass the processed file to [1DPosterior-tutorial.py](plotting/1DPosterior-tutorial.py) like

```bash
python plotting/1DPosterior-tutorial.py Test_Process.root
```

which will plot the projected one dimensional posteriors which should look something like

<img width="350" alt="1dPosterior example" src="https://github.com/user-attachments/assets/4ddf3abb-9794-4f21-ae9b-862718c2ff57">


Similarly you can use [LLHScan-plotting-tutorial.py](plotting/LLHScan-plotting-tutorial.py) to plot the LLH scans you made in the [How to run LLH scan](#how-to-run-llh-scan) section like

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


