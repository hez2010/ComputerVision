#pragma once
#include "Shared.h"

class Hough {
public:
    Hough(int kAngleSplits);
    vector<tuple<int, int, int>> detect(CImg<u8>& contours, int kMinCount);
    vector<double> sin_table = {}, cos_table = {};
private:
    int kAngleSplits;
};