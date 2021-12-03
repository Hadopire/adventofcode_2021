#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#define NUM_BITS 12

int findRating(std::vector<int>::iterator start, std::vector<int>::iterator end, bool reverse) {
    int bound = 0;
    for (int i = 0; i < NUM_BITS; ++i) {
        int size = std::distance(start, end);
        if (size == 1) {
            return *start;
        }

        auto mid = start + size / 2;
        int search = bound | (1 << (NUM_BITS - 1 - i));
        auto it = std::lower_bound(start, end, search);

        if ((mid >= it) != reverse) {
            start = it;
            bound = search;
        } else {
            end = it;
        }
    }

    return *start;
}

int main() {
    std::fstream file("d3.txt");

    char str[NUM_BITS + 1];
    int count[NUM_BITS]{0};
    std::vector<int> inputs;
    inputs.reserve(2000);
    while (file >> str) {
        int n = 0;
        for (int i = 0; i < NUM_BITS; ++i) {
            if (str[i] == '1') {
                ++count[i];
                n |= (1 << (NUM_BITS - 1 - i));
            } else {
                --count[i];
            }
        }
        inputs.push_back(n);
    }

    int gamma = 0;
    for (int i = 0; i < NUM_BITS; ++i) {
        if (count[i] > 0) {
            gamma |= (1 << (NUM_BITS - 1 - i));
        }
    }
    int epsilon = (~gamma & 0xFFF);

    std::sort(inputs.begin(), inputs.end());
    int oxygen = findRating(inputs.begin(), inputs.end(), false);
    int co2 = findRating(inputs.begin(), inputs.end(), true);

    std::cout << "Premier gaulois: " << gamma * epsilon << std::endl;
    std::cout << "Second gaulois: " << oxygen * co2 << std::endl;

    return 0;
}