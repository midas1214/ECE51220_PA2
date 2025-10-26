#include "pa2.hpp"
#include <cstdio>

int write_room_size(std::FILE* f, Dim d) {
    return std::fprintf(f, "(%d,%d)\n", d.w, d.h) < 0 ? -1 : 0;
}

int write_block_list(std::FILE* f, const std::vector<Node*>& leaves) {
    for (const Node* n : leaves) {
        if (std::fprintf(f, "%d((%d,%d)(%d,%d))\n",
                         n->label, n->size.w, n->size.h, n->x, n->y) < 0)
            return -1;
    }
    return 0;
}
