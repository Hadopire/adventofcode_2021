#include <iostream>
#include <charconv>

#include "utils.h"

int main() {
    size_t bufSize;
    const char *buf = mapFile("d24.txt", bufSize);

    char largest[15] = { 0 };
    char smallest[15] = { 0 };
    int idstack[7];
    int constantstack[7];
    int at = -1;

    const char *str = buf;
    const char *strend = buf + bufSize;
    for (int i = 0; str < strend - 1; ++i) {
        str += 37;
        bool push = *str == '1';
        while (!isspace(*str)) ++str;

        if (push) {
            for (int i = 0; i < 2; ++i) {
                while (*str != 'w') ++str;
                ++str;
            }

            int constant;
            std::from_chars(str + 7, strend, constant);

            ++at;
            idstack[at] = i;
            constantstack[at] = constant;
        } else {
            int constant;
            std::from_chars(str + 7, strend, constant);
            constant += constantstack[at];
            int id = idstack[at];
            --at;

            // we need : a + constant == b
            if (constant >= 0) {
                largest[id] = '0' + (9 - constant);
                largest[i] = '9';
                smallest[id] = '1';
                smallest[i] = '1' + constant;
            } else {
                largest[id] = '9';
                largest[i] = '0' + (9 + constant);
                smallest[id] = '1' - constant;
                smallest[i] = '1';
            }
        }

        while (str < strend - 1 && str[1] != 'n') ++str;
    }

    std::cout << "Premier gaulois: " << largest << std::endl;
    std::cout << "Second gaulois: " << smallest << std::endl;

    return 0;
}