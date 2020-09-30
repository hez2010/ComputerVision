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

typedef unsigned char byte;

class Canny {
private:
    // Original Image
    CImg<byte> img;
    // Grayscale
    CImg<byte> grayscaled;
    // Gradient
    CImg<byte> gradient;
    // Sobel Filtered
    CImg<byte> sobel_filtered;
    // Angle Map
    CImg<float> angles_map;
    // Non-maxima supp.
    CImg<byte> non_max_suppressed;
    // Double threshold
    CImg<byte> thres_result;
    // Contour result
    CImg<byte> contour_result;

    // 创建 Gaussian 滤波器
    vector<vector<double>> create_filter(int row, int column, double sigmaIn);
    // 将图片转为灰度图
    CImg<byte> luminance(CImg<byte> imgIn);
    // Gaussian 滤波
    CImg<byte> gaussian(CImg<byte> imgIn, vector<vector<double>> filter);
    // Sobel 滤波
    CImg<byte> sobel();
    // 非最大值抑制
    CImg<byte> non_max_suppression();
    // 双阈值
    template<typename T>
    CImg<T> threshold(CImg<T> imgIn, int low, int high);
    // 连接边缘
    CImg<byte> link_contours(CImg<byte> imgIn);
    // 删除小于 20 的连续边缘
    void remove_contours(CImg<byte> &imgIn, CImg<bool>& map, int x, int y, int step, bool &clean);
public:
    void detect(const char* filename, int core_size, int threshold_low, int threshold_high);
};