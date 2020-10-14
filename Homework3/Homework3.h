#pragma once
#include "Canny.h"
#include <cstdlib>
#include <vector>
#include <utility>
#include <tuple>
#include "Utils.h"

class NameCard
{
private:
    CImg<u8> image;
public:
    CImg<u8> load(char *filename);
    CImg<u8> find_contours();
    std::tuple<double, double, double> ransac(CImg<u8> &contours, int iter_cnt);
    std::vector<std::pair<int, int>> check(CImg<u8> &contours, std::pair<int, int> p1, std::pair<int, int> p2, std::tuple<double, double, double> line);
    void draw_line(std::tuple<double, double, double> line);
    void draw_point(std::pair<int, int> point);
    CImg<u8>& get();
};
