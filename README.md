# Welcome to MaCh3 Tutorial
After this tutorial you should know how to run MCMC, implement systematic uncertainties and how to plot standard Bayesian diagnostic.

[![Code - Documented](https://img.shields.io/badge/Code-Documented-2ea44f)](https://github.com/mach3-software/MaCh3/wiki)
[![Container Image](https://img.shields.io/badge/Container-Image-brightgreen)](https://github.com/mach3-software/MaCh3Tutorial/pkgs/container/mach3tutorial)
[![Code - Doxygen](https://img.shields.io/badge/Code-Doxygen-2ea44f)](https://mach3-software.github.io/MaCh3/index.html)

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

## Introduction
Aim of tutorial is to teach you basic of running MCMC how to viusalise/analise it what are systematic uncertainties and samples, how to implement them.

Current setup include single sample with several cross-section and oscillation parameters/

## How to run MCMC
To run MCMC simply
```bash
./bin/MCMCTutorial Inputs/ManagerTest.yaml
```
Congratulations! 🎉
You have just completed finished you first MCMC chain.

### MCMC Chain
Being able to visualise and analyse output of MCMC is standard procedure after chain has finished, you can produce simple plots with
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root
```
where Test.root is the output of running MCMCTutorial as described [here](#how-to-run-mcmc)
One of plots you will encounter is:

It is marginalised posterior of a single parameter. This is main output of MCMC.
<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/1073a76e-5d82-4321-8952-e098d1b0717f">
**WARNING** Your posterior may look very shaky and slightly different to one in example. This is because you run chain with low number of steps. Meaning you don't have enough statistic to build posterior distribution. You can easily modify in `Inputs/ManagerTest.yaml`
```yaml
General:
  MCMC:
    NSteps: 10000
```
It is good homework to increase number of steps and see how much more smooth posterior becomes, but at the cost of having to wait more.

**ProcessMCMC** has much more plotting options, we recommend to see [here](https://github.com/mach3-software/MaCh3/wiki/09.-Bayesian-Analysis,-Plotting-and-MCMC-Processor) to get better idea what each plot mean.

You can then take the output of running ProcessMCMC which will be called something like <inputName>_Process.root, and make fancier error plots from it using the `GetPostfitParamPlots` app like:

```bash
GetPostfitParamPlots Test_drawCorr.root
```

Output should look like file below, and it convey same information as individual posteriors, it is more compact way of presenting same information useful for comparison especially if you have order of hundred parameters

<img width="350" alt="Posterior example" src="https://github.com/user-attachments/assets/27e3e4c8-629e-4c05-ac64-f6a7bec85331">

## How to Develop Model of Systematic Uncertainties
In the next step you gonna modify analysis setup and repeat steps.
First let's better understand `Inputs/SystematicsTest.yaml`. This config controls what systematic uncertainties will be used in the analysis for example like this:
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
    DetID: 985
    StepScale:
      MCMC: 0.2
    Type: Norm
```
If you want to read more about implementation please go [here](https://github.com/mach3-software/MaCh3/wiki/02.-Implementation-of-Systematic)

As first step let's modify `Error: 0.11` to `Error: 2.0`, this should significantly modify error which shou.d be noticeable in MCMC.

Lastly we need to modify name of output file. This is governed by manager class (read more [here](https://github.com/mach3-software/MaCh3/wiki/01.-Manager-and-config-handling)) modify `OutputFile: "Test.root"` in `Inputs/ManagerTest.yaml` to for example
`OutputFile: "Test_Modified.root"`.

Now let's run MCMC again
```bash
./bin/MCMCTutorial Inputs/ManagerTest.yaml
```
Congratulations! 🎉
Next step is to compare both chains.

Warning: If you modified files in main folder not build you will have to call make install!
## How to Plot Comparisons?
Now that you have two chains you can try comparing them using following.
```bash
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root Default_Chain Test_Modified.root Modified_Chain
```
This will produce pdf file with overlayed posteriors. Most should be similarly except modified parameter.

## How to Develop New Samples
First we gonna investigate how to modify sample, let's take a look at `Inputs/SamplePDF_Tutorial.yaml`. Each sample has set of cuts right now we only introduce cut on TrueNeutrinoEnergy.
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
Up to this point we only modified sample but how to add new one? First make copy of sample config `Inputs/SamplePDF_Tutorial.yaml` and call it `Inputs/SamplePDF_User.yaml`. For the moment feel free to change name, binning etc but keep inputs the same. Go wild! Next go to `Inputs/ManagerTest.yaml`
```yaml
General:
  TutorialSamples: ["Inputs/SamplePDF_Tutorial.yaml"]
```
To add you newly implemented sample you will have to expand config to for example:
```yaml
General:
  TutorialSamples: ["Inputs/SamplePDF_Tutorial.yaml", "Inputs/SamplePDF_User.yaml"]
```

## MCMC Diagnostic
Crucial part of MCMC is diagnostic whether chain converged or not. You can read more on [here](https://github.com/mach3-software/MaCh3/wiki/11.-Step-size-tuning)

```bash
./bin/DiagMCMC Test.root bin/TutorialDiagConfig.yaml
```

### Useful Settings
There are plenty of usefull settings
**Fitting Algorithm**: Most likely you run MCMC but what if you want to use algorith like Minuit2?
```yaml
General:
  FittingAlgorithm: "MCMC"
```
In `Inputs/ManagerTest.yaml` you should switch following setting to "Minuit2"

**LLH Type**:
By default we use Barlow-Beeston LLH, however several are implemented. For example by changing confign you can use Poisson or maybe IceCube.
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
./bin/LLHScanTutorial Inputs/ManagerTest.yaml
```
You can plot the results of an LLH scan using the aptly named PlotLLH app like so

```bash
PlotLLH LLH_Test.root
```
where LLH_Test.root is the result of running the LLH scan as described [here](#how-to-run-llh-scan).

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


