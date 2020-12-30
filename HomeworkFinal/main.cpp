#include "Shared.h"
#include "Hough.h"
#include "Canny.h"
#include "Utils.h"
#include "UFS.h"
#include "Transform.h"
#include <svm.h>

const int kAngleSplits = 1024;

// CImgDisplay disp;

int predict(CImg<u8> input, svm_model* model) {
    constexpr int cols = 28, rows = 28;
    constexpr int size = cols * rows;
    CImg<u8> image = CImg<u8>(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            *image.data(i, j) = 0;
        }
    }

    double factor = max(input.width() / 24.0, input.height() / 24.0);
    input = input.resize(input.width() / factor, input.height() / factor, input.depth(), input.spectrum(), 3);
    int offsetX = (rows - input.width()) / 2, offsetY = (cols - input.height()) / 2;
    for (int i = 0; i < input.width(); i++) {
        for (int j = 0; j < input.height(); j++) {
            int x = offsetX + i - 2, y = offsetY + j - 2;
            if (x < 0 || y < 0 || x >= rows || x >= cols) continue;
            *image.data(x, y) = *input.data(i, j);
        }
    }

    CImg<u8> feature = CImg<u8>(14, 14);

    svm_node* node = new svm_node[size / 4 + 1];

    for (int i = 0; i < rows / 2; i++) {
        for (int j = 0; j < cols / 2; j++) {
            node[i * rows / 2 + j].index = i * rows / 2 + j + 1;
            auto value = (*image.data(i * 2, j * 2) +
                *image.data(i * 2, j * 2 + 1) +
                *image.data(i * 2 + 1, j * 2) +
                *image.data(i * 2 + 1, j * 2 + 1)) / 4;
            node[i * rows / 2 + j].value = value;
            *feature.data(i, j) = node[i * rows / 2 + j].value;
        }
    }
    /*feature.display(disp);
    do {
        disp.wait();
    } while (!disp.is_keyENTER());*/
    node[size / 4].index = -1;
    u8 result = (u8)svm_predict(model, node);
    delete[] node;
    return (int)result;
}

int count_region(CImg<u8>& img, int x, int y) {
    int result = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (*img.data(x + i, y + j) > 0) result++;
        }
    }
    return result;
}

vector<pair<int, int>> get_region(CImg<u8>& img, CImg<bool>& map, int x, int y) {
    vector<pair<int, int>> result = {};
    queue<pair<int, int>> q = {};

    const int dx[] = { 1,-1,0,0 };
    const int dy[] = { 0,0,1,-1 };

    q.push(make_pair(x, y));
    *map.data(x, y) = true;
    while (!q.empty()) {
        auto& f = q.front();
        q.pop();
        result.push_back(f);
        for (int i = 0; i < 4; i++) {
            int fx = f.first + dx[i], fy = f.second + dy[i];
            if (fx <= 0 || fx >= img.width() - 1 || fy <= 0 || fy >= img.height() - 1) continue;
            if (!(*map.data(fx, fy)) && count_region(img, fx, fy)) {
                *map.data(fx, fy) = true;
                q.push(make_pair(fx, fy));
            }
        }
    }

    return result;
}

vector<vector<pair<int, int>>> find_regions(CImg<u8> img) {
    CImg<u8> clone = CImg<u8>(img);
    vector<vector<pair<int, int>>> result = {};

    CImg<bool> map = CImg<bool>(img.width(), img.height());
    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            *map.data(i, j) = false;
        }
    }

    for (int i = 1; i < img.width() - 1; i++) {
        for (int j = 1; j < img.height() - 1; j++) {
            if (count_region(clone, i, j) >= 6) {
                auto region = get_region(clone, map, i, j);
                if (region.size() > 0) {
                    for (auto& x : region) {
                        *clone.data(x.first, x.second) = 0;
                    }
                    result.push_back(region);
                }
            }
        }
    }
    return result;
}

