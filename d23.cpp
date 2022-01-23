#include <iostream>
#include <queue>
#include <unordered_set>

#include "utils.h"

struct ivec2 {
    int x, y;
};

struct Amphipod {
    ivec2 pos;
    unsigned int type = 0;
};

struct Board {
    uint64_t encodedState;
    unsigned int energy = 0;
    unsigned int heuristic = 0xFFFFFFFF;
};

struct State {
    unsigned int cells[5][11] = {0};
    Amphipod amphipods[18];
};

const int energyCosts[4] = {1, 10, 100, 1000};

ivec2 decodePos(int encoded) {
    if (encoded < 11) {
        return {encoded, 0};
    }

    ivec2 pos;
    encoded -= 11;
    pos.x = ((encoded & 3) + 1) * 2;
    pos.y = 1 + (encoded >> 2);
    return pos;
}

uint64_t encode(const State &state) {
    uint64_t hash = 0;
    for (int i = 0; i < 27; ++i) {
        hash *= 5;
        ivec2 pos = decodePos(i);
        unsigned int id = state.cells[pos.y][pos.x];
        if (id) {
            hash += state.amphipods[id].type + 1;
        }
    }

    return hash;
}

void decode(uint64_t encoded, State &state) {
    unsigned int index = 1;
    for (int i = 26; i >= 0; --i) {
        unsigned int type = encoded % 5;
        if (type) {
            ivec2 pos = decodePos(i);
            state.cells[pos.y][pos.x] = index;
            state.amphipods[index].pos = pos;
            state.amphipods[index].type = type - 1;
            ++index;
        }

        encoded /= 5;
    }
}

constexpr bool isSettled(const Amphipod &amphipod) {
    int goal = (amphipod.type + 1) * 2;
    return goal == amphipod.pos.x;
}

int leastEnergy(const Board &initialBoard, const int nAmphipod) {
    auto cmp = [](const Board &a, const Board &b) {
        return a.energy + a.heuristic > b.energy + b.heuristic;
    };
    std::priority_queue<Board, std::vector<Board>, decltype(cmp)> pqueue(cmp);
    std::unordered_set<uint64_t> visited;
    unsigned int roomSize = nAmphipod / 4;

    pqueue.push(initialBoard);
    while (pqueue.size()) {
        Board board = pqueue.top();
        pqueue.pop();

        // Skip if this position has already been visited
        if (visited.count(board.encodedState)) continue;
        visited.insert(board.encodedState);

        if (board.heuristic == 0) {
            return board.energy;
        }

        State state;
        decode(board.encodedState, state);

        bool settled = true;
        for (int i = 1; i < nAmphipod + 1; ++i) {
            if (!isSettled(state.amphipods[i])) {
                settled = false;
                break;
            }
        }
        if (settled) {
            return board.energy;
        }
        
        // Check whether rooms have space for their associated amphipods
        // and at which index they have to move to.
        bool isRoomFree[4];
        int roomFreeIndex[4];
        for (int x = 2; x <= 8; x += 2) {
            int roomIndex = x / 2 - 1;
            isRoomFree[roomIndex] = true;
            for (int y = roomSize; y > 0; --y) {
                if (state.cells[y][x] == 0) {
                    roomFreeIndex[roomIndex] = y;
                    break;
                }

                const Amphipod &amphipod = state.amphipods[state.cells[y][x]];
                if (amphipod.type != roomIndex) {
                    isRoomFree[roomIndex] = false;
                    break;
                }
            }
        }

        auto doMove = [&](State &state, const Amphipod &amphipod, ivec2 dst) {
            ivec2 p = amphipod.pos;
            unsigned int id = state.cells[p.y][p.x];
            // Modify the state, ready to encode.
            state.cells[p.y][p.x] = 0;
            state.cells[dst.y][dst.x] = id;
            state.amphipods[id].pos = {dst.x, dst.y};

            Board newBoard = board;
            newBoard.encodedState = encode(state);

            // Calculate the lower bound energy cost to the winning state.
            unsigned int heuristic = 0;
            for (int i = 1; i < nAmphipod + 1; ++i) {
                const Amphipod &a = state.amphipods[i];

                // If the amphipod is not in his room OR is blocking the way.
                if (!isSettled(a) || !isRoomFree[a.type]) {
                    if (a.pos.y != 0) {
                        heuristic += a.pos.y * energyCosts[a.type];
                    }
                    int dstx = (a.type + 1) * 2;
                    heuristic += (std::abs(a.pos.x - dstx) + 1) * energyCosts[a.type];
                }
            }

            newBoard.energy += (std::abs(dst.x - p.x) + std::abs(dst.y - p.y)) * energyCosts[amphipod.type];
            newBoard.heuristic = heuristic;
            pqueue.push(newBoard);

            // Undo state modification.
            state.cells[dst.y][dst.x] = 0;
            state.cells[p.y][p.x] = id;
            state.amphipods[id].pos = p;
        };

        for (int i = 1; i < nAmphipod + 1; ++i) {
            const Amphipod &amphipod = state.amphipods[i];
            int roomIndex = amphipod.pos.x / 2 - 1;

            if (amphipod.pos.y != 0 && (roomIndex != amphipod.type || !isRoomFree[roomIndex])) {
                // Either this amphipod is in the wrong room or he needs to move out of the way.
                // This is a hallway move.

                // Check whether this amphipod is free to move out of the room.
                bool canMove = true;
                for (int y = amphipod.pos.y - 1; y > 0; --y) {
                    if (state.cells[y][amphipod.pos.x] != 0) {
                        canMove = false;
                        break;
                    }
                }
                if (!canMove) continue;

                for (int x = amphipod.pos.x + 1; x < 11 && state.cells[0][x] == 0; ++x) {
                    if (x == 2 || x == 4 || x == 6 || x == 8) continue;
                    doMove(state, amphipod, {x, 0});
                }

                for (int x = amphipod.pos.x - 1; x >= 0 && state.cells[0][x] == 0; --x) {
                    if (x == 2 || x == 4 || x == 6 || x == 8) continue;
                    doMove(state, amphipod, {x, 0});
                }

            } else if (amphipod.pos.y == 0 && isRoomFree[amphipod.type]) {
                // The side room of this amphipod has space for him.
                // This is a sideroom move.

                ivec2 dst;
                dst.x = (amphipod.type + 1) * 2;
                dst.y = roomFreeIndex[amphipod.type];

                // Check whether the hallway is clear for this amphipod to move
                int dirx = dst.x - amphipod.pos.x;
                dirx = (dirx >> 31) | (!!dirx);
                bool canMove = true;
                for (int x = amphipod.pos.x + dirx; x != dst.x; x += dirx) {
                    if (state.cells[0][x] != 0) {
                        canMove = false;
                        break;
                    }
                }
                if (!canMove) continue;

                doMove(state, amphipod, dst);
            }
        }
    }

    return 0;
}

