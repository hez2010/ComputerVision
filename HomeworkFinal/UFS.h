#pragma once
#include "Shared.h"

class UFS {
private:
    vector<int> father = {};
public:
    UFS(int size);

    int find(int x);
    void merge(int a, int b);
    bool in_same_group(int a, int b);
};