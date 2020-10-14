#include "Homework3.h"

CImg<u8> NameCard::load(char* filename)
{
    image = CImg<u8>(filename);
    return image;
}

CImg<u8> NameCard::find_contours()
{
    // Canny 查找边缘
    Canny canny;
    return canny.detect(image, 3, 40, 100);
}

// ransac
std::tuple<double, double, double> NameCard::ransac(CImg<u8>& contours, int iter_cnt) {
    std::tuple<double, double, double> ret_line;
    int width = contours.width();
    int height = contours.height();

    std::vector<std::pair<int, int>> max_result;

    while (iter_cnt--) {
        // 随机采样
        std::pair<int, int> p1, p2;
        do {
            p1 = std::make_pair(rand() % width, rand() % height);
        } while (*contours.data(p1.first, p1.second) == 0);
        do {
            p2 = std::make_pair(rand() % width, rand() % height);
        } while (*contours.data(p1.first, p1.second) == 0 || (p1.first == p2.first && p1.second == p2.second));

        // 计算直线方程
        auto line = Utils::calc_line(p1, p2);
        auto result = check(contours, p1, p2, line);
        if (result.size() > max_result.size()) {
            max_result = result;
            ret_line = line;
        }
    }

    // 将检测出的直线删除
    for (auto& i : max_result) {
        *contours.data(i.first, i.second) = 0;
    }

    return ret_line;
}

// ransac - 检查其他点
std::vector<std::pair<int, int>> NameCard::check(CImg<u8>& contours, std::pair<int, int> p1, std::pair<int, int> p2, std::tuple<double, double, double> line) {
    std::vector<std::pair<int, int>> points;
    int width = contours.width();
    int height = contours.height();

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            // 排除作为采样的点
            if (i == p1.first && j == p1.second) continue;
            if (i == p2.first && j == p2.second) continue;
            if (*contours.data(i, j) > 0) {
                // 计算距离
                auto distance = Utils::calc_distance(i, j, std::get<0>(line), std::get<1>(line), std::get<2>(line));
                if (distance <= 2) {
                    points.push_back(std::make_pair(i, j));
                }
            }
        }
    }

    return points;
}

// 画直线
void NameCard::draw_line(std::tuple<double, double, double> line) {
    const u8 red[] = { 255,0,0 };
    // A = 0 => k = 0
    if (std::get<0>(line) == 0) {
        // y = -C / B
        auto y = -std::get<2>(line) / std::get<1>(line);
        image.draw_line(0, y, image.width() - 1, y, red, 1);
        return;
    }

    // B = 0 => k = ∞
    if (std::get<1>(line) == 0) {
        // x = -C / A
        auto x = -std::get<2>(line) / std::get<0>(line);
        image.draw_line(x, 0, x, image.height() - 1, red, 1);
        return;
    }

    // let x = 0 => y = -C / B;
    auto y1 = -std::get<2>(line) / std::get<1>(line);
    // let x = max => y = (-C - A*max) / B;
    auto y2 = (-std::get<2>(line) - std::get<0>(line) * image.width()) / std::get<1>(line);
    image.draw_line(0, y1, image.width() - 1, y2, red, 1);
}

void NameCard::draw_point(std::pair<int, int> point) {
    const u8 green[] = { 0,255,0 };
    image.draw_point(point.first, point.second, green, 1);
}

CImg<u8>& NameCard::get() { return image; }
