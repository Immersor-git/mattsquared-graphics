#include "MatrixUtils.h"
#include <iostream>
#include <iomanip>

void printMat4(const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << std::setw(15) << mat[i][j] << ' ';
        }
        std::cout << '\n';
    }
}