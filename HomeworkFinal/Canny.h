#pragma once
#include "Shared.h"

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

    CImg<u8> sobel();
    CImg<u8> non_max_suppression();
public:
    CImg<u8> detect(CImg<u8> input, int core_size, int threshold_low, int threshold_high);
};