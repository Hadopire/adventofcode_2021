#include <iostream>
#include <vector>

#include "utils.h"

#define NUMSTEP 40

constexpr int pairFirst(int x) { return (x >> 5); }
constexpr int pairSecond(int x) { return (x & 0x1F); }
constexpr int makePair(int a, int b) { return (a << 5) | b; }
constexpr int makePairFromChars(char a, char b) { return ((a - 'A') << 5) | (b - 'A'); }

size_t getAnswer(std::vector<int> &elems, std::vector<int> &pairs, size_t *npairs) {
    size_t counts[26] = { 0 };
    counts[elems[elems.size() - 1]] += 1;

    for (int pair : pairs) {
        if (npairs[pair]) {
            counts[pairFirst(pair)] += npairs[pair];
        }
    }

    size_t min = -1;
    size_t max = 0;
    for (int i = 0; i < 26; ++i) {
        if (counts[i] == 0) continue;
        min = std::min(min, counts[i]);
        max = std::max(max, counts[i]);
    }
    return max - min;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d14.txt", bufSize);

    std::vector<int> elems;
    std::vector<int> pairs;
    size_t count[26] = { 0 };
    int rules[1024];
    size_t first, second;

    const char *str = buf;
    const char *strend = buf + bufSize;
    while (*str != '\n') {
        int elem = *str - 'A';
        ++count[elem];
        elems.push_back(*str - 'A');
        ++str;
    }

    str += 2;
    while (str < strend) {
        int pair = makePairFromChars(str[0], str[1]);
        pairs.push_back(pair);
        rules[pair] = str[6] - 'A';
        str += 8;
    }

    size_t *npairs = new size_t[1024 * (NUMSTEP + 1)] { 0 };
    for (int i = 1; i < elems.size(); ++i) {
        int pair = makePair(elems[i - 1], elems[i]);
        ++npairs[pair];
    }

    size_t *step = npairs;
    for (int i = 1; i <= NUMSTEP; ++i) {
        size_t *nextstep = step + 1024;
        for (int pair : pairs) {
            size_t npair = step[pair];
            if (!npair) continue;

            int a = pairFirst(pair);
            int b = pairSecond(pair);
            int c = rules[pair];
            nextstep[makePair(a, c)] += npair;
            nextstep[makePair(c, b)] += npair;
        }
        step = nextstep;

        if (i == 10) {
            first = getAnswer(elems, pairs, step);
        }
    }
    second = getAnswer(elems, pairs, step);
    
    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}