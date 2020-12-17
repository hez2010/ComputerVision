#pragma once

constexpr int LABEL_MAGIC = 2049;
constexpr int DATA_MAGIC = 2051;

#include <cstdio>
#include <iostream>
#include <fstream>
#include <CImg.h>

using namespace cimg_library;

typedef unsigned char u8;

class DataReader {
private:
    std::ifstream* label = nullptr, * data = nullptr;
    int count = 0, current = 0, rows = 0, cols = 0;

    int reverse_int(int i);

public:
    DataReader(const char* labelFileName, const char* dataFileName);
    bool move_next(u8* labelOut, CImg<u8>* dataOut);
    int get_rows();
    int get_cols();
    int get_size();
};