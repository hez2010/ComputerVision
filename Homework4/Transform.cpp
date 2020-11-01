#include "Transform.h"
#include "Matrix.h"

PerspectiveParams Transform::prepare_perspective_transform(Rect from, Rect to) {
    auto x1 = std::get<0>(from).first;
    auto x2 = std::get<1>(from).first;
    auto x3 = std::get<2>(from).first;
    auto x4 = std::get<3>(from).first;
    auto y1 = std::get<0>(from).second;
    auto y2 = std::get<1>(from).second;
    auto y3 = std::get<2>(from).second;
    auto y4 = std::get<3>(from).second;
    auto u1 = std::get<0>(to).first;
    auto u2 = std::get<1>(to).first;
    auto u3 = std::get<2>(to).first;
    auto u4 = std::get<3>(to).first;
    auto v1 = std::get<0>(to).second;
    auto v2 = std::get<1>(to).second;
    auto v3 = std::get<2>(to).second;
    auto v4 = std::get<3>(to).second;

    double** data = new double*[] {
        new double[] {x1,y1,1,0,0,0,-x1*u1,-y1*u1},
        new double[] {0,0,0,x1,y1,1,-x1*v1,-y1*v1},
        new double[] {x2,y2,1,0,0,0,-x2*u2,-y2*u2},
        new double[] {0,0,0,x2,y2,1,-x2*v2,-y2*v2},
        new double[] {x3,y3,1,0,0,0,-x3*u3,-y3*u3},
        new double[] {0,0,0,x3,y3,1,-x3*v3,-y3*v3},
        new double[] {x4,y4,1,0,0,0,-x4*u4,-y4*u4},
        new double[] {0,0,0,x4,y4,1,-x4*v4,-y4*v4}
    };
    auto transform_matrix = Matrix(data, 8, 8);
    auto inv_matrix = transform_matrix.inverse();

    double** right = new double*[] { new double[] {u1},new double[] {v1},new double[] {u2},new double[] {v2},new double[] {u3},new double[] {v3},new double[] {u4},new double[] {v4} };
    auto right_matrix = Matrix(right, 8, 1);
    auto multiplied = inv_matrix * right_matrix;

    auto result = std::make_tuple(multiplied(0, 0), multiplied(1, 0), multiplied(2, 0), multiplied(3, 0), multiplied(4, 0), multiplied(5, 0), multiplied(6, 0), multiplied(7, 0));
    for (int i = 0; i < 8; i++) {
        delete[] data[i];
        delete[] right[i];
    }
    delete[] data;
    delete[] right;
    return result;
}

Point Transform::apply_perspective_transform(Point origin, PerspectiveParams params) {
    double m0 = std::get<0>(params);
    double m1 = std::get<1>(params);
    double m2 = std::get<2>(params);
    double m3 = std::get<3>(params);
    double m4 = std::get<4>(params);
    double m5 = std::get<5>(params);
    double m6 = std::get<6>(params);
    double m7 = std::get<7>(params);
    double x = origin.first, y = origin.second;
    return std::make_pair(
        (m0 * x + m1 * y + m2) / (m6 * x + m7 * y + 1),
        (m3 * x + m4 * y + m5) / (m6 * x + m7 * y + 1)
    );
}
