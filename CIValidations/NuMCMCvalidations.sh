./bin/MCMCTutorial Inputs/FitterConfig.yaml

./CIValidations/NuMCMCconversion Test.root NewChain.root

pip install -r NuMCMCTools/requirements.txt

export PYTHONPATH="$PYTHONPATH:/${MaCh3Tutorial_ROOT}/NuMCMCTools"

cp NewChain.root CIValidations/NewChain.root

python3 CIValidations/TestNuMCMC.py
