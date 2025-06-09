# Welcome to MaCh3 Tutorial
After this tutorial you should know how to run MCMC, implement systematic uncertainties new samples and how to plot standard Bayesian diagnostic.

Current setup include single sample with several cross-section and oscillation parameters.

MaCh3 is predominantly C++ software although some functionality are available through python as well. To access them please use tale of contents.

[![Code - Documented](https://img.shields.io/badge/Code-Documented-2ea44f)](https://github.com/mach3-software/MaCh3/wiki)
[![Container Image](https://img.shields.io/badge/Container-Image-brightgreen)](https://github.com/mach3-software/MaCh3Tutorial/pkgs/container/mach3tutorial)
[![Code - Doxygen](https://img.shields.io/badge/Code-Doxygen-2ea44f)](https://mach3-software.github.io/MaCh3/index.html)

## Table of contents
1. [How to Start?](#how-to-start)
2. [How to Run MCMC](#how-to-run-mcmc)
    1. [MCMC Chain](#mcmc-chain)
3. [How to Develop Model of Systematic Uncertainties](#how-to-develop-model-of-systematic-uncertainties)
    1. [How to Plot Comparisons?](#how-to-plot-comparisons)
    2. [More Advanced Systematic Development](#more-advanced-systematic-development)
4. [How to Develop New Samples](#how-to-develop-new-samples)
    1. [Changing Oscillation Engine](#changing-oscillation-engine)
    2. [Atmospheric Sample](#atmospheric-sample)
    3. [Plotting Kinematic Distribution](#plotting-kinematic-distribution)
    4. [More Advanced Development](#more-advanced-development)
5. [MCMC Diagnostic](#mcmc-diagnostic)
    1. [Running Multiple Chains](#running-multiple-chains)
6. [Useful Settings](#useful-settings)
7. [How to Plot?](#how-to-plot)
    1. [How to run LLH scan](#how-to-run-llh-scan)
    2. [Plotting with Python](#plotting-with-python)

## How to Start?
To compile simply
```bash
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
To reed more how to use containers check our wiki [here](https://github.com/mach3-software/MaCh3/wiki/12.-Containers)

## How to run MCMC
To run MCMC simply
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml
```
Congratulations! 🎉
You have just completed finished you first MCMC chain. You can view `Test.root` for example in TBrowser like in plot below.

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/6d4c97e2-f36b-456e-8e7b-1751f142d2ac">

Single entry in tree represent single MCMC step. Other then debug purposes it is highly recommended to use MaCh3 processing tools.

### MCMC Chain
Being able to visualise and analyse output of MCMC is standard procedure after chain has finished, you can produce simple plots with
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root
```
where **Test.root** is the output of running MCMCTutorial as described [here](#how-to-run-mcmc).
You can find results in **Test_drawCorr.pdf**
One of plots you will encounter is:
<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/1073a76e-5d82-4321-8952-e098d1b0717f">

It is marginalised posterior of a single parameter. This is main output of MCMC.

**WARNING** Your posterior may look very shaky and slightly different to one in example. This is because you run chain with low number of steps. Meaning you don't have enough statistic to build posterior distribution. You can easily modify in `TutorialConfigs/FitterConfig.yaml`
```yaml
General:
  MCMC:
    NSteps: 10000
```
It is good homework to increase number of steps and see how much more smooth posterior becomes, but at the cost of having to wait more.

**Warning**: If you modified files in main folder not build you will have to call make install!

Alternatively you can achieve same way of modifying configs by using command line
```bash
./bin/MCMCTutorial TutorialConfigs/FitterConfig.yaml General:MCMC:NSteps:100000
```

**ProcessMCMC** has much more plotting options, we recommend to see [here](https://github.com/mach3-software/MaCh3/wiki/09.-Bayesian-Analysis,-Plotting-and-MCMC-Processor) to get better idea what each plot mean.
We especially recommend comparing 2D posteriors with correlation matrix and playing with triangle plots.

You can then take the output of running ProcessMCMC which will be called something like <inputName>_Process.root, and make fancier error plots from it using the `GetPostfitParamPlots` app like:

```bash
GetPostfitParamPlots Test_drawCorr.root
```

Output should look like file below, and it convey same information as individual posteriors, it is more compact way of presenting same information useful for comparison especially if you have order of hundred parameters

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/27e3e4c8-629e-4c05-ac64-f6a7bec85331">

#### Correlation Matrix Plotting
If you have run **ProcessMCMC** with option "PlotCorr" you will have in output correlation matrix.
This is a handy tool for viewing how correlated parameters are. However mature analyses with hundreds of parameters may run into the problem of having too large plots.
To plot only a subset of parameters we recommend using **MatrixPlotter**.
```bash
bin/MatrixPlotter bin/TutorialDiagConfig.yaml Test_drawCorr.root
```
Within MatrixPlot.pdf you should see a plot like this.
<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/14471069-27e7-4ea3-9d75-232615aa246a">
In this example, you can see only two parameters. Using TutorialDiagConfig.yaml you can easily modify it either by adding more Titles, or more parameters.

```yaml
MatrixPlotter:
  Titles: [
    "Norm",
  ]
  Norm: ["Norm_Param_0", "Norm_Param_1", "BinnedSplineParam1",
  ]
```
## How to Develop Model of Systematic Uncertainties
In the next step you gonna modify analysis setup and repeat steps.
First let's better understand `TutorialConfigs/CovObjs/SystematicModel.yaml`. This config controls what systematic uncertainties will be used in the analysis for example like this:
```yaml
- Systematic:
    Names:
      FancyName: Norm_Param_0
    Error: 0.11
    FlatPrior: false
    IsFixed: false
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
PlotMCMCDiag Test_MCMC_Diag.root
```
If you add second/third arguemnt it will compare several files:

```bash
PlotMCMCDiag Test_MCMC_Diag.root SecondFile_MCMC_Diag.root
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

There are a number of apps included to make plots from the results of your fits, llh scans etc. You can find more details on them and how they work in the main MaCh3 wiki [here](https://github.com/mach3-software/MaCh3/wiki). There you will also find some instructions on how you can write yor own plotting scripts.

The plotting library is configured using yaml files. You can see some examples of such config files in the plotting directory, and a detailed explanation of them is given in [the wiki](https://github.com/mach3-software/MaCh3/wiki).

Some examples on how to make some "standard" plots are given below.

### How to run LLH scan
You can run MCMC in very similar way as MCMC
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

### Plotting with Python

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


## Issues
If you encountered any issues or find something confusing please contact us:
* [Kamil Skwarczynski](mailto:Kamil.Skwarczynski@rhul.ac.uk)<br>
  Royal Holloway, University of London, UK


