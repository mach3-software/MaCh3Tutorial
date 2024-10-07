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

cmap = cm.inferno

def make_segments(x, y):
    """
    Create list of line segments from x and y coordinates, in the correct format
    for LineCollection: an array of the form numlines x (points per line) x 2 (x
    and y) array
    """

    points = np.array([x, y]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    return segments

def test_MCMC(man):
    
    ## make the plot
    fig, ax = plt.subplots() 
    
    ## The number of steps in the chain
    n_steps = man.input().get_n_MCMC_entries(0)

    pdf = matplotlib.backends.backend_pdf.PdfPages("MCMC-plots.pdf")

    for param_1 in man.input().get_tagged_parameters(["oscillation"], "any"):
        
        ## check that param_1 actually has MCMC steps
        if not param_1 in man.input().get_known_MCMC_parameters(0): 
            continue
            
        for param_2 in man.input().get_tagged_parameters(["oscillation"], "any"):
            
            ## check that param_2 actually has MCMC steps
            if not param_2 in man.input().get_known_MCMC_parameters(0): 
                continue

            if param_1 == param_2: 
                continue

            print ( "on parameters {} and {}".format(param_1, param_2) )
            
            ## hold the values of each of the 2 parameters
            ax1_vals = []
            ax2_vals = []

            for step in range(0, n_steps):
                if ( step % m.floor(n_steps / 5) == 0 ):
                    print ( "Step {} / {} :: {} %".format(step, n_steps, 100.0 * float(step) / float(n_steps)) )
                    
                man.input().get_MCMC_entry(0, step)
                
                ax1_vals.append( man.input().get_MCMC_value(0, param_1))
                ax2_vals.append( man.input().get_MCMC_value(0, param_2))

            
            ## make line segments and colour gradient values
            segments = make_segments( ax1_vals, ax2_vals )
            colours = np.arange( n_steps )
            
            ## make the collection of line segments
            lc = mcoll.LineCollection(segments, linewidth=1, alpha=0.75, array=colours, cmap=cmap)

    
            plt.cla()
            ax.add_collection(lc)
            ax.set_facecolor('k')
            
            plt.xlabel(man.style().prettify_parameter_name(param_1))
            plt.ylabel(man.style().prettify_parameter_name(param_2))

            ax1_array = np.array(ax1_vals)
            ax2_array = np.array(ax2_vals)

            plt.xlim(0.95 * ax1_array.min(), 1.05 * ax1_array.max())
            plt.ylim(0.95 * ax2_array.min(), 1.05 * ax2_array.max())
            
            cbar = plt.colorbar(lc, label="Step")
            plt.plot()

            pdf.savefig( fig )

            cbar.remove()

    pdf.close()

# make a PlottingManager and initialise
manager = PlottingManager()
manager.parse_inputs(sys.argv)

test_MCMC(manager)