./bin/MCMCTutorial Inputs/ManagerTest.yaml

./Apps/NuMCMCconversion Test.root NewChain.root

pip install -r NuMCMCTools/requirements.txt

export PYTHONPATH="$PYTHONPATH:/${MaCh3Tutorial_ROOT}/NuMCMCTools"

cp NewChain.root Apps/NewChain.root

python3 Apps/TestNuMCMC.py