int main() {
    size_t bufSize;
    const char *buf = mapFile("d23.txt", bufSize);

    State state;
    state.amphipods[1].type = buf[31] - 'A';
    state.amphipods[2].type = buf[33] - 'A';
    state.amphipods[3].type = buf[35] - 'A';
    state.amphipods[4].type = buf[37] - 'A';
    state.amphipods[5].type = buf[45] - 'A';
    state.amphipods[6].type = buf[47] - 'A';
    state.amphipods[7].type = buf[49] - 'A';
    state.amphipods[8].type = buf[51] - 'A';

    int k = 1;
    for (int y = 1; y <= 2; ++y) {
        for (int x = 2; x <= 8; x += 2) {
            state.amphipods[k].pos.x = x;
            state.amphipods[k].pos.y = y;
            state.cells[y][x] = k;
            ++k;
        }
    }

    Board board;
    board.encodedState = encode(state);
    int first = leastEnergy(board, 8);

    for (int i = 0; i < 4; ++i) {
        state.amphipods[5 + i].pos.y += 2;
        state.cells[4][2 + i * 2] = state.cells[2][2 + i * 2];
    }

    state.amphipods[9].type = 3;
    state.amphipods[10].type = 2;
    state.amphipods[11].type = 1;
    state.amphipods[12].type = 0;
    state.amphipods[13].type = 3;
    state.amphipods[14].type = 1;
    state.amphipods[15].type = 0;
    state.amphipods[16].type = 2;

    k = 9;
    for (int j = 2; j <= 3; ++j) {
        for (int i = 2; i <= 8; i += 2) {
            state.amphipods[k].pos = {i, j};
            state.cells[j][i] = k;
            ++k;
        }
    }

    board.encodedState = encode(state);
    int second = leastEnergy(board, 16);

    std::cout << "Premier gaulois: " << first << std::endl;
    std::cout << "Second gaulois: " << second << std::endl;

    return 0;
}