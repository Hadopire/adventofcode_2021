#include <iostream>
#include <vector>
#include <charconv>
#include <unordered_set>
#include <list>

#include "utils.h"

#define countof(x) (sizeof(x) / sizeof(x[0]))

struct ivec3 {
    int x, y, z;

    ivec3 operator-(const ivec3 &rhs) {
        return ivec3 {
            x - rhs.x,
            y - rhs.y,
            z - rhs.z
        };
    }

    ivec3 operator+(const ivec3 &rhs) {
        return ivec3 {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z
        };
    }

    bool operator==(const ivec3 &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator!=(const ivec3 &rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }
};

struct Hash {
    size_t operator()(const ivec3 &p) const {
        return p.x * 0x6FAE3FF4C49B3E11ull + p.y * 0xED62681331FA3117ull + p.z;
    }
};

ivec3 XYZ(ivec3& p)    { return ivec3{  p.x,  p.y,  p.z }; }
ivec3 XmYmZ(ivec3& p)  { return ivec3{  p.x, -p.y, -p.z }; }
ivec3 XZmY(ivec3& p)   { return ivec3{  p.x,  p.z, -p.y }; }
ivec3 XmZY(ivec3& p)   { return ivec3{  p.x, -p.z,  p.y }; }

ivec3 mXZY(ivec3& p)   { return ivec3{ -p.x,  p.z,  p.y }; }
ivec3 mXmZmY(ivec3& p) { return ivec3{ -p.x, -p.z, -p.y }; }
ivec3 mXYmZ(ivec3& p)  { return ivec3{ -p.x,  p.y, -p.z }; }
ivec3 mXmYZ(ivec3& p)  { return ivec3{ -p.x, -p.y,  p.z }; }

ivec3 YZX(ivec3& p)    { return ivec3{  p.y,  p.z,  p.x }; }
ivec3 YmZmX(ivec3& p)  { return ivec3{  p.y, -p.z, -p.x }; }
ivec3 YXmZ(ivec3& p)   { return ivec3{  p.y,  p.x, -p.z }; }
ivec3 YmXZ(ivec3& p)   { return ivec3{  p.y, -p.x,  p.z }; }

ivec3 mYXZ(ivec3& p)   { return ivec3{ -p.y,  p.x,  p.z }; }
ivec3 mYmXmZ(ivec3& p) { return ivec3{ -p.y, -p.x, -p.z }; }
ivec3 mYZmX(ivec3& p)  { return ivec3{ -p.y,  p.z, -p.x }; }
ivec3 mYmZX(ivec3& p)  { return ivec3{ -p.y, -p.z,  p.x }; }

ivec3 ZXY(ivec3& p)    { return ivec3{  p.z,  p.x,  p.y }; }
ivec3 ZmXmY(ivec3& p)  { return ivec3{  p.z, -p.x, -p.y }; }
ivec3 ZYmX(ivec3& p)   { return ivec3{  p.z,  p.y, -p.x }; }
ivec3 ZmYX(ivec3& p)   { return ivec3{  p.z, -p.y,  p.x }; }

ivec3 mZYX(ivec3& p)   { return ivec3{ -p.z,  p.y,  p.x }; }
ivec3 mZmYmX(ivec3& p) { return ivec3{ -p.z, -p.y, -p.x }; }
ivec3 mZXmY(ivec3& p)  { return ivec3{ -p.z,  p.x, -p.y }; }
ivec3 mZmXY(ivec3& p)  { return ivec3{ -p.z, -p.x,  p.y }; }

using TransformFn = ivec3 (*)(ivec3&);
const TransformFn transforms[24] {
    XYZ, XmYmZ, XZmY, XmZY,
    mXZY, mXmZmY, mXYmZ, mXmYZ,
    YZX, YmZmX, YXmZ, YmXZ,
    mYXZ, mYmXmZ, mYZmX, mYmZX,
    ZXY, ZmXmY, ZYmX, ZmYX,
    mZYX, mZmYmX, mZXmY, mZmXY,
};

struct Scan {
    std::vector<ivec3> array;
    std::vector<ivec3> transformed;
    std::vector<std::unordered_set<int>> distances;
    std::unordered_set<ivec3, Hash> set;
    ivec3 position = { 0, 0, 0 };
};


bool overlap(Scan &a, Scan &b, int aidx, int bidx) {
    for (int i = 0; i < countof(transforms); ++i) {
        int nmatch = 0;
        ivec3 spos = ivec3{0, 0, 0} - b.array[bidx];
        spos = transforms[i](spos) + a.array[aidx];
        for (int j = 0; j < b.array.size(); ++j) {
            ivec3 &pos = b.transformed[j];
            pos = b.array[j] - b.array[bidx];
            pos = transforms[i](pos);
            pos = pos + a.array[aidx];
            if (nmatch < 12 && a.set.count(pos)) {
                ++nmatch;
            }
        }

        if (nmatch >= 12) {
            b.array = std::move(b.transformed);
            b.set.clear();
            b.position = spos;
            for (auto &pos : b.array) {
                b.set.insert(pos);
            }
            return true;
        }
    }

    return false;
}

bool match(Scan &a, Scan &b) {
    for (int a0 = 0; a0 < a.array.size(); ++a0) {
        ivec3 beaconA = a.array[a0];
        auto &distFromA = a.distances[a0];
        if (distFromA.size() == 0) {
            distFromA.reserve(a.array.size() * 2);
            for (int a1 = 0; a1 < a.array.size(); ++a1) {
                if (a0 == a1) continue;
                ivec3 dif = beaconA - a.array[a1];
                distFromA.insert(std::abs(dif.x) + std::abs(dif.y) + std::abs(dif.z));
            }
        }

        for (int b0 = 0; b0 < b.array.size(); ++b0) {
            ivec3 beaconB = b.array[b0];
            int nmatch = 0;
            for (int b1 = 0; b1 < b.array.size(); ++b1) {
                if (b0 == b1) continue;
                ivec3 dif = beaconB - b.array[b1];
                int mdist = std::abs(dif.x) + std::abs(dif.y) + std::abs(dif.z);
                if (distFromA.count(mdist)) {
                    ++nmatch;
                    if (nmatch == 11) {
                        if (overlap(a, b, a0, b0)) {
                            return true;
                        }

                        break;
                    }
                }
            }
        }
    }

    return false;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d19.txt", bufSize);

    std::list<Scan> scans;
    std::list<Scan> orientedScans;
    size_t nentry = 0;

    const char *str = buf + 18; // skip first line
    const char *strend = buf + bufSize;
    while (str < strend) {
        Scan &scan = scans.emplace_back();
        scan.array.reserve(150);
        scan.set.reserve(150);
        while (str < strend && *str != '\n') {
            ivec3 &coordinates = scan.array.emplace_back();
            auto result = std::from_chars(str, strend, coordinates.x);
            result = std::from_chars(result.ptr + 1, strend, coordinates.y);
            result = std::from_chars(result.ptr + 1, strend, coordinates.z);
            scan.set.insert(coordinates);
            str = result.ptr + 1;
            ++nentry;
        }
        scan.distances.resize(scan.array.size());
        scan.transformed.resize(scan.array.size());

        str += 19;
        if (str < strend && *str == '\n') ++str;
    }

    orientedScans.splice(orientedScans.end(), scans, scans.begin());

    for (auto i = orientedScans.begin(); i != orientedScans.end(); ++i) {
        Scan &a = *i;
        for (auto j = scans.begin(); j != scans.end();) {
            Scan &b = *j;
            bool matched = match(a, b);
            if (matched) {
                auto tomove = j;
                ++j;
                orientedScans.splice(orientedScans.end(), scans, tomove);
            } else {
                ++j;
            }
        }
    }

    int second = 0;
    std::unordered_set<ivec3, Hash> uniques;
    uniques.reserve(nentry * 2);
    for (auto it = orientedScans.begin(); it != orientedScans.end(); ++it) {
        for (auto &pos : it->array) {
            uniques.insert(pos);
        }

        for (auto next = std::next(it, 1); next != orientedScans.end(); ++next) {
            ivec3 dif = next->position - it->position;
            second = std::max(second, std::abs(dif.x) + std::abs(dif.y) + std::abs(dif.z));
        }
    }
    std::cout << "Premier gaulois: " << uniques.size() << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}