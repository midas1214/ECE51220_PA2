#ifndef PA2_HPP
#define PA2_HPP

#include <cstdio>
#include <vector>

struct Dim { int w{}, h{}; };

enum class NodeType { V, H, LEAF };

struct Node {
    NodeType t{};
    Node* l{nullptr};
    Node* r{nullptr};
    int label{0};      // for leaves
    Dim size{};        // for leaves: first (w,h); internals: computed
    int x{0}, y{0};    // lower-left coordinates after placement
};

/* parse.cpp */
// Also collects leaves in input order into leaves_out.
Node* parse_postorder_file(const char* path, std::vector<Node*>& leaves_out);

/* pack.cpp */
void compute_sizes_first_impl(Node* root);
void assign_coords_first_impl(Node* root, int x0, int y0);

/* tree.cpp */
void free_tree(Node* x);

/* output.cpp */
int write_room_size(std::FILE* f, Dim d);
// Write each leaf in input order: label((w,h)(x,y))
int write_block_list(std::FILE* f, const std::vector<Node*>& leaves);

#endif // PA2_HPP
