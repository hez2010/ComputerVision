#include "DataReader.h"

int DataReader::reverse_int(int i) {
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}


DataReader::DataReader(const char* labelFileName, const char* dataFileName) {
    label = new std::ifstream(labelFileName, std::ios::binary);
    data = new std::ifstream(dataFileName, std::ios::binary);

    int label_magic_number = 0;
    int label_number_of_images = 0;
    if (label->is_open()) {
        label->read((char*)&label_magic_number, sizeof(label_magic_number));
        label_magic_number = reverse_int(label_magic_number);
        label->read((char*)&label_number_of_images, sizeof(label_number_of_images));
        label_number_of_images = reverse_int(label_number_of_images);
    }

    int data_magic_number = 0;
    int data_number_of_images = 0;
    int data_n_rows = 0;
    int data_n_cols = 0;
    if (data->is_open()) {
        data->read((char*)&data_magic_number, sizeof(data_magic_number));
        data_magic_number = reverse_int(data_magic_number);
        data->read((char*)&data_number_of_images, sizeof(data_number_of_images));
        data_number_of_images = reverse_int(data_number_of_images);
        data->read((char*)&data_n_rows, sizeof(data_n_rows));
        data_n_rows = reverse_int(data_n_rows);
        data->read((char*)&data_n_cols, sizeof(data_n_cols));
        data_n_cols = reverse_int(data_n_cols);
    }

    if (data_magic_number != DATA_MAGIC ||
        label_magic_number != LABEL_MAGIC ||
        label_number_of_images != data_number_of_images) {
        label->close();
        data->close();
        label = nullptr;
        data = nullptr;
        throw std::exception("Mismatch data and label");
    }

    count = data_number_of_images;
    current = 0;
    rows = data_n_rows;
    cols = data_n_cols;
}

bool DataReader::move_next(u8* labelOut, CImg<u8>* dataOut) {
    if (data == nullptr || label == nullptr || !data->is_open() || !label->is_open()) {
        return false;
    }
    if (current >= count) {
        return false;
    }
    u8* buffer = new u8[rows * cols];
    label->read((char*)labelOut, 1);
    dataOut->clear();
    dataOut->resize(rows, cols, 1, 1);
    data->read((char*)buffer, rows * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            *(dataOut->data(j, i)) = buffer[i * rows + j];
        }
    }
    delete[] buffer;
    current++;
    return true;
}

int DataReader::get_rows() {
    return rows;
}

int DataReader::get_cols() {
    return cols;
}

int DataReader::get_size() {
    return count;
}
