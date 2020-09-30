#include "Homework1.h"

void Homework1::display() {
	CImgDisplay disp;
	image->display(disp);
	disp.move(100, 100);
	disp.paint();
	disp.flush();
	while (!disp.is_keyENTER() && !disp.is_keySPACE()) {
		disp.wait();
	}
}

std::pair<int, int> Homework1::get_rotated_point(std::pair<int, int> size, std::pair<int, int> point, double degree) {
	// 计算从中心点平移到左上角后的坐标
	double x = point.first - size.first / 2, y = point.second - size.second / 2;
	// 转换为弧度
	double degree_rad = degree / 180.0 * PI;
	// 计算变换后坐标
	return std::make_pair(
		(int)(x * std::cos(degree_rad) + y * std::sin(degree_rad)) + size.first / 2,
		(int)(-x * std::sin(degree_rad) + y * std::cos(degree_rad)) + size.second / 2
	);
}

void Homework1::draw_point(int x, int y, const byte* color, bool use_lib) {
	if (use_lib) {
		image->draw_point(x, y, color, 1);
	}
	else {
		// 获取深度和通道数
		int depth = image->depth(), channel = image->spectrum();
		for (int i = 0; i < depth; i++) {
			for (int j = 0; j < channel; j++) {
				// 绘制点
				*(image->data(x, y, i, j)) = *(color + j);
			}
		}
	}
}

void Homework1::draw_triangle(bool use_lib) {
	const byte green[] = { 0, 255, 0 };

	if (use_lib) {
		image->draw_triangle(60, 60 - 15 / std::sqrt(3) * 2,
			60 - 15, 60 + (15 * std::sqrt(3) - 15 / std::sqrt(3) * 2),
			60 + 15, 60 + (15 * std::sqrt(3) - 15 / std::sqrt(3) * 2),
			green, 1);
	}
	else {
		// 取得顶部位置
		uint32_t top = 60 - 15 / std::sqrt(3) * 2;
		// 遍历边长
		for (int i = 0; i <= 30; i++) {
			// 计算边长为 i 时的中垂线坐标
			uint32_t y = i / 2 * std::sqrt(3);
			uint32_t x = i / 2;
			// 从左至右依次绘制点
			for (int j = 60 - x; j <= 60 + x; j++) {
				draw_point(j, top + y, green, use_lib);
			}
		}
	}
}

void Homework1::draw_circle(bool use_lib) {
	const byte yellow[] = { 255, 255, 0 };

	if (use_lib) {
		image->draw_circle(60, 60, 20, yellow, 1);
	}
	else {
		const uint32_t center = 60;
		// 从上至下按行绘制
		for (int i = 0; i <= 40; i++) {
			// 计算当前行 y 坐标
			uint32_t y = center - 20 + i;
			uint32_t dx = std::sqrt(std::pow(20, 2) - std::pow(std::abs(20 - i), 2));
			// 从左至右依次绘制点
			uint32_t x1 = center - dx;
			uint32_t x2 = center + dx;
			for (int j = x1; j <= x2; j++) {
				draw_point(j, y, yellow, use_lib);
			}
		}
	}
}

void Homework1::draw_line(bool use_lib) {
	const byte red[] = { 255, 0, 0 };

	if (use_lib) {
		image->draw_line(0, 0, 100 * std::sqrt(2), 100 * std::sqrt(2), red, 1);
	}
	else {
		// 按长度绘制线条
		for (int i = 0; i <= 100; i++) {
			// 计算横纵坐标
			uint32_t x = i * std::sqrt(2);
			uint32_t y = i * std::sqrt(2);
			// 绘制点
			draw_point(x, y, red, use_lib);
		}
	}
}

void Homework1::rotate(int degree, bool use_lib) {
	int width = image->width(), height = image->height(), depth = image->depth(), channel = image->spectrum();
	if (use_lib) {
		image->rotate(degree, width / 2.0, height / 2.0, 1);
	}
	else {
		std::pair<int, int> size = std::make_pair(width, height);
		CImg<byte>* newImg = new CImg<byte>(width, height, depth, channel);

		// 清空图像
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				for (int z = 0; z < depth; z++) {
					for (int c = 0; c < channel; c++) {
						*newImg->data(x, y, z, c) = 0;
					}
				}
			}
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				// 取得旋转变换后的坐标
				auto point = get_rotated_point(size, std::make_pair(x, y), -degree);
				// 确保坐标在图像有效区域内
				if (point.first < 0
					|| point.first >= width
					|| point.second < 0
					|| point.second >= height) {
					continue;
				}
				// 变换后坐标依次复制原坐标下的亮度值
				for (int z = 0; z < depth; z++) {
					for (int c = 0; c < channel; c++) {
						byte* data = image->data(x, y, z, c);
						*newImg->data(point.first, point.second, z, c) = *data;
					}
				}
			}
		}
		delete image;
		image = newImg;
	}
}