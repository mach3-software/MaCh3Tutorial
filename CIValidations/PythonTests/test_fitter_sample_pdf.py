## MaCh3 imports
import pyMaCh3
from pyMaCh3.pyMaCh3 import samples
from pyMaCh3.pyMaCh3 import splines
from pyMaCh3.pyMaCh3 import fitters
from pyMaCh3.pyMaCh3 import manager
from pyMaCh3.pyMaCh3 import parameters

## ROOT imports
import ROOT
from ROOT import TTree, TFile, TGraph

## Other imports
import numpy as np
import typing
import pytest
import sys

class test_sample(samples.SampleHandlerBase):

    def __init__(self, xsec_cov: parameters.ParameterHandlerBase):

        super(test_sample, self).__init__()

        self.n_splines: int = 0 ## Keep track of the number of spline parameters we have
        self.interp_types: typing.List[splines.InterpolationType] = [] ## Keep track of the interpolation type of spline parameters
        self.spline_names: typing.List[str] = [] ## Keep track of the names of spline parameters
        self.spline_indices: typing.List[int] = [] ## Keep track of the indices of spline parameters
        self.data_event_rate: float = 0.0 ## our "data"
        self.spline_monolith: splines.EventSplineMonolith = None ## The event spline monolith object that we'll use to reweight our events
        self.n_mc_events: int = 0 ## The number of unweighted mc events we have. Useful for iterating over
        self.event_weights: np.array = None ## Keep track of the individual weights of our mc events
        
        #self.set_xsec_cov(xsec_cov)
        self.xsec_cov: parameters.ParameterHandlerBase = xsec_cov

        self.proposed_param_array = np.asarray(xsec_cov.get_proposal_array())

        self.setup_splines("TutorialConfigs/MC/SplineFile.root")
        
        self.setup_MC("TutorialConfigs/MC/SplineFile.root")

        # Get the nominal event rate which we will later use as our "data"
        self.setup_asimov_data()

    def setup_asimov_data(self) -> None:
        ''' Build the Asimov data by setting all the parameters to their nominal values and weighting the events to these '''

        # First reset just in case this has already been run
        self.data_event_rate = 0.0
        
        # Now we set it in the spline monolith and calculate the event weights
        self.spline_monolith.set_param_value_array(self.proposed_param_array[self.spline_indices])
        self.spline_monolith.evaluate()

        # Now we get the overall nominal event rate by summing up all the nominal event weights
        for event_id in range(self.n_mc_events):
            self.data_event_rate += self.spline_monolith.get_event_weight(event_id) 

    def setup_splines(self, file_name: str) -> None:
        ''' Set up the event by event splines by reading them in from the specified spline file '''

        ## Open up the file and get the sample_sum tree from it
        spline_file: TFile = TFile(file_name)
        sample_sum: TTree = spline_file.sample_sum

        print(sample_sum.Print())

        ## check which parameters are spline parameters
        for i in range(self.xsec_cov.get_n_pars()):
            
            type: parameters.SystematicType = self.xsec_cov.get_par_type(i)

            if(type == parameters.SystematicType.Spline):

                spline_type: splines.InterpolationType = self.xsec_cov.get_par_spline_type(self.n_splines)
                spline_name: str = self.xsec_cov.get_par_spline_name(self.n_splines)

                self.spline_names.append(spline_name)
                self.interp_types.append(spline_type)
                self.spline_indices.append(i)

                self.n_splines += 1
        
        ## read the splines into the master spline array
        master_splines: typing.List[splines.ResponseFunction] = []
        
        for entry in sample_sum:

            entry_resp_fns: splines.ResponseFunction = [] # temporary list to hold response functions for this event

            # Only look for the names of spline parameters in the spline file
            for spline_name in self.spline_names:

                # Get the spline from the ttree
                spline: TGraph = getattr(entry, spline_name)[0]

                # hold the knot values of the splines
                x_vals: typing.List[float] = []
                y_vals: typing.List[float] = []

                # transfer the knot values from the TGraph in the file to the <x,y>_vals lists
                for i in range(spline.GetN()):
                    x_vals.append(float(spline.GetPointX(i)))
                    y_vals.append(float(spline.GetPointY(i)))

                # build the response function from the knots and add it to the list
                response: splines.ResponseFunction = splines.ResponseFunction(x_vals, y_vals, splines.InterpolationType.Cubic_TSpline3)
                entry_resp_fns.append(response)

            master_splines.append(entry_resp_fns)

        ## now we build ourselves an EventSplineMonolith object using our master_splines list
        self.spline_monolith = splines.EventSplineMonolith(master_splines)

    def setup_MC(self, file_name: str) -> None:
        ''' Set up our monte carlo events '''

        # Open up the input file and read in the event information from them
        # (currently this means just the number of events as input has no kinematic parameters) 
        spline_file: TFile = TFile(file_name)
        sample_sum: TTree = spline_file.sample_sum

        self.n_mc_events = sample_sum.GetEntries()
        self.event_weights = np.ones((self.n_mc_events))

    def get_likelihood(self) -> float:
        ''' Calculate the sample likelihood at the current point in parameter space '''

        # first we run the reweight function to get our weighted event rates
        self.reweight()

        # these will be our accumulated event rates that we'll use to calculate the likelihood
        mc: float = 0.0
        w2: float = 0.0 # <-the sum of the squared weights which is needed for some test stat calculations
        llh: float = 0.0

        # to get the "mc" bin content we loop through events and add up the event weights
        for event_id in range(self.n_mc_events):
            weight: float = self.spline_monolith.get_event_weight(event_id) 

            mc += weight
            w2 += weight * weight

        # we use the nominal weight as our "data" and calculate the "bin" llh
        llh = self.get_bin_LLH(self.data_event_rate, mc, w2)

        return llh
    
    def clean_memory_before_fit(self) -> None:
        pass

    def reweight(self) -> None:
        ''' Perform the event reweighting of our MC events. Currently this just does spline reweightign but normalisation should be added in future '''

        # First set the proposed parameter values in the spline monolith
        ## EM: I really don't want to have to do this
        ##     Want a way of getting just the spline parameter pointers from the xsec_cov and passing them to the monolith
        ##     just once at the start then they should track the changes
        self.spline_monolith.set_param_value_array(self.proposed_param_array[self.spline_indices])

        # Next we evaluate all the splines at their current values
        self.spline_monolith.evaluate()

        # loop through all our mc events and take the weights from the spline monolith
        for event_id in range(self.n_mc_events):
            self.event_weights[event_id] = self.spline_monolith.get_event_weight(event_id)

        return 

