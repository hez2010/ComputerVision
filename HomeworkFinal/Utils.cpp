#include "Utils.h"
#include "Matrix.h"

pair<double, double> Utils::get_intersection(Hough& hough, int t1, double r1, int t2, double r2) {

    double** leftData = new double* [] {
        new double[] {hough.cos_table[t1], hough.sin_table[t1]},
            new double[] {hough.cos_table[t2], hough.sin_table[t2]}
    };
    auto left = Matrix(leftData, 2, 2);
    double** rightData = new double* [] {
        new double[] {r1},
            new double[] {r2}
    };
    auto right = Matrix(rightData, 2, 1);
    auto point = left.inverse() * right;
    auto result = make_pair(point(0, 0), point(1, 0));
    for (int i = 0; i < 2; i++) {
        delete[] leftData[i];
        delete[] rightData[i];
    }
    delete[] leftData;
    delete[] rightData;
    return result;
}

pair<double, double> Utils::map_point(pair<double, double> point, double factorX, double factorY) {
    return make_pair(point.first * factorX, point.second * factorY);
}

CImg<u8> Utils::luminance(CImg<u8>& img) {
    CImg<u8> newImg = CImg<u8>(img.width(), img.height());

    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            int b = *(img.data(i, j, 0, 0));
            int g = *(img.data(i, j, 0, 1));
            int r = *(img.data(i, j, 0, 2));

            double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
            *newImg.data(i, j, 0, 0) = newValue;
        }
    }

    return newImg;
}

CImg<u8> Utils::inverse(CImg<u8>& img) {
    CImg<u8> newImg = CImg<u8>(img.width(), img.height(), img.depth(), img.spectrum());

    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            for (int d = 0; d < img.depth(); d++) {
                for (int c = 0; c < img.spectrum(); c++) {
                    *newImg.data(i, j, d, c) = 255 - *img.data(i, j, d, c);
                }
            }
        }
    }

    return newImg;
}

CImg<u8> Utils::gaussian(CImg<u8>& imgIn, vector<vector<double>>& filter)
{
    int size = (int)filter.size() / 2;
    CImg<u8> filteredImg = CImg<u8>(imgIn.width() - 2 * size, imgIn.height() - 2 * size, imgIn.depth());
    for (int i = size; i < imgIn.width() - size; i++)
    {
        for (int j = size; j < imgIn.height() - size; j++)
        {
            double sum = 0;

            for (int x = 0; x < filter.size(); x++)
                for (int y = 0; y < filter.size(); y++)
                {
                    sum += filter[x][y] * (double)(*imgIn.data(i + x - size, j + y - size));
                }

            *filteredImg.data(i - size, j - size) = sum;
        }

    }
    return filteredImg;
}

vector<vector<double>> Utils::create_filter(int row, int column, double sigmaIn)
{
    vector<vector<double>> filter;

    for (int i = 0; i < row; i++)
    {
        vector<double> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
        filter.push_back(col);
    }

    float coordSum = 0;
    float constant = 2.0 * sigmaIn * sigmaIn;

    // Sum is for normalization
    float sum = 0.0;

    for (int x = -row / 2; x <= row / 2; x++)
    {
        for (int y = -column / 2; y <= column / 2; y++)
        {
            coordSum = (x * x + y * y);
            filter[x + row / 2][y + column / 2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
            sum += filter[x + row / 2][y + column / 2];
        }
    }

    // Normalize the Filter
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            filter[i][j] /= sum;

    return filter;
}

CImg<u8> Utils::threshold(CImg<u8>& imgIn, int low, int high)
{
    if (low > 255)
        low = 255;
    if (high > 255)
        high = 255;

    CImg<u8> EdgeMat = CImg<u8>(imgIn.width(), imgIn.height(), imgIn.depth(), imgIn.spectrum());

    for (int i = 0; i < imgIn.width(); i++)
    {
        for (int j = 0; j < imgIn.height(); j++)
        {
            *EdgeMat.data(i, j) = *imgIn.data(i, j);
            if (*EdgeMat.data(i, j) > high)
                *EdgeMat.data(i, j) = 255;
            else if (*EdgeMat.data(i, j) < low)
                *EdgeMat.data(i, j) = 0;
            else
            {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x = i - 1; x < i + 2; x++)
                {
                    for (int y = j - 1; y < j + 2; y++)
                    {
                        if (x <= 0 || y <= 0 || EdgeMat.width() || y > EdgeMat.height()) //Out of bounds
                            continue;
                        else
                        {
                            if (*EdgeMat.data(x, y) > high)
                            {
                                *EdgeMat.data(i, j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if (*EdgeMat.data(x, y) <= high && *EdgeMat.data(x, y) >= low)
                                anyBetween = true;
                        }
                    }
                    if (anyHigh)
                        break;
                }
                if (!anyHigh && anyBetween)
                    for (int x = i - 2; x < i + 3; x++)
                    {
                        for (int y = j - 1; y < j + 3; y++)
                        {
                            if (x < 0 || y < 0 || x > EdgeMat.width() || y > EdgeMat.height()) //Out of bounds
                                continue;
                            else
                            {
                                if (*EdgeMat.data(x, y) > high)
                                {
                                    *EdgeMat.data(i, j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if (anyHigh)
                            break;
                    }
                if (!anyHigh)
                    *EdgeMat.data(i, j) = 0;
            }
        }
    }
    return EdgeMat;
}

CImg<u8> Utils::rect(CImg<u8>& img, int l, int t, int r, int b) {
    CImg<u8> newImg = CImg<u8>(img.width() - r - l, img.height() - b - t, img.depth(), img.spectrum());
    for (int i = 0; i < img.width() - r - l; i++) {
        for (int j = 0; j < img.height() - b - t; j++) {
            if (i + l >= img.width() || j + t >= img.height()) continue;
            for (int d = 0; d < img.depth(); d++) {
                for (int c = 0; c < img.spectrum(); c++) {
                    *newImg.data(i, j, d, c) = *img.data(i + l, j + t, d, c);
                }
            }
        }
    }
    return newImg;
}