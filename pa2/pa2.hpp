#ifndef PA2_HPP
#define PA2_HPP

#include <cstdio>

struct Dim { int w{}, h{}; };

enum class NodeType { V, H, LEAF };

struct Node {
    NodeType t{};
    Node* l{nullptr};
    Node* r{nullptr};
    int label{0};   // kept for later outputs
    Dim size{};     // leaves: first (w,h); internals: computed
};

/* parse.cpp */
Node* parse_postorder_file(const char* path);

/* pack.cpp */
void compute_sizes_first_impl(Node* root);

/* tree.cpp */
void free_tree(Node* x);

/* output.cpp */
int write_room_size(std::FILE* f, Dim d);

#endif // PA2_HPP
