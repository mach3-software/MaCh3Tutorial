# Mach3 inputs
import pyMaCh3
from pyMaCh3 import splines
# Other python includes
import numpy as np

def test_monolith():
    # set up the response functions
    response1 = splines.ResponseFunction([0.0, 1.0, 2.0], [1.0, 0.5, 2.0], splines.InterpolationType.Cubic_TSpline3)
    response2 = splines.ResponseFunction([10.0, 11.0, 12.0], [-2.0, 0.0, 0.5], splines.InterpolationType.Linear)

    for val in [0.0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]:
        print("response1 at {}: {}".format(val, response1.evaluate(val)))

    # set up the parameter value array
    param_array = np.array([0.0, 12.0])

    # build the monolith
    monolith = splines.EventSplineMonolith([[response1, response2]])

    monolith.set_param_value_array(param_array)

    monolith.evaluate()

    print("param array: {}", param_array)
    print("Event 0 weight:")
    print(monolith.get_event_weight(0))


    param_array[:] = np.array([1.0, 11.0])

    monolith.evaluate()
    print("param array: {}", param_array)
    print("Event 0 weight after param change:")
    print(monolith.get_event_weight(0))

    assert(True)

test_monolith()

