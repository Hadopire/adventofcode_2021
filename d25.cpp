#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>

int main() {
    std::filesystem::path path = "d25.txt";
    size_t bufSize = std::filesystem::file_size(path);
    std::ifstream file(path);
    
    std::vector<char> buf(bufSize);
    file.read(buf.data(), bufSize);

    int bufWidth = 1;
    for (int i = 0; !isspace(buf[i]); ++i) {
        ++bufWidth;
    }
    int width = bufWidth - 1;
    int height = (bufSize + 1) / bufWidth;

    bool moved = true;
    int nmove = 0;
    while (moved) {
        moved = false;
        for (int y = 0; y < height; ++y) {
            int end = buf[y * bufWidth] == '.' ? width : width - 1;
            for (int x = 0; x < end; ++x) {
                if (buf[x + y * bufWidth] == '>' && buf[(x+1) % width + y * bufWidth] == '.') {
                    std::swap(buf[x + y * bufWidth], buf[(x+1) % width + y * bufWidth]);
                    moved = true;
                    ++x;
                }
            }
        }

        for (int x = 0; x < width; ++x) {
            int end = buf[x] == '.' ? height : height - 1;
            for (int y = 0; y < end; ++y) {
                if (buf[x + y * bufWidth] == 'v' && buf[x + ((y+1) % height) * bufWidth] == '.') {
                    std::swap(buf[x + y * bufWidth], buf[x + ((y+1) % height) * bufWidth]);
                    moved = true;
                    ++y;
                }
            }
        }

        ++nmove;
    }

    std::cout << "Premier gaulois: " << nmove << std::endl;

    return 0;
}