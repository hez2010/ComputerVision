#include "Shared.h"
#include "Hough.h"
#include "Canny.h"
#include "Utils.h"
#include "UFS.h"
#include "Transform.h"

const int kAngleSplits = 1024;

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("Usage: classifier.exe image\n");
        return 0;
    }
    printf("[Info] Load image: %s...\n", *(argv + 1));
    CImg<u8> image = CImg<u8>(*(argv + 1)).resize(600, 800);
    printf("[Info] Processing canny detection...\n");
    Canny canny;
    auto contours = canny.detect(image, 1, 50, 80);

    printf("[Info] Processing hough detection...\n");

    Hough hough(kAngleSplits);
    int w = contours.width(), h = contours.height();
    auto result = hough.detect(contours, min(w, h) / 3);

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
        printf("[Error] Cannot find enough corners.\n");
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
        printf("[Info] Filtered corner: (%lf, %lf)\n", i.first, i.second);
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

    transformed.display();

    return 0;
}