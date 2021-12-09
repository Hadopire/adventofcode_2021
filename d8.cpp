#include <bitset>
#include <charconv>
#include <iostream>
#include <string_view>
#include <vector>

#include "utils.h"

struct Entry {
    std::bitset<7> outputs[4] = {0};
    std::bitset<7> digits[10];
    std::bitset<7> seg5digits[3];
};

void parse(const char *str, size_t bufSize, std::vector<Entry> &entries) {
    const char *end = str + bufSize;
    while (str < end) {
        Entry entry;
        int seg5count = 0;
        for (int i = 0; *str != '|'; ++i) {
            const char *at = str;
            while (std::isalpha(*at)) ++at;
            std::string_view word(str, at - str);
            str = at + 1;

            std::bitset<7> digit = 0;
            for (int j = 0; j < word.size(); ++j) {
                digit |= (1 << ('g' - word[j]));
            }

            if (word.size() == 2)
                entry.digits[1] = digit;
            else if (word.size() == 3)
                entry.digits[7] = digit;
            else if (word.size() == 4)
                entry.digits[4] = digit;
            else if (word.size() == 7)
                entry.digits[8] = digit;

            if (word.size() == 5) {
                entry.seg5digits[seg5count] = digit;
                ++seg5count;
            }
        }

        str = str + 2;
        for (int i = 0; str < end && *str != '\n'; ++i) {
            const char *at = str;
            while (std::isalpha(*at)) ++at;
            std::string_view word(str, at - str);
            while (*at == ' ') ++at;
            str = at;

            for (int j = 0; j < word.size(); ++j) {
                entry.outputs[i] |= (1 << ('g' - word[j]));
            }
        }

        entries.push_back(entry);
        ++str;
    }
}

int main() {
    std::vector<Entry> entries;
    entries.reserve(200);

    size_t bufSize;
    const char *buf = mapFile("d8.txt", bufSize);
    parse(buf, bufSize, entries);

    int sum0 = 0;
    int sum1 = 0;
    for (auto &entry : entries) {

        //  AAAAA     AAAAA   AAAAA           AAAAA   AAAAA  AAAAA   AAAAA   AAAAA
        // F     B B       B       B F     B F       F            B F     B F     B
        // F     B B       B       B F     B F       F            B F     B F     B
        //            GGGGG   GGGGG   GGGGG   GGGGG   GGGGG          GGGGG   GGGGG
        // E     C C E             C       C       C E     C      C E     C       C
        // E     C C E             C       C       C E     C      C E     C       C
        //  DDDDD     DDDDD   DDDDD           DDDDD   DDDDD          DDDDD   DDDDD

        // segment A is the segment in 7 that is not in 1
        std::bitset<7> a = entry.digits[7] ^ entry.digits[1];
        // segments FG are the segments in 4 that are not in 1
        std::bitset<7> fg = entry.digits[4] ^ entry.digits[1];

        for (auto &digit : entry.seg5digits) {
            if ((digit & fg) == fg) {
                // digit 5 is the only digit with 5 segment and both FG segments
                entry.digits[5] = digit;
                break;
            }
        }

        // segment B is the segment in 1 that is not in 5
        std::bitset<7> b = entry.digits[1] & (~entry.digits[5]);
        // segment C is the segment in 1 that is not B
        std::bitset<7> c = entry.digits[1] ^ b;
        // segment D is the segment in 5 that is not A C F or G
        std::bitset<7> d = entry.digits[5] ^ (a | c | fg);

        std::bitset<7> g;
        for (auto &digit : entry.seg5digits) {
            // digit 3 is the five segment digit with only one segment left when we remove A B C and D
            // segment G is the remaining segment
            g = digit & (~(a | b | c | d));
            if (g.count() == 1) {
                entry.digits[3] = digit;
                break;
            }
        }

        // segment F is the segment in 5 that is not A C D or G
        std::bitset<7> f = entry.digits[5] ^ (a | g | c | d);
        // segment E is the segment in 8 that is not A B C D F or G
        std::bitset<7> e = entry.digits[8] ^ (a | b | c | d | f | g);
        entry.digits[0] = a | b | c | d | e | f;
        entry.digits[2] = a | b | d | e | g;
        entry.digits[6] = a | c | d | e | f | g;
        entry.digits[9] = a | b | c | d | f | g;

        int number = 0;
        int powerOf10[] = {1000, 100, 10, 1};
        for (int i = 0; i < 4; ++i) {
            size_t bitcount = entry.outputs[i].count();
            if (bitcount == 2 || bitcount == 4 || bitcount == 3 || bitcount == 7) {
                ++sum0;
            }

            for (int j = 0; j < 10; ++j) {
                if (entry.outputs[i] == entry.digits[j]) {
                    number += powerOf10[i] * j;
                }
            }
        }

        sum1 += number;
    }

    std::cout << "Premier gaulois: " << sum0 << std::endl;
    std::cout << "Second gaulois: " << sum1 << std::endl;
    return 0;
}