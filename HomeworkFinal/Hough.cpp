#include "Hough.h"

Hough::Hough(int kAngleSplits) {
    this->kAngleSplits = kAngleSplits;
    const double kTableConst = M_PI / kAngleSplits;
    for (int i = 0; i < kAngleSplits; i++) {
        sin_table.push_back(sin(kTableConst * i));
        cos_table.push_back(cos(kTableConst * i));
    }
}

vector<tuple<int, int, int>> Hough::detect(CImg<u8>& contours, int kMinCount) {
    int w = contours.width(), h = contours.height();
    int diagonal = (int)sqrt(w * w + h * h);
    int d2 = diagonal * 2;
    vector<u8> dst_image = {};
    for (int i = 0; i < d2; i++) {
        for (int j = 0; j < kAngleSplits; j++) {
            dst_image.push_back(0);
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (*contours.data(x, y) == 0) continue;
            for (int t = 0; t < kAngleSplits; t++) {
                int r = (int)(x * cos_table[t] + y * sin_table[t] + 0.5);
                int p = (r + diagonal) * kAngleSplits + t;
                dst_image[p] = dst_image[p] + 1;
            }
        }
    }

    int max_count = 0;
    vector<tuple<int, int, int>> result = {};
    while (true) {
        max_count = 0;
        int t_max = 0, r_max = 0;
        for (int r = 0; r < d2; r++) {
            for (int t = 0; t < kAngleSplits; t++) {
                int cnt = dst_image[r * kAngleSplits + t];
                if (max_count < cnt) {
                    max_count = cnt;
                    t_max = t;
                    r_max = r;
                }
            }
        }
        if (max_count < kMinCount) break;
        result.push_back(make_tuple(t_max, r_max, max_count));
        for (int r = -10; r <= 10; r++) {
            for (int t = -30; t <= 30; t++) {
                int t2 = t_max + t, r2 = r_max + r;
                if (t2 < 0) {
                    t2 += kAngleSplits;
                    r2 = -r2;
                }
                if (t2 >= kAngleSplits) {
                    t2 -= kAngleSplits;
                    r2 = -r2;
                }
                if (r2 < 0 || r2 >= d2) continue;
                dst_image[r2 * kAngleSplits + t2] = 0;
            }
        }
    }
    return result;
}