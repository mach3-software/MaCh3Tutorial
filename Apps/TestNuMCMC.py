#!/usr/bin/env python3

import uproot
import os
from numcmctools import PlotStack, MCMCSamples
import numpy as np
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
root_file_path = os.path.join(script_dir, "NewChain.root")

samples = MCMCSamples(root_file_path, "mcmc")

stack = PlotStack(samples)

stack.add_plot(["Deltam2_32", "Theta23"],[],[50,50],[[2.2E-3,2.7E-3],[0.7,0.9]])
stack.add_plot(["DeltaCP"],[],[-np.pi, -0.95*np.pi, -0.9*np.pi, -0.85*np.pi, -0.8*np.pi, -0.75*np.pi,
                                   -0.7*np.pi,  -0.65*np.pi,  -0.6*np.pi,  -0.55*np.pi,  -0.5*np.pi,
                                    -0.45*np.pi,  -0.4*np.pi,  -0.35*np.pi,  -0.3*np.pi,  -0.25*np.pi,
                                     -0.2*np.pi,  -0.15*np.pi,  -0.1*np.pi,  -0.05*np.pi, 0,
                                   0.2*np.pi, 0.4*np.pi, 0.6*np.pi, 0.8*np.pi, np.pi], mo_option=True)
stack.fill_plots()

fig = plt.figure()
axs = fig.subfigures(2, 2)

stack.plots[0].draw_plot(axs[0,0])
stack.plots[1].draw_plot(axs[1,0])

stack.plots[0].make_intervals([0.68,0.95])
stack.plots[0].draw_interval(axs[0,1])

stack.plots[1].make_intervals([0.68,0.95])
stack.plots[1].draw_interval(axs[1,1])
axs_ax = axs[1,1].get_axes()
axs_ax[0].set_xlabel(r'$\delta_{CP}$ NO')
axs_ax[1].set_xlabel(r'$\delta_{CP}$ IO')

plt.print("blarb.png")

