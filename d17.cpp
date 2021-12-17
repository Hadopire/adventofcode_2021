#include <iostream>
#include <charconv>

#include "utils.h"

bool hit(int vx, int vy, int minx, int maxx, int miny, int maxy) {
    int px = 0;
    int py = 0;
    while (px <= maxx && py >= miny) {
        px += vx;
        py += vy;

        if (px >= minx && px <= maxx && py >= miny && py <= maxy) {
            return true;
        }

        if (vx) --vx;
        --vy;
    }
    return false;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d17.txt", bufSize);
    const char *endbuf = buf + bufSize;

    int minx, maxx, miny, maxy;

    auto result = std::from_chars(buf + 15, endbuf, minx);
    result = std::from_chars(result.ptr + 2, endbuf, maxx);
    result = std::from_chars(result.ptr + 4, endbuf, miny);
    std::from_chars(result.ptr + 2, endbuf, maxy);

    // this assumes that there is a triangular number x such that minx <= x <= maxx
    int maxvy = -miny - 1;
    int highest = maxvy*maxvy - (maxvy * (maxvy-1)) / 2;

    int minvy = miny;
    int maxvx = maxx;
    int minvx;

    for (int i = 1; true; ++i) {
        int vx = i*(i+1) / 2;
        if (vx >= minx) {
            minvx = i;
            break;
        }
    }

    int nhits = 0;
    for (int y = minvy; y <= maxvy; ++y) {
        for (int x = minvx; x <= maxvx; ++x) {
            if (hit(x, y, minx, maxx, miny, maxy)) {
                ++nhits;
            }
        }
    }

    std::cout << "Premier gaulois: " << highest << std::endl;
    std::cout << "Second gaulois: " << nhits << std::endl;

    return 0;
}