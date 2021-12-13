#include <algorithm>
#include <charconv>
#include <iostream>
#include <vector>

#include "utils.h"

struct ivec2 {
    int x, y;
};

struct fold {
    char along;
    int at;
};

int main() {
    size_t bufSize;
    const char *buf = mapFile("d13.txt", bufSize);

    const char *str = buf;
    const char *strend = str + bufSize;

    ivec2 inputs[1000];
    int ninputs = 0;
    ivec2 size0 = {0, 0};

    for (; *str != '\n'; ++ninputs) {
        auto result = std::from_chars(str, strend, inputs[ninputs].x);
        result = std::from_chars(result.ptr + 1, strend, inputs[ninputs].y);

        size0.x = std::max(size0.x, inputs[ninputs].x + 1);
        size0.y = std::max(size0.y, inputs[ninputs].y + 1);

        str = result.ptr + 1;
    }

    ivec2 size1;
    fold folds[20];
    int nfolds = 0;
    for (; ++str < strend; ++nfolds) {
        int at;
        auto result = std::from_chars(str + 13, strend, at);

        folds[nfolds].at = at;
        folds[nfolds].along = str[11];

        if (folds[nfolds].along == 'x') {
            size1.x = at;
        } else {
            size1.y = at;
        }

        str = result.ptr;
    }

    if (folds[0].along == 'x') {
        size0.x = folds[0].at;
    } else {
        size0.y = folds[0].at;
    }

    int numdots = 0;
    std::vector<bool> dots0(size0.x * size0.y, false);
    std::vector<bool> dots1(size1.x * size1.y, false);
    for (int i = 0; i < ninputs; ++i) {
        ivec2 &input = inputs[i];

        {
            int &coord = folds[0].along == 'x' ? input.x : input.y;
            if (coord > folds[0].at) {
                coord = folds[0].at - (coord - folds[0].at);
            }

            auto dot = dots0.at(input.x + input.y * size0.x);
            if (!dot) ++numdots;
            dot = true;
        }

        for (int j = 1; j < nfolds; ++j) {
            int &coord = folds[j].along == 'x' ? input.x : input.y;
            if (coord > folds[j].at) {
                coord = folds[j].at - (coord - folds[j].at);
            }
        }

        dots1[input.x + input.y * size1.x] = true;
    }

    std::cout << "Premier gaulois: " << numdots << std::endl;
    std::cout << "Second gaulois:" << std::endl;

    for (int y = 0; y < size1.y; ++y) {
        for (int x = 0; x < size1.x; ++x) {
            if (dots1[x + y * size1.x]) {
                std::cout << "#";
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}