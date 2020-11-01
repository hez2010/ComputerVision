#include "Canny.h"

CImg<u8> Canny::detect(CImg<u8> input, int core_size, int threshold_low, int threshold_high) {
    vector<vector<double>> filter = create_filter(core_size, core_size, 1);

    // 将图片转为灰度图
    grayscaled = luminance(input);
    // grayscaled.save_bmp("1.grayscaled.bmp");
    // Gaussian 滤波
    gradient = gaussian(grayscaled, filter);
    // gradient.save_bmp("2.gradient.bmp");
    // Sobel 滤波
    sobel_filtered = sobel();
    // sobel_filtered.save_bmp("3.sobel_filtered.bmp");
    // 非最大值抑制
    non_max_suppressed = non_max_suppression();
    // non_max_suppressed.save_bmp("4.non_max_suppressed.bmp");
    // 双阈值
    thres_result = threshold(non_max_suppressed, threshold_low, threshold_high);
    // thres_result.save_bmp("5.thres_result.bmp");
    auto final_result = link_contours(thres_result);
    return final_result;
}

CImg<u8> Canny::luminance(CImg<u8> img) {
    CImg<u8> newImg = CImg<u8>(img.width(), img.height());

    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            int b = *(img.data(i, j, 0, 0));
            int g = *(img.data(i, j, 0, 1));
            int r = *(img.data(i, j, 0, 2));
            // 计算灰度值
            double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
            *newImg.data(i, j, 0, 0) = newValue;
        }
    }

    return newImg;
}

CImg<u8> Canny::gaussian(CImg<u8> imgIn, vector<vector<double>> filter)
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

vector<vector<double>> Canny::create_filter(int row, int column, double sigmaIn)
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

template<typename T>
CImg<T> Canny::threshold(CImg<T> imgIn, int low, int high)
{
    // 将超出范围的阈值设置为 255
    if (low > 255)
        low = 255;
    if (high > 255)
        high = 255;

    CImg<T> EdgeMat = CImg<T>(imgIn.width(), imgIn.height(), imgIn.depth(), imgIn.spectrum());

    for (int i = 0; i < imgIn.width(); i++)
    {
        for (int j = 0; j < imgIn.height(); j++)
        {
            // 根据双阈值进行二值化
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

void Canny::remove_contours(CImg<u8>& imgIn, CImg<bool>& map, int x, int y, int step, bool& clean) {
    // 方向向量
    const int dx[] = { 1,-1,0,0,1,1,-1,-1 };
    const int dy[] = { 0,0,1,-1,1,-1,1,-1 };
    bool found = false;
    for (int i = 0; i < 8; i++) {
        // 判断是否在区域内、是否访问过以及是否有值
        if (x + dx[i] >= 0 &&
            y + dy[i] >= 0 &&
            x + dx[i] < map.width() &&
            y + dy[i] < map.height() &&
            !(*map.data(x + dx[i], y + dy[i])) &&
            *imgIn.data(x + dx[i], y + dy[i]) > 0) {
            *map.data(x + dx[i], y + dy[i]) = true;
            found = true;
            clean = false;
            // 递归查找
            remove_contours(imgIn, map, x + dx[i], y + dy[i], step + 1, clean);
        }
    }

    // 未找到新的相邻边缘且已有边缘长度小于 20
    if (!found && step < 20) {
        clean = true;
    }
    // 回溯时清理边缘
    if (clean) {
        *imgIn.data(x, y) = 0;
    }
}

CImg<u8> Canny::link_contours(CImg<u8> imgIn) {
    // 记录是否已访问
    CImg<bool> map = CImg<bool>(imgIn.width(), imgIn.height());

    for (int i = 0; i < map.width(); i++) {
        for (int j = 0; j < map.height(); j++) {
            *map.data(i, j) = 0;
        }
    }

    for (int i = 0; i < imgIn.width(); i++) {
        for (int j = 0; j < imgIn.height(); j++) {
            // 如果未访问过且含有边缘信息
            if (!(*map.data(i, j))) {
                if (*imgIn.data(i, j) > 0) {
                    bool clean = false;
                    remove_contours(imgIn, map, i, j, 1, clean);
                }
            }
        }
    }
    return imgIn;
}
