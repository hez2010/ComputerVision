#include <svm.h>
#include <CImg.h>
#include <cstdio>
#include "DataReader.h"

using namespace cimg_library;

void predict(char* imageFile, char* modelFile) {
    CImg<u8> image = CImg<u8>(imageFile);
    constexpr int cols = 28, rows = 28;
    constexpr int size = cols * rows;
    image.resize(rows, cols, 1, 1);
    svm_model* model = svm_load_model(modelFile);
    svm_node* node = new svm_node[size / 4 + 1];

    for (int i = 0; i < rows / 2; i++) {
        for (int j = 0; j < cols / 2; j++) {
            node[i * rows / 2 + j].index = i * rows / 2 + j + 1;
            node[i * rows / 2 + j].value =
                (255 - *image.data(i * 2, j * 2) +
                    255 - *image.data(i * 2, j * 2 + 1) +
                    255 - *image.data(i * 2 + 1, j * 2) +
                    255 - *image.data(i * 2 + 1, j * 2 + 1)) / 4
                >= 100 ? 1 : 0;
        }
    }
    node[size / 4].index = -1;
    u8 result = (u8)svm_predict(model, node);
    printf("Predict result = %d\n", (int)result);
}

void test(char* labelFile, char* dataFile, char* modelFile, svm_parameter& param) {
    DataReader reader(labelFile, dataFile);
    int cols = reader.get_cols(), rows = reader.get_rows();
    int size = cols * rows;

    svm_model* model = svm_load_model(modelFile);
    svm_node* node = new svm_node[size / 4 + 1];

    CImg<u8> image = {};
    u8 label = 0;

    int count = 0;
    int correct = 0;
    while (reader.move_next(&label, &image)) {
        count++;
        for (int i = 0; i < rows / 2; i++) {
            for (int j = 0; j < cols / 2; j++) {
                node[i * rows / 2 + j].index = i * rows / 2 + j + 1;
                node[i * rows / 2 + j].value =
                    (*image.data(i * 2, j * 2) +
                        *image.data(i * 2, j * 2 + 1) +
                        *image.data(i * 2 + 1, j * 2) +
                        *image.data(i * 2 + 1, j * 2 + 1)) / 4
                    >= 100 ? 1 : 0;
            }
        }
        node[size / 4].index = -1;

        u8 result = (u8)svm_predict(model, node);
        printf("Test case #%d, expected = %d, actual = %d\n", count, (int)label, (int)result);
        if (label == result) {
            correct++;
        }
    }
    printf("Test result = %d/%d, accuracy = %lf%%\n", correct, count, correct * 100.0 / count);
}

void train(char* labelFile, char* dataFile, char* modelFile, svm_parameter& param) {
    DataReader reader(labelFile, dataFile);
    int cols = reader.get_cols(), rows = reader.get_rows();
    int size = cols * rows;

    CImg<u8> image = {};
    u8 label = 0;

    svm_problem prob = {};
    prob.l = reader.get_size();
    prob.x = new svm_node * [prob.l];
    prob.y = new double[prob.l];

    int index = 0;
    while (reader.move_next(&label, &image)) {
        prob.x[index] = new svm_node[size / 4 + 1];
        for (int i = 0; i < rows / 2; i++) {
            for (int j = 0; j < cols / 2; j++) {
                prob.x[index][i * rows / 2 + j].index = i * rows / 2 + j + 1;
                prob.x[index][i * rows / 2 + j].value =
                    (*image.data(i * 2, j * 2) +
                        *image.data(i * 2, j * 2 + 1) +
                        *image.data(i * 2 + 1, j * 2) +
                        *image.data(i * 2 + 1, j * 2 + 1)) / 4
                    >= 100 ? 1 : 0;
            }
        }
        prob.x[index][size / 4].index = -1;
        prob.y[index] = label;
        index++;
    }

    svm_model* model = svm_train(&prob, &param);
    svm_save_model(modelFile, model);

    for (int i = 0; i < prob.l; i++) {
        delete[] prob.x[i];
    }
    delete[] prob.x;
    delete[] prob.y;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("hw5.exe train [label_file] [data_file] [model_file]\n");
        printf("hw5.exe test [label_file] [data_file] [model_file]\n");
        printf("hw5.exe predict [image_file] [model_file]\n");
        printf("Example: hw5.exe train datasets/train-labels.idx1-ubyte datasets/train-images.idx3-ubyte model.data\n");
        return 0;
    }

    svm_parameter param = {};
    param.C = 100;
    param.gamma = 0.09;
    param.coef0 = 1;
    param.nu = 0.5;
    param.p = 1;
    param.degree = 10;
    param.eps = 1e-3;
    param.svm_type = C_SVC;
    param.kernel_type = RBF;
    param.cache_size = 512;

    switch (argv[1][2]) {
    case 's':
    case 'S':
        test(*(argv + 2), *(argv + 3), *(argv + 4), param);
        break;
    case 'a':
    case 'A':
        train(*(argv + 2), *(argv + 3), *(argv + 4), param);
        break;
    case 'e':
    case 'E':
        predict(*(argv + 2), *(argv + 3));
        break;
    }
    return 0;
}