int main(int argc, char** argv) {
    // disp.show();
    if (argc <= 2) {
        printf("Usage: classifier.exe image model\n");
        return 0;
    }

    printf("[Info] Loading image: %s...\n", *(argv + 1));
    CImg<u8> image = CImg<u8>(*(argv + 1));
    image = image.resize(600, 800, image.depth(), image.spectrum(), 3);

    printf("[Info] Processing contours detection...\n");
    Canny canny;
    auto contours = canny.detect(image, 1, 50, 80);

    printf("[Info] Processing lines detection...\n");

    Hough hough(kAngleSplits);
    int w = contours.width(), h = contours.height();

    auto result = hough.detect(contours, 200);

    printf("[Info] Found %d lines\n", (int)result.size());

    printf("[Info] Processing corners detection...\n");
    int diagonal = (int)sqrt(w * w + h * h);
    int len = result.size();
    vector<Point> intersections = {};
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < i; j++) {
            int t1 = get<0>(result[i]), r1 = get<1>(result[i]) - diagonal;
            int t2 = get<0>(result[j]), r2 = get<1>(result[j]) - diagonal;
            auto inter = Utils::get_intersection(hough, t1, r1, t2, r2);
            if (inter.first < -w / 2 || inter.first > 3 * w / 2) continue;
            if (inter.second < -h / 2 || inter.second > 3 * h / 2) continue;
            intersections.push_back(inter);
        }
    }

    int intersLen = intersections.size();
    UFS ufs = UFS(intersLen);
    for (int i = 0; i < intersLen; i++) {
        for (int j = 0; j < i; j++) {
            auto& point1 = intersections[i];
            auto& point2 = intersections[j];
            if (sqrt(pow(point1.first - point2.first, 2) + pow(point1.second - point2.second, 2)) < 100) {
                ufs.merge(i, j);
            }
        }
    }

    unordered_map<int, vector<Point>> cornersGroup = {};
    vector<Point> corners = {};

    for (int i = 0; i < intersLen; i++) {
        int f = ufs.find(i);
        if (!cornersGroup.count(f)) cornersGroup[f] = {};
        cornersGroup[f].push_back(intersections[i]);
    }

    for (auto& i : cornersGroup) {
        corners.push_back(*i.second.begin());
    }

    sort(corners.begin(), corners.end(), [w, h](Point& a, Point& b)->bool {
        auto dis1 = min(min(min(pow(a.first, 2) + pow(a.second, 2),
            pow(a.first - w, 2) + pow(a.second, 2)),
            pow(a.first, 2) + pow(a.second - h, 2)),
            pow(a.first - w, 2) + pow(a.second - h, 2));
        auto dis2 = min(min(min(pow(b.first, 2) + pow(b.second, 2),
            pow(b.first - w, 2) + pow(b.second, 2)),
            pow(b.first, 2) + pow(b.second - h, 2)),
            pow(b.first - w, 2) + pow(b.second - h, 2));
        return dis1 < dis2;
        });

    for (auto& i : corners) {
        printf("[Info] Found candidate corner: (%lf, %lf)\n", i.first, i.second);
    }

    if (corners.size() < 4) {
        printf("[Error] Cannot find enough corners\n");
        return 0;
    }

    vector<Point> sorted_corners = {};
    for (int i = 0; i < 4; i++) {
        sorted_corners.push_back(corners[i]);
    }

    sort(sorted_corners.begin(), sorted_corners.end(), [w, h](Point& a, Point& b) -> bool {
        return a.first + a.second < b.first + b.second;
        });

    for (auto& i : sorted_corners) {
        printf("[Info] Filtered and sorted corner: (%lf, %lf)\n", i.first, i.second);
    }

    printf("[Info] Processing perspective transform...\n");

    auto transformed = Transform::perspective_transform(
        image,
        std::make_tuple(
            sorted_corners[0],
            sorted_corners[1].first < sorted_corners[2].first ? sorted_corners[1] : sorted_corners[2],
            sorted_corners[1].first < sorted_corners[2].first ? sorted_corners[2] : sorted_corners[1],
            sorted_corners[3]
        ),
        std::make_tuple(
            std::make_pair(0, 0),
            std::make_pair(0, sorted_corners[3].second - sorted_corners[0].second),
            std::make_pair(sorted_corners[3].first - sorted_corners[0].first, 0),
            std::make_pair(sorted_corners[3].first - sorted_corners[0].first, sorted_corners[3].second - sorted_corners[0].second)
        )
    );

    printf("[Info] Processing regions detection...\n");

    CImg<u8> processed = Utils::rect(Utils::inverse(Utils::luminance(transformed)), 20, 20, 20, 20);

    auto contours2 = canny.detect(processed, 1, 50, 80).dilate(1, 8);
    const unsigned char color[] = { 255 };
    auto regions = find_regions(contours2);
    vector<tuple<int, int, int, int>> digits = {};
    for (auto& i : regions) {
        int minX = contours2.width() - 1, minY = contours2.height() - 1, maxX = 0, maxY = 0;
        for (auto& j : i) {
            minX = min(minX, j.first);
            minY = min(minY, j.second);
            maxX = max(maxX, j.first);
            maxY = max(maxY, j.second);
        }
        if ((maxY - minY) * (maxX - minX) < 20) continue;
        printf("[Info] Located bounding box: (%d, %d, %d, %d)\n", minX, minY, maxX, maxY);
        digits.push_back(make_tuple(minX, minY, maxX, maxY));
    }

    UFS digitsLines(digits.size());
    for (int i = 0; i < digits.size(); i++) {
        for (int j = 0; j < i; j++) {
            if (abs(get<1>(digits[i]) - get<1>(digits[j])) < 20) {
                digitsLines.merge(i, j);
            }
        }
    }

    map<int, vector<tuple<int, int, int, int>>> groupedDigits = {};
    for (int i = 0; i < digits.size(); i++) {
        int index = digitsLines.find(i);
        if (!groupedDigits.count(get<1>(digits[index]))) groupedDigits[get<1>(digits[index])] = {};
        groupedDigits[get<1>(digits[index])].push_back(digits[i]);
    }

    printf("[Info] Loading SVM model: %s...\n", *(argv + 2));
    auto model = svm_load_model(*(argv + 2));

    printf("[Info] Processing number prediction...\n");

    CImg<u8> predicted = CImg<u8>(processed.dilate(2));
    char* text = new char[10];

    for (auto& i : groupedDigits) {
        printf("[Info] Line prediction result: ");
        sort(i.second.begin(), i.second.end(), [](tuple<int, int, int, int>& a, tuple<int, int, int, int>& b) -> bool {
            return get<0>(a) < get<0>(b);
            });

        for (auto& j : i.second) {
            auto digitRect = Utils::rect(processed, get<0>(j), get<1>(j), processed.width() - get<2>(j), processed.height() - get<3>(j));
            auto predictResult = predict(digitRect, model);
            printf("%d", predictResult);
            memset(text, 0, 10 * sizeof(char));
            text = itoa(predictResult, text, 10);
            predicted.draw_text(get<0>(j), get<3>(j), text, color, 0, 1, 20);
        }
        printf("\n");
    }

    delete[] text;

    predicted.display();

    return 0;
}
