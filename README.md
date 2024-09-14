## How to Start?
To compile simply
```
mkdir build;
cd build;
cmake ../
make -jN [set number of threads]
make install
```
then
```
source bin/setup.MaCh3.sh
source bin/setup.MaCh3Validations.sh
```
alternatively you can use containers by
```
docker pull ghcr.io/mach3-software/mach3tutorial:alma9latest
```
To reed more how to use containers check our wiki [here](https://github.com/mach3-software/MaCh3/wiki/12.-Containers)

## How to run MCMC
To run MCMC simply
```
./bin/MCMCTutorial Inputs/ManagerTest.yaml
```
Congratulations! 🎉
You have just completed finished you first MCMC chain.

## How to Plot?
You can produce simple plots with
```
./bin/ProcessMCMC bin/TutorialDiagConfig.yaml Test.root
```

## How to Modify?
add syst to config, add new sample with binning, mode whatever

## How to Plot Comparisons?
MCMC process, plot LLH

Asher suggested to also include some simple MCMC diag like auto correlations

## More advanced development????
