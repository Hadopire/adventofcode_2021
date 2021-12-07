#include <iostream>
#include <charconv>

#include "utils.h"

int main() {
    size_t bufSize;
    const char *buf = mapFile("d6.txt", bufSize);

    size_t fishes[9] = {0};
    for (size_t i = 0; i < bufSize; i += 2) {
        ++fishes[buf[i] - '0'];
    }

    size_t first = 0;
    size_t second = 0;
    for (int day = 0; day < 256; ++day) {
        fishes[(day+7)%9] += fishes[day%9];

        if (day == 80) {
            for (int i = 0; i < 9; ++i) {
                first += fishes[i];
            }
        }
    }

    for (int i = 0; i < 9; ++i) {
        second += fishes[i];
    }

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}