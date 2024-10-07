## import the plotting manager from the plotting library
from pyMaCh3.pyMaCh3.plotting import PlottingManager

## this is needed in order to get the command line arguments
import sys

## get the matplotlib stuff
from matplotlib import pyplot as plt
import matplotlib.backends.backend_pdf

## some other python libraries
import numpy as np
import math as m

def test_LLH_scans(man):
    ''' Function to plot log likelihood scans '''

    pdf = matplotlib.backends.backend_pdf.PdfPages("LLH-scans.pdf")

    # loop through all parameters the manager knows about
    for i, param in enumerate( man.input().get_known_parameters() ):
        
        fig = plt.figure()
        plt.title(man.style().prettify_parameter_name(param))

        # go through all files specified in cmd line
        for file_id in range(man.input().get_n_input_files()):
            llh_scan = man.input().get_llh_scan(file_id, param, "total")
            plt.plot(llh_scan[0], llh_scan[1], label = man.get_file_label(file_id))
        
        plt.ylabel("LLH")
        plt.legend()

        pdf.savefig( fig )

    pdf.close()


# make a PlottingManager and initialise using the command line arguments
manager = PlottingManager()
manager.parse_inputs(sys.argv)

test_LLH_scans(manager)
