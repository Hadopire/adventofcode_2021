#include <iostream>
#include <charconv>
#include <vector>
#include <cmath>

#include "utils.h"

#define WIN_SCORE 21
#define countof(x) (sizeof(x) / sizeof(x[0]))

using QuanticStates = size_t(*)[WIN_SCORE][10][10];

int main() {
    size_t bufSize;
    const char *buf = mapFile("d21.txt", bufSize);

    int start0, start1;
    auto result = std::from_chars(buf + 28, buf + bufSize, start0);
    std::from_chars(result.ptr + 29, buf + bufSize, start1);

    std::vector<int> positions0, positions1;
    int sum0 = 0;
    int sum1 = 0;
    int dices0[] = {6, 24, 42, 60, 78};
    int dices1[] = {15, 33, 51, 69, 87};

    int pos0 = start0;
    int dice = 0;
    for (int i = 0; dice != dices0[4] || pos0 != start0; ++i) {
        dice = dices0[i % 5];
        pos0 = 1 + (pos0 + dice - 1) % 10;
        sum0 += pos0;
        positions0.push_back(pos0);
    }

    int pos1 = start1;
    dice = 0;
    for (int i = 0; dice != dices1[4] || pos1 != start1; ++i) {
        dice = dices1[i % 5];
        pos1 = 1 + (pos1 + dice - 1) % 10;
        sum1 += pos1;
        positions1.push_back(pos1);
    }

    int p0win = std::ceil(1000.0 * positions0.size() / sum0);
    int p1win = std::ceil(1000.0 * positions1.size() / sum1);

    int first;
    if (p0win <= p1win) {
        first = (p0win - 1) / positions1.size() * sum1;
        for (int i = 0; i < (p0win - 1) % positions1.size(); ++i) {
            first += positions1[i];
        }
        first *= (p0win * 2 - 1) * 3;
    } else {
        first = p1win / positions0.size() * sum0;
        for (int i = 0; i < p1win % positions0.size(); ++i) {
            first += positions0[i];
        }
        first *= p1win * 2 * 3;
    }

    int dicesSums[7] = {1, 3, 6, 7, 6, 3, 1};
    QuanticStates states = (QuanticStates) new size_t[WIN_SCORE * WIN_SCORE * 10 * 10]{0};
    QuanticStates nextStates = (QuanticStates) new size_t[WIN_SCORE * WIN_SCORE * 10 * 10]{0};
    states[0][0][start0 - 1][start1 - 1] = 1;

    size_t p0wins = 0;
    size_t p1wins = 0;

    int minp0score = 0;
    int minp1score = 0;
    int maxp0score = 0;
    int maxp1score = 0;

    while (minp0score < WIN_SCORE && minp1score < WIN_SCORE) {
        int max = 0;
        int min = WIN_SCORE + 1;
        for (int s0 = minp0score; s0 <= std::min(maxp0score, WIN_SCORE-1); ++s0) {
            for (int s1 = minp1score; s1 <= std::min(maxp1score, WIN_SCORE-1); ++s1) {
                for (int pos0 = 0; pos0 < 10; ++pos0) {
                    for (int pos1 = 0; pos1 < 10; ++pos1) {
                        if (states[s0][s1][pos0][pos1] != 0) {
                            for (int i = 0; i < countof(dicesSums); ++i) {
                                int dice = i + 3;
                                int multiplier = dicesSums[i];
                                int pos = (pos0 + dice) % 10;
                                int score = s0 + pos + 1;
                                max = std::max(score, max);
                                min = std::min(score, min);
                                if (score >= WIN_SCORE) {
                                    p0wins += states[s0][s1][pos0][pos1] * multiplier;
                                } else {
                                    nextStates[score][s1][pos][pos1] += states[s0][s1][pos0][pos1] * multiplier;
                                }
                            }
                            states[s0][s1][pos0][pos1] = 0;
                        }
                    }
                }
            }
        }
        minp0score = min;
        maxp0score = max;
        std::swap(states, nextStates);
        
        max = 0;
        min = WIN_SCORE + 1;
        for (int s0 = minp0score; s0 <= std::min(maxp0score, WIN_SCORE-1); ++s0) {
            for (int s1 = minp1score; s1 <= std::min(maxp1score, WIN_SCORE-1); ++s1) {
                for (int pos0 = 0; pos0 < 10; ++pos0) {
                    for (int pos1 = 0; pos1 < 10; ++pos1) {
                        if (states[s0][s1][pos0][pos1] != 0) {
                            for (int i = 0; i < countof(dicesSums); ++i) {
                                int dice = i + 3;
                                int multiplier = dicesSums[i];
                                int pos = (pos1 + dice) % 10;
                                int score = s1 + pos + 1;
                                max = std::max(score, max);
                                min = std::min(score, min);
                                if (score >= WIN_SCORE) {
                                    p1wins += states[s0][s1][pos0][pos1] * multiplier;
                                } else {
                                    nextStates[s0][score][pos0][pos] += states[s0][s1][pos0][pos1] * multiplier;
                                }
                            }
                            states[s0][s1][pos0][pos1] = 0;
                        }
                    }
                }
            }
        }
        minp1score = min;
        maxp1score = max;
        std::swap(states, nextStates);
    }
    size_t second = std::max(p0wins, p1wins);

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;

    return 0;
}