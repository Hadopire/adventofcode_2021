#include <chrono>
#include <iostream>

#include "utils.h"

struct ivec2 {
    int x, y;
};

void inc(int map[10][10], int x, int y, ivec2 *flashed, int &numflashed) {
    ++map[y][x];
    if (map[y][x] == 10) {
        flashed[numflashed] = ivec2{ x, y };
        ++numflashed;
    
        if (x) inc(map, x - 1, y, flashed, numflashed);
        if (y) inc(map, x, y - 1, flashed, numflashed);
        if (x && y) inc(map, x - 1, y - 1, flashed, numflashed);
        if (x < 9 && y) inc(map, x + 1, y - 1, flashed, numflashed);
        if (x < 9) inc(map, x + 1, y, flashed, numflashed);
        if (y < 9 && x < 9) inc(map, x + 1, y + 1, flashed, numflashed);
        if (y < 9) inc(map, x, y + 1, flashed, numflashed);
        if (y < 9 && x) inc(map, x - 1, y + 1, flashed, numflashed);

    }
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d11.txt", bufSize);

    int map[10][10];
    for (int y = 0; y < 10; ++y, ++buf) {
        for (int x = 0; x < 10; ++x, ++buf) {
            map[y][x] = *buf - '0';
        }
    }

    ivec2 flashed[100];
    int sum = 0;
    int step = 0;
    while (true) {
        int numflashed = 0;
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                inc(map, x, y, flashed, numflashed);
            }
        }

        ++step;
        if (step <= 100 ) sum += numflashed;
        if (numflashed == 100) break;
        for (int i = 0; i < numflashed; ++i) {
            ivec2 pos = flashed[i];
            map[pos.y][pos.x] = 0;
        }

    }

    std::cout << "Premier gaulois: " << sum << std::endl;
    std::cout << "Second gaulois: " << step << std::endl;

    return 0;
}