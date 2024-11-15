# Mach3 inputs
import pyMaCh3
from pyMaCh3 import splines
# Other python includes
import numpy as np

def get_test_response_fns():
    '''
    return a list of dictionaries containing information on test response functions which have keys:
      'fn': the response function
      'x' : the knot x values
      'y' : the knot y values
    '''

    # set up some test response functions
    # values are totally arbitrary
    # You can add some more here if you like and they will be picked up by 
    # the tests below if e.g. you want to test some weird special case 
    
    response_1_x, response_1_y = [0.0, 1.0, 2.0], [1.0, 0.5, 2.0]
    response_1 = splines.ResponseFunction(response_1_x, response_1_y, splines.InterpolationType.Cubic_TSpline3)

    response_2_x, response_2_y = [10.0, 11.0, 12.0], [-2.0, 0.0, 0.5]
    response_2 = splines.ResponseFunction(response_2_x, response_2_y, splines.InterpolationType.Linear)
    
    response_3_x, response_3_y = [-2.0, -0.5, 0.0, 0.5], [10.0, 10.0, 0.0, 0.0]
    response_3 = splines.ResponseFunction(response_3_x, response_3_y, splines.InterpolationType.Cubic_Monotonic)

    return (
        {"fn": response_1, "x": response_1_x, "y": response_1_y},
        {"fn": response_2, "x": response_2_x, "y": response_2_y},
        {"fn": response_3, "x": response_3_x, "y": response_3_y}
    )

def test_response_fns_at_knots():
    '''
    go through each test response function and each of the knot x values 
    and check that the response function gives exactly the knot y value
    '''
    
    response_fn_dicts = get_test_response_fns()

    print("Testing that spline knots evaluate to correct y value")

    for i, response in enumerate(response_fn_dicts):
        print("for test function {}".format(i))
        for x, y in zip(response["x"], response["y"]):
            eval_y = response["fn"].evaluate(x)
            print("x = {} :: True y = {} :: Eval y = {}".format(x, y, eval_y))
            assert( eval_y == y )
        print()

def test_monolith(n_values = 10):
    '''
    High level test of the event by event spline monolith.

    Sets up the monolith and sets the parameter values to some random values then 
    checks that the calculated event weight is as expected

    n_values: the number of random values to test at
    '''

    print("\n##### Testing spline monolith #####\n")

    response_fn_dicts = get_test_response_fns()
    response_fn_dicts_copy = get_test_response_fns() # <- building the monolith deletes the response fns so need to keep a copy
    
    # RNG with specific seed so that we have some "deterministic" randomness
    rng = np.random.default_rng(12345)

    # set up the parameter value array
    # initialised to some default values
    param_array = np.zeros(len(response_fn_dicts))

    # build the monolith
    monolith = splines.EventSplineMonolith([[response["fn"] for response in response_fn_dicts_copy]])

    monolith.set_param_value_array(param_array)
    
    # go through and set the parameter array to randomised values
    # check that the monolith gives the same weight we expect to get by just evaluating 
    # each of the response functions and multiplying together
    for i_val in range(n_values):

        print("--- Test point {} ---".format(i_val))
        expected_weight = 1.0
        
        for idx, response in enumerate(response_fn_dicts):

            # get a random number in the range covered by the response function
            rand_val = rng.uniform(response["x"][-1], response["x"][0])

            param_array[idx] = rand_val

            expected_weight *= response["fn"].evaluate(rand_val)

        monolith.evaluate()
        calculated_weight = monolith.get_event_weight(0)

        print("param array: {}".format(param_array))
        print("Event 0 weight expected weight = {} :: monolith weight = {}".format(expected_weight, calculated_weight))
        print()

        assert(expected_weight - calculated_weight < 1e-5)

# TODO add proper test for the interpolation, this really only checks that it actually runs, which is redundant given the knot check above
def test_interpolation():
    
    response_fn_dicts = get_test_response_fns()

    for i, response in enumerate(response_fn_dicts):
        for val in [0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]:
            print("response fn {} at {}: {}".format(i, val, response["fn"].evaluate(val)))

        print()


