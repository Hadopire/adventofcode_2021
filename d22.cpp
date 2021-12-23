#include <iostream>
#include <vector>
#include <charconv>
#include <list>
#include <algorithm>

#include "utils.h"

struct ivec3 {
    int x, y, z;
};

struct Cube {
    ivec3 start;
    ivec3 end;
};

struct Step {
    bool on;
    Cube cube;
};

int main() {
    size_t bufSize;
    const char *buf = mapFile("d22.txt", bufSize);

    std::vector<Step> steps;
    std::list<Cube> cubes;
    steps.reserve(500);

    const char *str = buf;
    const char *strend = buf + bufSize;
    while (str < strend) {
        Step &step = steps.emplace_back();
        while (*str != ' ') ++str;
        step.on = str[-1] == 'f' ? false : true;
        auto result = std::from_chars(str + 3, strend, step.cube.start.x);
        result = std::from_chars(result.ptr + 2, strend, step.cube.end.x);
        result = std::from_chars(result.ptr + 3, strend, step.cube.start.y);
        result = std::from_chars(result.ptr + 2, strend, step.cube.end.y);
        result = std::from_chars(result.ptr + 3, strend, step.cube.start.z);
        result = std::from_chars(result.ptr + 2, strend, step.cube.end.z);
        str = result.ptr + 1;
    }

    for (auto &step : steps) {
        Cube &newCube = step.cube;
        for (auto it = cubes.begin(); it != cubes.end();) {
            Cube &cube = *it;
            if (cube.end.x < newCube.start.x || cube.start.x > newCube.end.x || cube.end.z < newCube.start.z || cube.start.z > newCube.end.z || cube.end.y < newCube.start.y || cube.start.y > newCube.end.y) {
                ++it;
                continue;
            }

            if (cube.start.x < newCube.start.x) {
                Cube &subCube = cubes.emplace_back(cube);
                subCube.end.x = newCube.start.x - 1;
            }
            if (cube.end.x > newCube.end.x) {
                Cube &subCube = cubes.emplace_back(cube);
                subCube.start.x = newCube.end.x + 1;
            }
            if (cube.start.y < newCube.start.y) {
                Cube &subCube = cubes.emplace_back(cube);
                subCube.start.x = std::max(cube.start.x, newCube.start.x);
                subCube.end.x = std::min(cube.end.x, newCube.end.x);
                subCube.end.y = newCube.start.y - 1;
            }
            if (cube.end.y > newCube.end.y) {
                Cube &subCube = cubes.emplace_back(cube);
                subCube.start.x = std::max(cube.start.x, newCube.start.x);
                subCube.end.x = std::min(cube.end.x, newCube.end.x);
                subCube.start.y = newCube.end.y + 1;
            }
            if (cube.start.z < newCube.start.z) {
                Cube &subCube = cubes.emplace_back();
                subCube.start.x = std::max(cube.start.x, newCube.start.x);
                subCube.end.x = std::min(cube.end.x, newCube.end.x);
                subCube.start.y = std::max(cube.start.y, newCube.start.y);
                subCube.end.y = std::min(cube.end.y, newCube.end.y);
                subCube.start.z = cube.start.z;
                subCube.end.z = newCube.start.z - 1;
            }
            if (cube.end.z > newCube.end.z) {
                Cube &subCube = cubes.emplace_back();
                subCube.start.x = std::max(cube.start.x, newCube.start.x);
                subCube.end.x = std::min(cube.end.x, newCube.end.x);
                subCube.start.y = std::max(cube.start.y, newCube.start.y);
                subCube.end.y = std::min(cube.end.y, newCube.end.y);
                subCube.start.z = newCube.end.z + 1;
                subCube.end.z = cube.end.z;
            }

            it = cubes.erase(it);
        }

        if (step.on) {
            cubes.push_back(newCube);
        }
    }

    size_t count0 = 0;
    size_t count1 = 0;
    for (auto &cube : cubes) {
        if (!(cube.end.x < -50 || cube.start.x > 50 || cube.end.z < -50 || cube.start.z > 50 || cube.end.y < -50 || cube.start.y > 50)) {
            Cube clamped;
            clamped.start.x = std::clamp(cube.start.x, -50, 50);
            clamped.end.x = std::clamp(cube.end.x, -50, 50);
            clamped.start.y = std::clamp(cube.start.y, -50, 50);
            clamped.end.y = std::clamp(cube.end.y, -50, 50);
            clamped.start.z = std::clamp(cube.start.z, -50, 50);
            clamped.end.z = std::clamp(cube.end.z, -50, 50);
            count0 += (long long)(clamped.end.x - clamped.start.x + 1) * (clamped.end.y - clamped.start.y + 1) * (clamped.end.z - clamped.start.z + 1);
        }
        count1 += (long long)(cube.end.x - cube.start.x + 1) * (cube.end.y - cube.start.y + 1) * (cube.end.z - cube.start.z + 1);
    }

    std::cout << "Premier gaulois: " << count0 << std::endl;
    std::cout << "Second gaulois: " << count1 << std::endl;

    return 0;
}