#pragma once
#include "Shared.h"
#include "Hough.h"

class Utils {
public:
    static pair<double, double> get_intersection(Hough& hough, int t1, double r1, int t2, double r2);
};
