#include "pa2.hpp"

namespace {
inline int max2(int a, int b) { return a > b ? a : b; }

void dfs_sizes(Node* x) {
    if (!x) return;
    if (x->t == NodeType::LEAF) return;
    dfs_sizes(x->l);
    dfs_sizes(x->r);
    if (x->t == NodeType::V) {
        x->size.w = x->l->size.w + x->r->size.w;
        x->size.h = max2(x->l->size.h, x->r->size.h);
    } else { // H
        x->size.w = max2(x->l->size.w, x->r->size.w);
        x->size.h = x->l->size.h + x->r->size.h;
    }
}

// Placement follows the spec's convention:
// V: left at (x,y), right at (x + W_left, y)
// H: right at (x,y), left at (x, y + H_right)
void dfs_place(Node* x, int x0, int y0) {
    if (!x) return;
    if (x->t == NodeType::LEAF) {
        x->x = x0; x->y = y0;
        return;
    }
    if (x->t == NodeType::V) {
        // left first, right shifted in +x by left width
        dfs_place(x->l, x0, y0);
        dfs_place(x->r, x0 + x->l->size.w, y0);
    } else { // H
        // right at base, left stacked above by right height
        dfs_place(x->r, x0, y0);
        dfs_place(x->l, x0, y0 + x->r->size.h);
    }
}
} // namespace

void compute_sizes_first_impl(Node* root) {
    dfs_sizes(root);
}

void assign_coords_first_impl(Node* root, int x0, int y0) {
    dfs_place(root, x0, y0);
}
