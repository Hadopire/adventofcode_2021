#include <iostream>
#include <vector>
#include <algorithm>

#include "utils.h"

int main() {
    size_t bufSize;
    const char *buf = mapFile("d10.txt", bufSize);
    
    char charmap[128];
    charmap[')'] = '(';
    charmap['('] = ')';
    charmap['['] = ']';
    charmap[']'] = '[';
    charmap['{'] = '}';
    charmap['}'] = '{';
    charmap['>'] = '<';
    charmap['<'] = '>';

    int scoremap0[128];
    scoremap0[')'] = 3;
    scoremap0[']'] = 57;
    scoremap0['}'] = 1197;
    scoremap0['>'] = 25137;

    int scoremap1[128];
    scoremap1['('] = 1;
    scoremap1['['] = 2;
    scoremap1['{'] = 3;
    scoremap1['<'] = 4;

    bool chunkstart[128];
    chunkstart[')'] = false;
    chunkstart['('] = true;
    chunkstart['['] = true;
    chunkstart[']'] = false;
    chunkstart['{'] = true;
    chunkstart['}'] = false;
    chunkstart['>'] = false;
    chunkstart['<'] = true;

    int sum0 = 0;
    std::vector<size_t> sums1;
    sums1.reserve(100);

    const char *str = buf;
    const char *endbuf = buf + bufSize;
    while (str < endbuf) {
        char stack[255];
        int stackptr = -1;

        while (str < endbuf && *str != '\n') {
            bool ischunkstart = chunkstart[*str];
            if (ischunkstart) {
                ++stackptr;
                stack[stackptr] = *str;
            } else {
                if (charmap[*str] != stack[stackptr]) {
                    sum0 += scoremap0[*str];
                    stackptr = -1;
                    break;
                } else {
                    --stackptr;
                }
            }

            ++str;
        }

        while (str < endbuf && *str != '\n') ++str;
        ++str;

        if (stackptr == -1) continue;
        
        size_t score = 0;
        while (stackptr > -1) {
            score *= 5;
            score += scoremap1[stack[stackptr]];
            --stackptr;
        }

        sums1.push_back(score);
    }

    int n = sums1.size() / 2;
    std::nth_element(sums1.begin(), sums1.begin() + n, sums1.end());

    std::cout << "Premier gaulois: " << sum0 << std::endl;
    std::cout << "Second gaulois: " << sums1[n] << std::endl;

    return 0;
}