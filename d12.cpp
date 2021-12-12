#include <iostream>
#include <vector>
#include <string_view>

#include "utils.h"

enum class Cave {
    unknown,
    start,
    end,
    small,
    big,
};

struct Node {
    Cave type = Cave::unknown;
    std::string_view name;
    int visited = 0;
    std::vector<Node*> connected;
};


int crawl(Node *current) {
    current->visited = 1;

    int n = 0;
    for (Node *node : current->connected) {
        if (node->type == Cave::start) continue;
        if (node->type == Cave::end) {
            ++n;
            continue;
        }

        if (node->type == Cave::small && node->visited) continue;
        n += crawl(node);
    }

    current->visited = 0;
    return n;
}


int crawlv2(Node *current, bool allowtwice = true) {
    if (current->type == Cave::small && ++current->visited == 2) {
        allowtwice = false;
    }

    int n = 0;
    for (Node *node : current->connected) {
        if (node->type == Cave::start) continue;
        if (node->type == Cave::end) {
            ++n;
            continue;
        }

        if (node->type == Cave::small && !allowtwice && node->visited) {
            continue;
        }
        n += crawlv2(node, allowtwice);
    }

    --current->visited;
    return n;
}


constexpr int keystart = 's' + 't' + 'a' + 'r' + 't';
constexpr int keyend = 'e' + 'n' + 'd';

int main() {
    size_t bufSize;
    const char *buf = mapFile("d12.txt", bufSize);

    const char *str = buf;
    const char *strend = buf + bufSize;
    Node nodes[1000];
    while (str < strend) {
        const char *at = str;
        int key = 0;
        Cave type = *at >= 'A' && *at <= 'Z' ? Cave::big : Cave::small;
        // weird hash I know but after testing it theres no collision with my input so lets go
        while (*at != '-') {
            key += *at;
            ++at;
        }

        switch(key) {
            case keystart:
                type = Cave::start;
                break;
            case keyend:
                type = Cave::end;
                break;
        }

        Node &a = nodes[key];
        a.type = type;
        a.name = std::string_view(str, at - str);

        ++at;
        str = at;

        key = 0;
        type = *at >= 'A' && *at <= 'Z' ? Cave::big : Cave::small;
        while(at < strend && *at != '\n') {
            key += *at;
            ++at;
        }

        switch(key) {
            case keystart:
                type = Cave::start;
                break;
            case keyend:
                type = Cave::end;
                break;
        }

        Node &b = nodes[key];
        b.type = type;
        b.name = std::string_view(str, at - str);

        str = at + 1;

        a.connected.push_back(&b);
        b.connected.push_back(&a);
    }

    int first = crawl(&nodes[keystart]);
    int second = crawlv2(&nodes[keystart]);

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;
    return 0;
}