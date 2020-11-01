#include "Homework4.h"

// 寻找四个角点
void find_matches(std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& src, int* count, int step, int* result, bool &found, bool* selected) {
    if (found || step == 4) return;

    int len = src.size();
    for (int i = 0; i < len; i++) {
        if (!selected[i] && count[src[i].first.first] > 0 && count[src[i].first.second] > 0) {
            result[step] = i;
            selected[i] = true;
            count[src[i].first.first]--;
            count[src[i].first.second]--;
            if (step == 3 && count[0] == 0 && count[1] == 0 && count[2] == 0 && count[3] == 0) {
                found = true;
                return;
            }
            find_matches(src, count, step + 1, result, found, selected);
            selected[i] = false;
            count[src[i].first.first]++;
            count[src[i].first.second]++;
        }
        if (found) break;
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    NameCard card;
    card.load(argv[1]);
    auto contours = card.find_contours();
    
    std::vector<std::tuple<double, double, double>> lines;
    int hori = 2, vert = 2;
    for (int i = 0; i < 4; i++) {
        auto line = card.ransac(contours, 4000);
        auto k = std::abs(Utils::calc_kvalue(line));
        // 横线
        if (k < 1) {
            if (hori <= 0) {
                i--;
                continue;
            }
            hori--;
        }
        // 竖线
        else {
            if (vert <= 0) {
                i--;
                continue;
            }
            vert--;
        }
        lines.push_back(line);
    }

    printf("lines:\n");

    for (auto& i : lines) {
        printf("(%lf)x + (%lf)y + (%lf) = 0\n", std::get<0>(i), std::get<1>(i), std::get<2>(i));
    }

    // (两条直线的编号, 两条直线的交点)[]
    auto cpoints = std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>();

    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            cpoints.push_back(std::make_pair(std::make_pair(i, j), Utils::calc_cpoint(lines[i], lines[j])));
        }
    }

    int cx = card.get().width() / 2;
    int cy = card.get().height() / 2;

    // 按照交点距离图像中心的距离从近到远排序
    sort(cpoints.begin(), cpoints.end(), [cx, cy](std::pair<std::pair<int, int>, std::pair<int, int>>& a, std::pair<std::pair<int, int>, std::pair<int, int>>& b) -> bool
        {
            double d1 = std::pow(a.second.first - cx, 2) + std::pow(a.second.second - cy, 2);
            double d2 = std::pow(b.second.first - cx, 2) + std::pow(b.second.second - cy, 2);

            return d1 < d2;
        });

    // 每条直线应当恰好被选中两次
    int count[] = { 2,2,2,2 };
    int result[4] = { 0,0,0,0 };
    bool selected[] = { false,false,false,false,false,false };
    bool found = false;
    find_matches(cpoints, count, 0, result, found, selected);

    vector<std::pair<int, int>> sorted_corners;

    printf("corner points:\n");
    int maxx = 0, minn = 2147483647;
    for (int i = 0; i < 4; i++) {
        sorted_corners.push_back(cpoints[result[i]].second);
    }

    sort(sorted_corners.begin(), sorted_corners.end(), [](std::pair<int, int> &a, std::pair<int, int> &b) -> bool {
        return a.first * a.second < b.first * b.second;
    });

    for (auto& c : sorted_corners) {
        printf("(%d, %d)\n", c.first, c.second);
    }

    auto transformed = card.perspective_transform(
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
