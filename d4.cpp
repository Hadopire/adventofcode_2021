#include <vector>
#include <string_view>
#include <charconv>
#include <iostream>
#include <unordered_map>

#include "utils.h"

// trying out weird stuff with c++17 and mmap for fast parsing

struct Cell {
    int line, column;
};

struct Grid {
    bool won = false;
    int sum = 0;
    int lines[5] = { 0 }; 
    int columns[5] = { 0 };
    std::unordered_map<int, Cell> cells;
};

int main() {
    std::vector<int> draw;
    std::vector<Grid> grids;

    size_t bufSize;
    const char *buf = mapFile("d4.txt", bufSize);
    std::string_view str(buf, bufSize);
    while (!std::isspace(str[0])) {
        int n;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), n);
        str = ptr + 1;
        draw.push_back(n);
    }

    while (std::isspace(str[0])) {
        str = str.begin() + 1;
    }

    while (str[0]) {
        grids.push_back({});
        Grid &grid = grids.back();
        grid.cells.reserve(50);
        for (int line = 0; line < 5; ++line) {
            for (int column = 0; column < 5; ++column) {
                int n;
                auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), n);
                while (std::isspace(*ptr)) {
                    ++ptr;
                }
                str = ptr;
                grid.sum += n;
                grid.cells[n] = { line, column };
            }
        }
    }

    int first = 0;
    int last = 0;
    int nWinner = 0;
    for (int i = 0; i < draw.size() && nWinner != grids.size(); ++i) {
        int n = draw[i];
        for (auto &grid : grids) {
            auto it = grid.cells.find(n);
            if (grid.won || it == grid.cells.end()) {
                continue;
            }

            grid.sum -= n;
            Cell &cell = it->second;
            if (++grid.lines[cell.line] == 5 || ++grid.columns[cell.column] == 5) {
                grid.won = true;
                last = n * grid.sum;
                if (!first) {
                    first = last;
                }
            }
            
        }
    }

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << last << std::endl;
    return 0;
}