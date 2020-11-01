#pragma once

#include <tuple>
#include <iostream>

typedef std::pair<double, double> Point;
typedef std::tuple<double, double, double, double, double, double, double, double> PerspectiveParams;
typedef std::tuple<Point, Point, Point, Point> Rect;

class Transform {
public:
    static PerspectiveParams prepare_perspective_transform(Rect from, Rect to);
    static Point apply_perspective_transform(Point from, PerspectiveParams params);
};
