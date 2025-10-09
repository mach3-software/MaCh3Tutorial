"""
A basic test of the fitter library.
This will do very quick stat only fits with each of the implemented fitters using 
basic xsec and osc covariances.
@author: Ewan Miller
"""

# MaCh3 includes
import pyMaCh3
from pyMaCh3.pyMaCh3 import fitters
from pyMaCh3.pyMaCh3 import manager
from pyMaCh3.pyMaCh3 import parameters
# other python includes
import sys
import pytest

def setup_manager(pytestconfig):

    man: manager.Manager = manager.Manager(pytestconfig.getoption("config"))
    man.print()

    # get the config files
    xsec_covs = [i.data() for i in man.raw()["General"]["Systematics"]["XsecCovFile"]]

    # make the covariance objects
    xsec_covariance = parameters.ParameterHandlerGeneric(xsec_covs, name="XSec_covariance")

    return (man, xsec_covariance)

def test_mcmc(pytestconfig):
    """ 
    Basic test of the MCMC fitter.
    - Set parameter handler objects (but not sample handler).
    - Run the basic fitter functions (1d LLH scan, getting step size from LLH, MCMC algorithm).
    """

    # first set up the manager and covariances based on the specified options for this test run
    man, xsec_covariance = setup_manager(pytestconfig)
    
    # build the mcmc fitter
    mcmc_fitter = fitters.mcmc(man)
    mcmc_fitter.add_syst_object(xsec_covariance)

    # run an LLH scan
    mcmc_fitter.run_LLH_scan()

    # get some good ish step scales
    mcmc_fitter.get_step_scale_from_LLH_scan()

    # RUUUUUN!!!
    mcmc_fitter.set_chain_length(10000)
    mcmc_fitter.run()

    assert(True) # <- test passed successfully

def test_minuit_fit(pytestconfig):
    """ 
    Basic test of the Minuit fitter.
    - Set parameter handler objects (but not sample handler).
    - Run the fit.

    Note that osc is not supported for this fitter in c++ so is not supported for python and so won't be tested
    """
    
    # first set up the manager and covariances based on the specified options for this test run
    man, xsec_covariance = setup_manager(pytestconfig)

    # build the minuit fit
    minuit_fit = fitters.MinuitFit(man)
    minuit_fit.add_syst_object(xsec_covariance)
     
    # RUUUUUN!!!
    minuit_fit.run()

    assert(True) # <- test passed successfully


## PSO Doesn't currently work
## so leave out for now (by adding the _ to the start)
def _test_PSO(pytestconfig):
    """ 
    Basic test of the PSO fitter.
    - Set parameter handler objects (but not sample handler).
    - Run the fit.

    Note that osc is not supported for this fitter in c++ so is not supported for python and so won't be tested
    """
    
    # first set up the manager and covariances based on the specified options for this test run
    man, xsec_covariance, osc_covariance = setup_manager(pytestconfig)

    # build the PSO fit
    PSO_fit = fitters.PSO(man)
    PSO_fit.add_syst_object(xsec_covariance)
    PSO_fit.add_syst_object(osc_covariance)
    PSO_fit.init()
    
    # RUUUUUN!!!
    PSO_fit.run()

    assert(True)
