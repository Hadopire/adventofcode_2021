#include <iostream>
#include <vector>
#include <queue>

#include "utils.h"

struct Map {
    const char *data;
    size_t width, height;

    inline char get(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return '9';
        }

        return data[x + ((width+1) * y)];
    }
};

struct ivec2 {
    int x, y;
};

int bfs(Map &map, ivec2 start, bool *visited) {
    std::queue<ivec2> queue;
    queue.push(start);
    visited[start.x + start.y * map.width] = true;

    static ivec2 directions[4] = {
        { -1, 0 },
        { 1, 0 },
        { 0, -1 },
        { 0, 1 },
    };
    int ret = 1;
    while (queue.size()) {
        ivec2 position = queue.front();
        queue.pop();
        for (auto &dir : directions) {
            ivec2 p = { position.x + dir.x, position.y + dir.y };
            char cell = map.get(p.x, p.y);
            if (cell != '9' && !visited[p.x + p.y * map.width]) {
                queue.push(p);
                visited[p.x + p.y * map.width] = true;
                ++ret;
            }
        }
    }
    return ret;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d9.txt", bufSize);

    size_t width = 0;
    while (buf[width] != '\n') ++width;
    size_t height = bufSize / width;
    
    Map map { buf, width, height };
    std::vector<ivec2> lowPoints;
    lowPoints.reserve(1000);
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            char current = map.get(x, y);
            char top = map.get(x, y-1);
            char bot = map.get(x, y+1);
            char left = map.get(x-1, y);
            char right = map.get(x+1, y);

            if (current < top && current < bot && current < left && current < right) {
                lowPoints.push_back({x, y});
            }
        }
    }

    int sum0 = 0;
    bool *visited = new bool[width*height] { false };
    int max0, max1, max2;
    max0 = max1 = max2 = 0;
    for (auto point : lowPoints) {
        sum0 += map.get(point.x, point.y) - '0' + 1;
        int size = bfs(map, point, visited);
        if (max0 < size) {
            max2 = max1;
            max1 = max0;
            max0 = size;
        } else if (max1 < size) {
            max2 = max1;
            max1 = size;
        } else if (max2 < size) {
            max2 = size;
        }
    }
    
    std::cout << "Premier gaulois: " << sum0 << std::endl;
    std::cout << "Second gaulois: " << max0 * max1 * max2 << std::endl;
    return 0;
}