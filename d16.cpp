#include <iostream>
#include <vector>

#include "utils.h"

using BitBuffer = std::vector<bool>;

int int3(BitBuffer &bits, int &pos) {
    int n = 0;
    n |= bits[pos++] << 2;
    n |= bits[pos++] << 1;
    n |= bits[pos++];
    return n;
}

int int4(BitBuffer &bits, int &pos) {
    int n = 0;
    n |= bits[pos++] << 3;
    n |= bits[pos++] << 2;
    n |= bits[pos++] << 1;
    n |= bits[pos++];
    return n;
}

int intn(BitBuffer &bits, int &pos, int nbits) {
    int n = 0;
    for (int i = nbits - 1; i >= 0; --i) {
        n |= bits[pos++] << i;
    }

    return n;
}

size_t literal(BitBuffer &bits, int &pos) {
    size_t n = 0;
    while (bits[pos++]) {
        n <<= 4;
        n |= int4(bits, pos);
    }
    n <<= 4;
    n |= int4(bits, pos);

    return n;
}

constexpr size_t op(size_t a, size_t b, int op) {
    switch (op) {
    case 0:
        return a + b;
    case 1:
        return a * b;
    case 2:
        return std::min(a, b);
    case 3:
        return std::max(a, b);
    case 5:
        return a > b;
    case 6:
        return a < b;
    case 7:
        return a == b;
    }

    return 0;
}

size_t packet(BitBuffer &bits, int &pos, int &vsum) {
    int version = int3(bits, pos);
    int id = int3(bits, pos);
    size_t value;

    vsum += version;

    if (id == 4) {
        value = literal(bits, pos);
    } else {
        bool lengthType = bits[pos++];
        if (!lengthType) {
            int length = intn(bits, pos, 15);
            int end = pos + length;
            value = packet(bits, pos, vsum);
            while (pos < end) {
                value = op(value, packet(bits, pos, vsum), id);
            }
        } else {
            int npacket = intn(bits, pos, 11);
            value = packet(bits, pos, vsum);
            for (int i = 1; i < npacket; ++i) {
                value = op(value, packet(bits, pos, vsum), id);
            }
        }
    }

    return value;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d16.txt", bufSize);

    size_t nbits = bufSize * 4;
    BitBuffer bits(nbits);

    const char *str = buf;
    const char *strend = buf + bufSize;
    for (int i = 0; i < bufSize; ++i) {
        int value = str[i] >= 'A' ? str[i] - 'A' + 10 : str[i] - '0';
        bits[i * 4] = value >> 3;
        bits[i * 4 + 1] = (value >> 2) & 1;
        bits[i * 4 + 2] = (value >> 1) & 1;
        bits[i * 4 + 3] = value & 1;
    }

    int at = 0;
    int vsum = 0;
    size_t result = packet(bits, at, vsum);

    std::cout << "Premier gaulois: " << vsum << std::endl;
    std::cout << "Second gaulois: " << result << std::endl;

    return 0;
}