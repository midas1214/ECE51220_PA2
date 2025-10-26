#include "pa2.hpp"

void free_tree(Node* x) {
    if (!x) return;
    free_tree(x->l);
    free_tree(x->r);
    delete x;
}
