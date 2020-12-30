#pragma once
#include "Shared.h"
#include "Hough.h"

class Utils {
public:
    static pair<double, double> get_intersection(Hough& hough, int t1, double r1, int t2, double r2);
    static pair<double, double> map_point(pair<double, double> point, double factorX, double factorY);
    static CImg<u8> luminance(CImg<u8>& img);
    static CImg<u8> inverse(CImg<u8>& img);
    static CImg<u8> gaussian(CImg<u8>& imgIn, vector<vector<double>>& filter);
    static vector<vector<double>> create_filter(int row, int column, double sigmaIn);
    static CImg<u8> threshold(CImg<u8>& imgIn, int low, int high);
    static CImg<u8> rect(CImg<u8>& img, int l, int t, int r, int b);
};
