#include "Homework2.h"

int main(int argc, char** argv)
{
    if (argc < 5) {
        printf("Usage: canny filename core_size threshold_low threshold_high\nExample: canny 1.bmp 3 20 40\n");
        return 0;
    }

    Canny canny;
    canny.detect(*(argv + 1), atoi(*(argv + 2)), atoi(*(argv + 3)), atoi(*(argv + 4)));
    return 0;
}
