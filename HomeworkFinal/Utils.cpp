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
