#include <fstream>
#include <iostream>
#include <charconv>
#include <string_view>

#include "utils.h"

struct ivec2 {
    int x, y;

    inline ivec2 operator-(ivec2 &rhs) {
        return ivec2 { x - rhs.x, y - rhs.y };
    }

    inline ivec2 operator+(ivec2 &rhs) {
        return ivec2 { x + rhs.x, y + rhs.y };
    }

    inline bool operator!=(ivec2 &rhs) {
        return x != rhs.x || y != rhs.y;
    }
};

int main() {
    int grid0[1000][1000] = {0};
    int grid1[1000][1000] = {0};
    int sum0 = 0;
    int sum1 = 0;

    size_t bufSize;
    const char *buf = mapFile("d5.txt", bufSize);
    std::string_view str(buf, bufSize);
    while (str[0]) {
        ivec2 a, b;
        auto result = std::from_chars(str.data(), str.data() + str.size(), a.x);
        result = std::from_chars(result.ptr + 1, str.data() + str.size(), a.y);
        result = std::from_chars(result.ptr + 4, str.data() + str.size(), b.x);
        result = std::from_chars(result.ptr + 1, str.data() + str.size(), b.y);
        while (std::isspace(*result.ptr)) ++result.ptr;
        str = result.ptr;

        bool straight = a.x == b.x || a.y == b.y;
        bool diagonal = std::abs(a.x - b.x) == std::abs(a.y - b.y);
        if (straight || diagonal) {
            ivec2 dir = b - a;
            dir.x = (dir.x >> 31) | (!!dir.x);
            dir.y = (dir.y >> 31) | (!!dir.y);
            a = a - dir;
            while (a != b) {
                a = a + dir;
                sum0 += straight && ++grid0[a.x][a.y] == 2;
                sum1 += ++grid1[a.x][a.y] == 2;
            }
        }
    }

    std::cout << "Premier gaulois: " << sum0 << std::endl;
    std::cout << "Second gaulois: " << sum1 << std::endl;
    return 0;
}