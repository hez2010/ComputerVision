#include "Utils.h"

// 计算点到直线距离
double Utils::calc_distance(int x, int y, double a, double b, double c)
{
    return std::abs(a * x + b * y + c) / std::sqrt(std::pow(a, 2) + std::pow(b, 2));
}

// 计算直线方程
std::tuple<double, double, double> Utils::calc_line(std::pair<int, int> p1, std::pair<int, int> p2)
{
    auto x1 = p1.first, x2 = p2.first, y1 = p1.second, y2 = p2.second;
    return std::make_tuple(y2 - y1, x1 - x2, x2 * y1 - x1 * y2);
}

// 计算交点
std::pair<int, int> Utils::calc_cpoint(std::tuple<double, double, double> line1, std::tuple<double, double, double> line2) {
    auto a1 = std::get<0>(line1), b1 = std::get<1>(line1), c1 = std::get<2>(line1);
    auto a2 = std::get<0>(line2), b2 = std::get<1>(line2), c2 = std::get<2>(line2);

    // 无交点
    if (a1 * b2 == a2 * b1) {
        return std::make_pair(2147483647, 2147483647);
    }

    return std::make_pair(
        (int)((c2 * b1 - c1 * b2) / (a1 * b2 - a2 * b1)),
        (int)((c1 * a2 - c2 * a1) / (a1 * b2 - a2 * b1))
    );
}

// 计算斜率
double Utils::calc_kvalue(std::tuple<double, double, double> line) {
    if (std::get<1>(line) == 0) return 2147483647;
    return -std::get<0>(line) / std::get<1>(line);
}