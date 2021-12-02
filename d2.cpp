#include <fstream>
#include <iostream>

struct position {
    int horizontal;
    int depth;
    int eye;
};

int main() {
    std::fstream file("d2.txt");
    position posA{0};
    position posB{0};

    char str[8];
    int n;
    while (file >> str && file >> n) {
        if (str[0] == 'f') {
            posA.horizontal += n;
            posB.horizontal += n;
            posB.depth += posB.eye * n;
        } else if (str[0] == 'u') {
            posA.depth -= n;
            posB.eye -= n;
        } else if (str[0] == 'd') {
            posA.depth += n;
            posB.eye += n;
        }
    }

    std::cout << "Premier gaulois: " << posA.horizontal * posA.depth << std::endl;
    std::cout << "Second gaulois: " << posB.horizontal * posB.depth << std::endl;
    return 0;
}