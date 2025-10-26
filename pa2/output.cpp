#include "pa2.hpp"
#include <cstdio>

int write_room_size(std::FILE* f, Dim d) {
    return std::fprintf(f, "(%d,%d)\n", d.w, d.h) < 0 ? -1 : 0;
}
