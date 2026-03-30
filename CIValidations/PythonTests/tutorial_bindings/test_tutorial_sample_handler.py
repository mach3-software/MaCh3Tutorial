import os

# Now import pyMaCh3
from pyMaCh3_tutorial import parameters
from pyMaCh3_tutorial import samples

import pytest

print(f'MaCh3_ROOT env var: {os.environ["MaCh3_ROOT"]}')

def test_tutorial_sample_handler(pytestconfig):
    
    # Create a parameter handler object from the SystematicModel.yaml file
    parameter_handler = parameters.ParameterHandlerGeneric(["TutorialConfigs/CovObjs/SystematicModel.yaml", "TutorialConfigs/CovObjs/OscillationModel.yaml"])

    # create the samplehandler object
    sample_handler = samples.SampleHandlerTutorial("TutorialConfigs/Samples/SampleHandler_Tutorial.yaml", parameter_handler)

    # now try reweighting and getting the weighted histogram
    sample_handler.reweight()
    mc_prediction, bin_edges_x, bin_edges_y = sample_handler.get_mc_hist(0)
    data, bin_edges_x, bin_edges_y = sample_handler.get_data_hist(0)
