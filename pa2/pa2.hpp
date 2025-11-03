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
    Dim size{};                 // used by Outputs #1/2 and after backtrack for #4
    std::vector<Dim> impls;     // ALL leaf implementations

    // Placement (for #2 and #4)
    int x{0}, y{0};

    // --- Frontier for #3/#4 (with backpointers) ---
    struct FP {
        Dim d;        // this frontier point's (W,H)
        int li{-1};   // index into left->frontier_pts (for internals)
        int ri{-1};   // index into right->frontier_pts (for internals)
        int impl{-1}; // for leaves: index into impls; for internals: -1
    };
    std::vector<FP> frontier_pts;
};

/* parse.cpp */
Node* parse_postorder_file(const char* path, std::vector<Node*>& leaves_out);

/* pack.cpp */
void compute_sizes_first_impl(Node* root);
void assign_coords_first_impl(Node* root, int x0, int y0);

/* opt.cpp */
// Rebuilds frontiers and returns best (W,H) area at root (for Output #3)
Dim compute_optimal_room_size(Node* root);

// Rebuilds frontiers, picks a min-area root point, BACKTRACKS to set each node's
// chosen size (and leaf impl), then returns the chosen root (W,H). (For Output #4)
Dim compute_and_apply_optimal(Node* root);

/* tree.cpp */
void free_tree(Node* x);

/* output.cpp */
int write_room_size(std::FILE* f, Dim d);
int write_block_list(std::FILE* f, const std::vector<Node*>& leaves);

#endif // PA2_HPP
