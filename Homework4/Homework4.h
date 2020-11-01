#pragma once

#include <cstdlib>
#include <vector>
#include <utility>
#include <tuple>
#include <cmath>
#include <ctime>
#include "Canny.h"
#include "Utils.h"
#include "Transform.h"

class NameCard
{
private:
    CImg<u8> image;
public:
    CImg<u8> load(char *filename);
    CImg<u8> find_contours();
    std::tuple<double, double, double> ransac(CImg<u8> &contours, int iter_cnt);
    int check(CImg<u8> &contours, std::pair<int, int> p1, std::pair<int, int> p2);
    void remove(CImg<u8> &contours, std::pair<int, int> p1, std::pair<int, int> p2);
    void draw_line(std::tuple<double, double, double> line);
    void draw_point(std::pair<int, int> point);
    CImg<u8> perspective_transform(Rect from, Rect to);
    CImg<u8>& get();
};
