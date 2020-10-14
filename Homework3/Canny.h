#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
#include <CImg.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;
using namespace cimg_library;

typedef unsigned char u8;

class Canny {
private:
    // Original Image
    CImg<u8> img;
    // Grayscale
    CImg<u8> grayscaled;
    // Gradient
    CImg<u8> gradient;
    // Sobel Filtered
    CImg<u8> sobel_filtered;
    // Angle Map
    CImg<float> angles_map;
    // Non-maxima supp.
    CImg<u8> non_max_suppressed;
    // Double threshold
    CImg<u8> thres_result;
    // Contour result
    CImg<u8> contour_result;

    // 创建 Gaussian 滤波器
    vector<vector<double>> create_filter(int row, int column, double sigmaIn);
    // 将图片转为灰度图
    CImg<u8> luminance(CImg<u8> imgIn);
    // Gaussian 滤波
    CImg<u8> gaussian(CImg<u8> imgIn, vector<vector<double>> filter);
    // Sobel 滤波
    CImg<u8> sobel();
    // 非最大值抑制
    CImg<u8> non_max_suppression();
    // 双阈值
    template<typename T>
    CImg<T> threshold(CImg<T> imgIn, int low, int high);
    // 连接边缘
    CImg<u8> link_contours(CImg<u8> imgIn);
    // 删除小于 20 的连续边缘
    void remove_contours(CImg<u8> &imgIn, CImg<bool>& map, int x, int y, int step, bool &clean);
public:
    CImg<u8> detect(CImg<u8> input, int core_size, int threshold_low, int threshold_high);
};