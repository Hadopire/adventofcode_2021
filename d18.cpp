#include <cmath>
#include <iostream>
#include <list>
#include <vector>

#include "utils.h"

struct Number {
    int value;
    int depth;
};

using SnailNumber = std::list<Number>;

SnailNumber::iterator explode(SnailNumber &number, SnailNumber::iterator it) {
    auto next = std::next(it, 1);
    if (it != number.begin()) {
        auto prev = std::prev(it, 1);
        prev->value += it->value;
    }

    auto nextnext = std::next(next, 1);
    if (nextnext != number.end()) {
        nextnext->value += next->value;
    }

    it->value = 0;
    it->depth -= 1;
    if (it != number.begin()) it = std::prev(it, 1);
    number.erase(next);


    
    return it;
}

void explodeAll(SnailNumber &number, int baseDepth) {
    for (auto it = number.begin(); it != number.end();) {
        auto next = std::next(it, 1);
        if (next == number.end()) break;
        if (it->depth + baseDepth > 4 && next->depth == it->depth) {
            it = explode(number, it);
        } else {
            ++it;
        }
    }
}

size_t magnitude(SnailNumber &number) {
    while (number.size() != 1) {
        for (auto it = number.begin(); it != number.end();) {
            auto next = std::next(it, 1);
            if (next == number.end()) break;
            if (next->depth == it->depth) {
                size_t magnitude = it->value * 3 + next->value * 2;
                it->value = magnitude;
                it->depth -= 1;
                if (it != number.begin()) --it;
                number.erase(next);
            } else {
                ++it;
            }
        }
    }
    return number.front().value;
}

void eval(SnailNumber &number, int baseDepth) {
    explodeAll(number, baseDepth);
    for (auto it = number.begin(); it != number.end();) {
        if (it->value >= 10) {
            int a = std::floor(it->value / 2.0);
            int b = std::ceil(it->value / 2.0);
            it->depth += 1;
            it->value = b;
            it = number.insert(it, { a, it->depth });
            
            if (it->depth + baseDepth > 4) {
                it = explode(number, it);
            }
        } else {
            ++it;
        }
    }
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d18.txt", bufSize);

    std::vector<SnailNumber> numbers;

    const char *str = buf;
    const char *strend = buf + bufSize;
    int depth = 0;
    while (str < strend) {
        SnailNumber number;
        while (str < strend && *str != '\n') {
            if (*str == '[') {
                ++depth;
            } else if (*str == ']') {
                --depth;
            } else if (*str != ','){
                number.push_back({*str - '0', depth});
            }
            ++str;
        }
        numbers.push_back(std::move(number));
        if (numbers.size() >= 2) --depth;
        ++str;
    }

    SnailNumber sum = numbers[0];
    for (int i = 1; i < numbers.size(); ++i) {
        std::copy(numbers[i].begin(), numbers[i].end(), std::back_inserter(sum));
        eval(sum, i);
        
        if (i == 1) continue;
        for (auto &n : numbers[i]) {
            n.depth += i - 1;
        }
    }

    size_t first = magnitude(sum);
    size_t second = 0;
    for (int i = 0; i < numbers.size(); ++i) {
        for (int j = 0; j < numbers.size(); ++j) {
            if (i == j) continue;
            SnailNumber sum = numbers[i];
            std::copy(numbers[j].begin(), numbers[j].end(), std::back_inserter(sum));
            eval(sum, 1);
            second = std::max(second, magnitude(sum));
        }
    }

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}