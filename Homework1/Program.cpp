#include "Homework1.h"

int main(int argc, char** argv)
{
    bool use_lib = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(*(argv + i), "--use-lib") == 0) {
            use_lib = true;
            break;
        }
    }

    Homework1 hw1("1.bmp");

    hw1.draw_triangle(use_lib);
    hw1.display();
    
    hw1.draw_circle(use_lib);
    hw1.display();
    
    hw1.draw_line(use_lib);
    hw1.display();
    
    hw1.rotate(15, use_lib);
    hw1.display();
    
    hw1.rotate(40, use_lib);
    hw1.display();
    
    hw1.save("2.bmp");

    return 0;
}
