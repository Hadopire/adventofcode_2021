#include <iostream>
#include <vector>

#include "utils.h"

struct ivec2 {
    int x, y;

    ivec2 operator-(const ivec2 &rhs) const {
        return {x - rhs.x, y - rhs.y};
    }

    ivec2 operator+(const ivec2 &rhs) const {
        return {x + rhs.x, y + rhs.y};
    }
};

struct Pixels {
    std::vector<bool> pixels;
    ivec2 start;
    ivec2 end;
    ivec2 size;

    Pixels(size_t size) {
        pixels.resize(size);
    }

    void resize(ivec2 _start, ivec2 _end) {
        start = _start;
        end = _end;
        size = {end.x - start.x + 1, end.y - start.y + 1};
        if (pixels.size() < size.x * size.y) {
            std::cout << "resize wtf" << std::endl;
            pixels.resize(size.x * size.y * 2);
        }
    }

    bool at(ivec2 pos, bool odd, bool firstRule) {
        if (pos.x < start.x || pos.x > end.x || pos.y < start.y || pos.y > end.y) {
            if (odd && firstRule) return true;
            return false;
        }

        pos.x -= start.x;
        pos.y -= start.y;
        return pixels[pos.x + pos.y * size.x];
    }

    void set(ivec2 pos, bool value) {
        pos.x -= start.x;
        pos.y -= start.y;
        pixels[pos.x + pos.y * size.x] = value;
    }
};

int get9BitValue(Pixels &pixels, ivec2 pos, bool odd, bool firstRule) {
    int shift = 8;
    int result = 0;
    for (int y = pos.y - 1; y <= pos.y + 1; ++y) {
        for (int x = pos.x - 1; x <= pos.x + 1; ++x) {
            result |= (pixels.at({x, y}, odd, firstRule) << shift);
            --shift;
        }
    }

    return result;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d20.txt", bufSize);

    bool rules[512];
    Pixels pixels(100000);
    Pixels pixelsNext(100000);

    const char *str = buf;
    const char *strend = buf + bufSize;
    for (int i = 0; i < 512; ++i, ++str) {
        rules[i] = *str == '#';
    }
    str += 2;

    int width = 2;
    while (str[width - 2] != '\n') {
        ++width;
    }

    ivec2 end = {0, 0};
    ivec2 begin = {-1, -1};
    while (str < strend) {
        end.x = 0;
        while (str < strend && *str != '\n') {
            pixels.pixels[(end.x + 1) + (end.y + 1) * width] = *str == '#';
            ++end.x;
            ++str;
        }
        ++end.y;
        ++str;
    }

    pixels.resize(begin, end);

    size_t first, second;
    for (int i = 0; i < 50; ++i) {
        second = 0;
        pixelsNext.resize(pixels.start - ivec2{1, 1}, pixels.end + ivec2{1, 1});

        bool odd = (i % 2 == 1);
        for (int y = pixelsNext.start.y; y <= pixelsNext.end.y; ++y) {
            for (int x = pixelsNext.start.x; x <= pixelsNext.end.x; ++x) {
                ivec2 pos{x, y};
                int rule = get9BitValue(pixels, pos, odd, rules[0]);
                pixelsNext.set(pos, rules[rule]);
                second += rules[rule];
            }
        }

        std::swap(pixels.pixels, pixelsNext.pixels);
        std::swap(pixels.start, pixelsNext.start);
        std::swap(pixels.end, pixelsNext.end);
        std::swap(pixels.size, pixelsNext.size);

        if (i == 1) {
            first = second;
        }
    }

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;

    return 0;
}