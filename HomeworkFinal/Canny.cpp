#include "Canny.h"
#include "Utils.h"

CImg<u8> Canny::detect(CImg<u8> input, int core_size, int threshold_low, int threshold_high) {
    vector<vector<double>> filter = Utils::create_filter(core_size, core_size, 1);

    grayscaled = input.spectrum() == 3 ? Utils::luminance(input) : CImg<u8>(input);

    gradient = Utils::gaussian(grayscaled, filter);

    sobel_filtered = sobel();

    non_max_suppressed = non_max_suppression();

    thres_result = Utils::threshold(non_max_suppressed, threshold_low, threshold_high);

    return thres_result;
}

CImg<u8> Canny::sobel()
{
    //Sobel X Filter
    double x1[] = { -1.0, 0, 1.0 };
    double x2[] = { -2.0, 0, 2.0 };
    double x3[] = { -1.0, 0, 1.0 };

    vector<vector<double>> xFilter(3);
    xFilter[0].assign(x1, x1 + 3);
    xFilter[1].assign(x2, x2 + 3);
    xFilter[2].assign(x3, x3 + 3);

    //Sobel Y Filter
    double y1[] = { 1.0, 2.0, 1.0 };
    double y2[] = { 0, 0, 0 };
    double y3[] = { -1.0, -2.0, -1.0 };

    vector<vector<double>> yFilter(3);
    yFilter[0].assign(y1, y1 + 3);
    yFilter[1].assign(y2, y2 + 3);
    yFilter[2].assign(y3, y3 + 3);

    //Limit Size
    int size = (int)xFilter.size() / 2;

    CImg<u8> filteredImg = CImg<u8>(gradient.width() - 2 * size, gradient.height() - 2 * size);

    angles_map = CImg<float>(gradient.width() - 2 * size, gradient.height() - 2 * size, 1, 3); //AngleMap

    for (int i = size; i < gradient.width() - size; i++)
    {
        for (int j = size; j < gradient.height() - size; j++)
        {
            double sumx = 0;
            double sumy = 0;

            for (int x = 0; x < xFilter.size(); x++)
                for (int y = 0; y < xFilter.size(); y++)
                {
                    sumx += xFilter[x][y] * (double)(*gradient.data(i + x - size, j + y - size)); //Sobel_X Filter Value
                    sumy += yFilter[x][y] * (double)(*gradient.data(i + x - size, j + y - size)); //Sobel_Y Filter Value
                }
            double sumxsq = sumx * sumx;
            double sumysq = sumy * sumy;

            double sq2 = sqrt(sumxsq + sumysq);

            if (sq2 > 255) //Unsigned Char Fix
                sq2 = 255;
            *filteredImg.data(i - size, j - size) = sq2;

            if (sumx == 0) //Arctan Fix
                *angles_map.data(i - size, j - size) = 90;
            else
                *angles_map.data(i - size, j - size) = atan(sumy / sumx);
        }
    }

    return filteredImg;
}

CImg<u8> Canny::non_max_suppression()
{
    CImg<u8> nonMaxSupped = CImg<u8>(sobel_filtered.width() - 2, sobel_filtered.height() - 2);
    for (int i = 1; i < sobel_filtered.width() - 1; i++) {
        for (int j = 1; j < sobel_filtered.height() - 1; j++) {
            float Tangent = *angles_map.data(i, j);

            *nonMaxSupped.data(i - 1, j - 1) = *sobel_filtered.data(i, j);
            //Horizontal Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((*sobel_filtered.data(i, j) < *sobel_filtered.data(i, j + 1)) || (*sobel_filtered.data(i, j) < *sobel_filtered.data(i, j - 1)))
                    *nonMaxSupped.data(i - 1, j - 1) = 0;
            }
            //Vertical Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((*sobel_filtered.data(i, j) < *sobel_filtered.data(i + 1, j)) || (*sobel_filtered.data(i, j) < *sobel_filtered.data(i - 1, j)))
                    *nonMaxSupped.data(i - 1, j - 1) = 0;
            }

            //-45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
                if ((*sobel_filtered.data(i, j) < *sobel_filtered.data(i - 1, j + 1)) || (*sobel_filtered.data(i, j) < *sobel_filtered.data(i + 1, j - 1)))
                    *nonMaxSupped.data(i - 1, j - 1) = 0;
            }

            //45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
                if ((*sobel_filtered.data(i, j) < *sobel_filtered.data(i + 1, j + 1)) || (*sobel_filtered.data(i, j) < *sobel_filtered.data(i - 1, j - 1)))
                    *nonMaxSupped.data(i - 1, j - 1) = 0;
            }
        }
    }
    return nonMaxSupped;
}
