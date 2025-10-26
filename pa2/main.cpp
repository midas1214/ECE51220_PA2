// Build: g++ -std=c++17 -Wall -Wextra -Wshadow -O3 *.cpp -o pa2
#include <cstdio>
#include <cstdlib>
#include "pa2.hpp"

static std::FILE* xfopenw(const char* path) {
    if (std::FILE* f = std::fopen(path, "w")) return f;
    std::perror(path);
    std::exit(1);
}

int main(int argc, char** argv) {
    if (argc != 6) {
        std::fprintf(stderr,
            "Usage: %s in_file out_file1 out_file2 out_file3 out_file4\n", argv[0]);
        return 1;
    }

    const char* in_path  = argv[1];
    const char* out1path = argv[2];
    const char* out2path = argv[3];
    const char* out3path = argv[4];
    const char* out4path = argv[5];

    // 1) Parse → slicing tree (leaves store FIRST (w,h)), also keep input-order leaves
    std::vector<Node*> leaves_in_input_order;
    Node* root = parse_postorder_file(in_path, leaves_in_input_order);
    if (!root) {
        std::fprintf(stderr, "Parse failed\n");
        return 1;
    }

    // 2) Compute sizes and assign coordinates for FIRST-impl pass
    compute_sizes_first_impl(root);
    assign_coords_first_impl(root, /*x0=*/0, /*y0=*/0);

    // 3) Open outputs
    std::FILE* f1 = xfopenw(out1path);
    std::FILE* f2 = xfopenw(out2path);
    std::FILE* f3 = xfopenw(out3path);
    std::FILE* f4 = xfopenw(out4path);

    // 4) Write Output #1
    if (write_room_size(f1, root->size) != 0) {
        std::fprintf(stderr, "Failed writing out_file1\n");
    }

    // 5) Write Output #2 (per-block dims & coords, input order)
    if (write_block_list(f2, leaves_in_input_order) != 0) {
        std::fprintf(stderr, "Failed writing out_file2\n");
    }

    // 6) Cleanup
    std::fclose(f1);
    std::fclose(f2);
    std::fclose(f3);
    std::fclose(f4);
    free_tree(root);
    return 0;
}
