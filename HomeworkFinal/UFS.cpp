#include "UFS.h"

UFS::UFS(int size) {
    father.clear();
    for (int i = 0; i < size; i++) {
        father.push_back(i);
    }
}

int UFS::find(int x) {
    if (father[x] == x) return x;
    return father[x] = find(father[x]);
}

void UFS::merge(int a, int b) {
    int fa = find(a), fb = find(b);
    if (fa != fb) father[fa] = fb;
}

bool UFS::in_same_group(int a, int b) {
    return find(a) == find(b);
}
