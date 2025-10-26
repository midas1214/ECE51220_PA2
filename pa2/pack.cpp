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
} // namespace

void compute_sizes_first_impl(Node* root) {
    dfs_sizes(root);
}
