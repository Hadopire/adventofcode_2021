#include <iostream>
#include <charconv>
#include <vector>
#include <cmath>
#include <algorithm>

#include "utils.h"

int main() {
    size_t bufSize;
    const char *buf = mapFile("d7.txt", bufSize);

    std::vector<int> positions;
    positions.reserve(10000);
    float mean = 0.f;
    int first = 0;
    int second = 0;
    int target;
    
    const char *str = buf;
    const char *last = buf + bufSize;
    while (str < last) {
        int n;
        auto result = std::from_chars(str, last, n);
        str = result.ptr + 1;

        positions.push_back(n);
        mean += n;
    }

    std::sort(positions.begin(), positions.end());
    size_t size = positions.size();
    target = positions[size / 2];
    if (size % 2 == 0) {
        target = std::round((positions[(size - 1) / 2] + target) / 2.0);
    }
    for (int i = 0; i < positions.size(); ++i) {
        first += std::abs(positions[i] - target);
    }

    target = std::floor(mean / size);
    for (int i = 0; i < positions.size(); ++i) {
        int step = std::abs(positions[i] - target);
        second += (step*(step+1)) / 2;
    }

    std::cout << "Premier gaulois: " << first <<  std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}