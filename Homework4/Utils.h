#pragma once

#include <cstdlib>
#include <utility>
#include <cmath>
#include <tuple>

class Utils {
public:
    static double calc_distance(int x, int y, double a, double b, double c);
    static std::tuple<double, double, double> calc_line(std::pair<int, int> p1, std::pair<int, int> p2);
    static std::pair<int, int> calc_cpoint(std::tuple<double, double, double> line1, std::tuple<double, double, double> line2);
    static double calc_kvalue(std::tuple<double, double, double> line);
};