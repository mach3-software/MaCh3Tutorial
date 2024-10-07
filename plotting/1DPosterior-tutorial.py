## import the plotting manager from the plotting library
from pyMaCh3.pyMaCh3.plotting import PlottingManager

## this is needed in order to get the command line arguments
import sys

## get the matplotlib stuff
from matplotlib import pyplot as plt
import matplotlib.collections as mcoll
from matplotlib import cm
import matplotlib.backends.backend_pdf

## some other python libraries
import numpy as np
import math as m
import imageio

def plot_1d_posteriors(man):
    pdf = matplotlib.backends.backend_pdf.PdfPages("1d-posteriors.pdf")

    # loop through all parameters the manager knows about
    for i, param in enumerate( man.input().get_known_parameters() ):
        
        fig = plt.figure()
        plt.title(man.style().prettify_parameter_name(param))

        # go through all files specified in cmd line
        for file_id in range(man.input().get_n_input_files()):
            posterior = man.input().get_1d_posterior(file_id, param)
            plt.plot(posterior[0], posterior[1], label = man.get_file_label(file_id))
        
        plt.ylabel("N Steps")
        plt.legend()

        pdf.savefig( fig )

    pdf.close()

# make a PlottingManager and initialise using the command line arguments
manager = PlottingManager()
manager.parse_inputs(sys.argv)

plot_1d_posteriors(manager)