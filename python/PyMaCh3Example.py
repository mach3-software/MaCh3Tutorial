#!/usr/bin/env python
# coding: utf-8

# # Welcome to pyMaCh3!
# 
# This notebook is meant to provide a brief introduction to the pyMaCh3 library. It will demonstrate basic functionality and examples of how to use the bindings that have been made from the C++ code of MaCh3. As more bindings are added then more examples can be added.
# 
# The first step is setting up your environment to find and use pyMaCh3. For python to find the library that has been made, you need to setup your PYTHONPATH environment variable to include the directory where the library is installed. If you are using a conda environment then I would recommend adding that PYTHONPATH to your envrionment to make it easier to find the library.

# import python stuff
import os

# import 3rd party libraries
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Now import pyMaCh3
import pyMaCh3_tutorial as m3

print(f'MaCh3_ROOT env var: {os.environ["MaCh3_ROOT"]}')

# # First steps
# 
# Now that pyMaCh3 is imported we can do some basic operations on MaCh3 objects. We can try creating parameter and sample handler objects and make some nice plots from them.

# Create a parameter handler object from the SystematicModel.yaml file
parameter_handler = m3.parameters.ParameterHandlerGeneric(["TutorialConfigs/CovObjs/SystematicModel.yaml", "TutorialConfigs/CovObjs/OscillationModel.yaml"])

# Voila! We now have a parameterHandler object to play with in python. For fun, let's throw from this matrix, save the parameter values in a dataframe and then plot them using matplotlib.

# Check how many parameters there are in the parameter handler
num_parameters = parameter_handler.get_n_pars()
print(f"Number of parameters: {num_parameters}")

n_toys = 1000

# Create an array of the parameter names
parameter_names = [f"{parameter_handler.get_fancy_par_name(i)}" for i in range(num_parameters)]

# Create a dataframe to store parameter values with the parameter names as the columns and the number of toys as the index
parameters_df = pd.DataFrame(columns=parameter_names)

for toy in range(n_toys):
    # throw the parameter_handler object
    parameter_handler.propose_step()
    parameters_df.loc[toy] = parameter_handler.get_proposal_array()

print(f"Threw {n_toys} and filled dictionary with parameter values")
print(f"Parameter dataframe has shape: {parameters_df.shape}")
print(f"Dataframe columns are: {parameters_df.columns}")

# Now we can simply use some of the functionality from pandas to draw a scatter plot of the parameters.

# Plot a scatter matrix of the parameters
Axes = pd.plotting.scatter_matrix(parameters_df, alpha=0.2, figsize=(24,12), diagonal='kde')

# This just makes the y-axis labels somewheat readable (or at least not terribly overlapping)
[plt.setp(item.yaxis.get_label(), 'size', 6) for item in Axes.ravel()]

# Let's show the plot!
plt.savefig("toy-throws.png")


# As you can see, this all looks perfectly sensible and you can even see the inbuild prior correlations between some parameters.
# 
# We can now move on to creating a sample handler object.

# # Sample Handler example
# 
# We will create a sample handler object and produce some spectra from a few toys we threw.

sample_handler = m3.samples.SampleHandlerTutorial("TutorialConfigs/Samples/SampleHandler_Tutorial.yaml", parameter_handler)

# As you can see we've now create a sample handler object and you can see the usual C++ stdout from MaCh3. We can now produce a histogram of the MC prediction from MaCh3.

sample_handler.reweight()
mc_prediction, bin_edges_x, bin_edges_y = sample_handler.get_mc_hist(0)

# count the number of non-zero entries in the MC prediction
non_zero_entries = np.count_nonzero(mc_prediction)
print(f"Number of non-zero entries in MC prediction: {non_zero_entries}")

#Now pass this to matplotlib to draw a histogram
plt.figure(figsize=(10, 6))
plt.hist(bin_edges_x[:-1], bins=bin_edges_x, weights=mc_prediction, histtype='step', 
         linewidth=2, label="MC prediction")
plt.xlabel("Energy [GeV]", fontsize=12)
plt.ylabel("Events", fontsize=12)
plt.legend(fontsize=11)
plt.grid(True, alpha=0.3)
plt.title("MaCh3 Tutorial - MC Prediction", fontsize=14)
plt.tight_layout()
plt.savefig("MC-prediction.png")

# Finally, let's bring the two bits of code together and produce many spectra from the sample handler from the parameter handler throws we previously made.

# Produce many spectra using the parameter values in the parameters_df
mc_prediction_integral = np.zeros(n_toys)

for toy in range(n_toys):
    # Set the parameter values from the dataframe
    parameter_handler.set_parameters(parameters_df.iloc[toy].values)
    # Re-weight the sample handler
    sample_handler.reweight()
    # Get the MC prediction and integrate it
    mc_prediction, _, _ = sample_handler.get_mc_hist(0)
    mc_prediction_integral[toy] = np.sum(mc_prediction)

# Plot the integrals of the MC predictions
plt.figure(figsize=(10, 6))
plt.hist(mc_prediction_integral, bins=20, histtype='step', 
         linewidth=2, label="MC prediction integral")
plt.xlabel("MC prediction integral", fontsize=12)
plt.ylabel("Events", fontsize=12)
plt.legend(fontsize=11)

plt.savefig("MC-prediction-integrals.png")
# And just like that we have a prior uncertainty on the total number of events in our prediction. This is a very simple example but it might inspire more complicated python scripts.
