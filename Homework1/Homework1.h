#pragma once
#ifndef PI
#define PI 3.14159265358
#endif

#include <iostream>
#include <CImg.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

using namespace cimg_library;

class Homework1 {
private:
    CImg<byte>* image = nullptr;
    // 获取旋转变换后坐标
    std::pair<int, int> get_rotated_point(std::pair<int, int> size, std::pair<int, int> point, double degree);
public:
    Homework1() = delete;
    Homework1(const char* filename) {
        image = new CImg<byte>(filename);
    }

    ~Homework1() {
        delete image;
        image = nullptr;
    }

    void display();
    // 绘制三角形
    void draw_triangle(bool use_lib);
    // 绘制圆形
    void draw_circle(bool use_lib);
    // 绘制直线
    void draw_line(bool use_lib);
    // 绘制点
    void draw_point(int x, int y, const byte *color, bool use_lib);
    // 旋转图像
    void rotate(int degree, bool use_lib);
    // 保存图像
    void save(const char* filename) {
        image->save_bmp(filename);
    }
    
};