def test_sample_pdf(pytestconfig):
    # initialise our main manager object by passing it the yaml config specified on the command line
    man: manager.Manager = manager.Manager(pytestconfig.getoption("config"))
    man.print()

    # get the config files
    cov_names: typing.List[str] = ["CIValidations/PythonTests/SystematicModel_EventByEvent.yaml"]

    # make the covariance objects
    covariance: parameters.ParameterHandlerBase = parameters.ParameterHandlerGeneric(cov_names)

    sample: test_sample = test_sample(covariance)
    print("\nParameter values befores throw: ", sample.proposed_param_array)
    sample.reweight()
    print("Event weights before throw: ", sample.event_weights)
    print("Likelihood before throw: ", sample.get_likelihood())

    # propose a random step and see likelihoods at the new step
    covariance.propose_step()
    print("\nParameter values after throw: ", sample.proposed_param_array)
    sample.reweight()
    print("Event weights after throw: ", sample.event_weights)
    print("Likelihood after throw: ", sample.get_likelihood())

    # build a fitter object
    mcmc_fitter:fitters.MCMC = fitters.mcmc(man)
    mcmc_fitter.add_syst_object(covariance)
    mcmc_fitter.add_sample_handler(sample)

    # run an LLH scan
    mcmc_fitter.run_LLH_scan()

    # get some good ish step scales
    mcmc_fitter.get_step_scale_from_LLH_scan()

    # RUUUUUN!!!
    mcmc_fitter.set_chain_length(1000)
    mcmc_fitter.run()
