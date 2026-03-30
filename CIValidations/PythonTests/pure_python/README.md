# Pure Python Extension Tests

These tests check extending the core MaCh3 code in pure python.

Current tests are:

* test_fitter_sample_pdf.py - creates a sample handler written in pure python, instantiates it, along with manager and parameterHandler, then runs short mcmc fit with it
* test_fitter_stat_only.pdf - runs stat only mcmc and minuit fits with ParameterHandlerGeneric instance
* test_splines.py - tests the splinehandler class and the interpolation functions
