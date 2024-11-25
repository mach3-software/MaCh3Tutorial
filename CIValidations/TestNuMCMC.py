#!/usr/bin/env python3

import uproot
import os
from numcmctools import PlotStack, MCMCSamples
import numpy as np
import matplotlib.pyplot as plt

# Get the path of the test MCMC chain file
script_dir = os.path.dirname(os.path.abspath(__file__))
root_file_path = os.path.join(script_dir, "NewChain.root")

# Create the MCMCSamples object
samples = MCMCSamples(root_file_path, "mcmc")

#make a plotstack
stack = PlotStack(samples)

#define a 1D and 2D plot; note the 1D plot has both non-equal bins and is split by mass-ordering
stack.add_plot(["Deltam2_32", "Theta23"],[],[50,50],[[2.2E-3,2.7E-3],[0.7,0.9]])
stack.add_plot(["DeltaCP"],[],[-np.pi, -0.95*np.pi, -0.9*np.pi, -0.85*np.pi, -0.8*np.pi, -0.75*np.pi,
                                   -0.7*np.pi,  -0.65*np.pi,  -0.6*np.pi,  -0.55*np.pi,  -0.5*np.pi,
                                    -0.45*np.pi,  -0.4*np.pi,  -0.35*np.pi,  -0.3*np.pi,  -0.25*np.pi,
                                     -0.2*np.pi,  -0.15*np.pi,  -0.1*np.pi,  -0.05*np.pi, 0,
                                   0.2*np.pi, 0.4*np.pi, 0.6*np.pi, 0.8*np.pi, np.pi], mo_option=True)
#fill the plots
stack.fill_plots()


# Here we show for a simple case how to manage the drawing of plots in detail

# Create figure and a gridspace
fig = plt.figure()
gs = fig.add_gridspec(2, 4)

# Make plt.Axes object for each subplot
ax1 = fig.add_subplot(gs[0, :2])
ax2 = fig.add_subplot(gs[0, 2:])
ax3 = fig.add_subplot(gs[1, :2])

# Merge y axis for the very last plot, where we want no and io next to each other
gs_sub = gs[1, 2:].subgridspec(1, 2, wspace=0)
ax4_no = fig.add_subplot(gs_sub[0, 0])
ax4_io =  fig.add_subplot(gs_sub[0, 1], sharey=ax4_no)
plt.setp(ax4_io.get_yticklabels(), visible=False)

# Draw all the plots
stack.plots[0].draw_plot(ax1)
stack.plots[1].draw_plot(ax3)

stack.plots[0].make_intervals([0.68,0.95])
stack.plots[0].draw_interval(ax2)

stack.plots[1].make_intervals([0.68,0.95])
stack.plots[1].draw_interval(ax3)
stack.plots[1].draw_interval([ax4_no, ax4_io])

fig.tight_layout()
fig.savefig('simpleplots.pdf')
