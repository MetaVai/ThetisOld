#include "Matrix.h"

extern "C" {
    int matrixFn0(const char *x) {
        std::cerr << "matrixFn0: " << x << std::endl;
        return 314;
    }
}