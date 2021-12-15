#include <iostream>
#include <queue>

#include "utils.h"

struct Cell {
    unsigned char risk;
    bool visited;
    unsigned short dist;
};

struct ivec2 {
    int x, y;
};

size_t shortestPath(Cell *grid, int width, int height) {
    auto cmp = [grid, width](const ivec2 &a, const ivec2 &b) {
        return grid[a.x + a.y * width].dist > grid[b.x + b.y * width].dist;
    };
    std::priority_queue<ivec2, std::vector<ivec2>, decltype(cmp)> pqueue(cmp);

    grid[0].dist = 0;
    pqueue.push(ivec2{0, 0});
    while (true) {
        ivec2 pos = pqueue.top();
        Cell *current = &grid[pos.x + pos.y * width];
        pqueue.pop();

        if (pos.x == (width - 1) && pos.y == (height - 1)) break;
        if (current->visited) continue;
        current->visited = true;

        if (pos.x) {
            Cell *n = current - 1;
            int dist = current->dist + n->risk;
            if (n->visited == false && dist < n->dist) {
                n->dist = dist;
                pqueue.push({pos.x - 1, pos.y});
            }
        }

        if (pos.x < (width - 1)) {
            Cell *n = current + 1;
            int dist = current->dist + n->risk;
            if (n->visited == false && dist < n->dist) {
                n->dist = dist;
                pqueue.push({pos.x + 1, pos.y});
            }
        }

        if (pos.y) {
            Cell *n = current - width;
            int dist = current->dist + n->risk;
            if (n->visited == false && dist < n->dist) {
                n->dist = dist;
                pqueue.push({pos.x, pos.y - 1});
            }
        }

        if (pos.y < (height - 1)) {
            Cell *n = current + width;
            int dist = current->dist + n->risk;
            if (n->visited == false && dist < n->dist) {
                n->dist = dist;
                pqueue.push({pos.x, pos.y + 1});
            }
        }
    }

    return grid[(width - 1) + (height - 1) * width].dist;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d15.txt", bufSize);

    const char *str = buf;
    const char *strend = buf + bufSize;

    size_t width = 0;
    while (++width && *str != '\n') ++str;
    size_t height = (bufSize + 1) / width;
    width -= 1;

    Cell *grid0 = new Cell[width * height];
    Cell *grid1 = new Cell[width * height * 5 * 5];

    str = buf;
    int x = 0;
    int y = 0;
    while (str < strend) {
        Cell &cell0 = grid0[x + y * width];
        cell0.risk = *str - '0';
        cell0.visited = false;
        cell0.dist = 0xFFFF;

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                int xx = x + width * j;
                int yy = y + height * i;
                Cell &cell1 = grid1[xx + yy * (width * 5)];
                cell1.risk = cell0.risk + i + j;
                if (cell1.risk > 9)
                    cell1.risk = 1 + (cell1.risk - 1) % 9;
                cell1.visited = false;
                cell1.dist = 0xFFFF;
            }
        }

        ++str;
        ++x;
        if (*str == '\n') {
            ++str;
            x = 0;
            ++y;
        }
    }

    size_t first = shortestPath(grid0, width, height);
    size_t second = shortestPath(grid1, width * 5, height * 5);

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;

    return 0;
}