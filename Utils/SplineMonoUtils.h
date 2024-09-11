#pragma once

// MaCh3 spline includes
#include "splines/SplineMonolith.h"


std::vector< std::vector<TResponseFunction_red*> > GetMasterSpline(
                            std::string FileName, std::vector<std::string> DialNames);
