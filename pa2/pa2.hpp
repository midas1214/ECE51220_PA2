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

    // Leaf info
    int label{0};
    Dim size{};                 // used by Outputs #1/2 (first impl & internal computed)
    std::vector<Dim> impls;     // ALL (w,h) implementations for the leaf

    // Placement (for Output #2, kept as-is)
    int x{0}, y{0};

    // DP frontier (for Output #3/4)
    std::vector<Dim> frontier;  // Pareto-optimal (W,H) for this subtree
};

/* parse.cpp */
// For Outputs #1/2 we still fill 'size' with the FIRST impl.
// For Outputs #3/4 we also fill 'impls' with ALL impls for each leaf.
Node* parse_postorder_file(const char* path, std::vector<Node*>& leaves_out);

/* pack.cpp */
void compute_sizes_first_impl(Node* root);
void assign_coords_first_impl(Node* root, int x0, int y0);

/* opt.cpp */
Dim compute_optimal_room_size(Node* root);  // returns best (W,H) by area

/* tree.cpp */
void free_tree(Node* x);

/* output.cpp */
int write_room_size(std::FILE* f, Dim d);
int write_block_list(std::FILE* f, const std::vector<Node*>& leaves);

#endif // PA2_HPP
