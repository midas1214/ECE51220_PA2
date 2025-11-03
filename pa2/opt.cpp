#include "pa2.hpp"
#include <algorithm>
#include <limits>

// Sort by w asc, then h asc
static inline void sort_wh(std::vector<Node::FP>& v) {
    std::sort(v.begin(), v.end(), [](const Node::FP& a, const Node::FP& b){
        if (a.d.w != b.d.w) return a.d.w < b.d.w;
        return a.d.h < b.d.h;
    });
}

// Remove duplicates and dominated points; keep skyline with strictly decreasing h
static void pareto_prune(std::vector<Node::FP>& v) {
    if (v.empty()) return;
    sort_wh(v);

    // dedup equal (w,h) – keep the first (preserves its backpointers)
    v.erase(std::unique(v.begin(), v.end(), [](const Node::FP& a, const Node::FP& b){
        return a.d.w == b.d.w && a.d.h == b.d.h;
    }), v.end());

    // skyline: as w increases, keep only strictly smaller h's
    std::vector<Node::FP> out;
    out.reserve(v.size());
    int best_h = std::numeric_limits<int>::max();
    for (const auto& p : v) {
        if (p.d.h < best_h) {
            out.push_back(p);
            best_h = p.d.h;
        }
    }
    v.swap(out);
}

static void build_frontier(Node* x) {
    if (!x) return;
    if (x->t == NodeType::LEAF) {
        x->frontier_pts.clear();
        x->frontier_pts.reserve(x->impls.size());
        for (int i = 0; i < (int)x->impls.size(); ++i) {
            Node::FP pt;
            pt.d    = x->impls[i];
            pt.li   = -1;
            pt.ri   = -1;
            pt.impl = i;   // leaf impl index
            x->frontier_pts.push_back(pt);
        }
        pareto_prune(x->frontier_pts);
        return;
    }

    build_frontier(x->l);
    build_frontier(x->r);

    const auto& L = x->l->frontier_pts;
    const auto& R = x->r->frontier_pts;

    std::vector<Node::FP> cand;
    cand.reserve(L.size() * R.size());

    if (x->t == NodeType::V) {
        for (int i = 0; i < (int)L.size(); ++i)
            for (int j = 0; j < (int)R.size(); ++j) {
                Node::FP pt;
                pt.d.w = L[i].d.w + R[j].d.w;
                pt.d.h = std::max(L[i].d.h, R[j].d.h);
                pt.li = i; pt.ri = j; pt.impl = -1;
                cand.push_back(pt);
            }
    } else { // H
        for (int i = 0; i < (int)L.size(); ++i)
            for (int j = 0; j < (int)R.size(); ++j) {
                Node::FP pt;
                pt.d.w = std::max(L[i].d.w, R[j].d.w);
                pt.d.h = L[i].d.h + R[j].d.h;
                pt.li = i; pt.ri = j; pt.impl = -1;
                cand.push_back(pt);
            }
    }

    pareto_prune(cand);
    x->frontier_pts.swap(cand);
}

// returns index (in node->frontier_pts) of min-area point
static int pick_min_area_idx(const std::vector<Node::FP>& v) {
    long best_area = std::numeric_limits<long>::max();
    int best_idx = -1;
    for (int i = 0; i < (int)v.size(); ++i) {
        long area = 1L * v[i].d.w * v[i].d.h;
        if (area < best_area) {
            best_area = area;
            best_idx = i;
        }
    }
    return best_idx;
}

// Backtrack: choose sizes/impls along one optimal path and set Node::size bottom-up
static void backtrack_apply(Node* x, int idx) {
    if (!x) return;
    const Node::FP& here = x->frontier_pts[idx];
    x->size = here.d;

    if (x->t == NodeType::LEAF) {
        // here.impl already identifies which (w,h) we chose; x->size set above
        return;
    }

    // internal: recurse into children using recorded indices
    backtrack_apply(x->l, here.li);
    backtrack_apply(x->r, here.ri);
}

// --- Public API ---

Dim compute_optimal_room_size(Node* root) {
    build_frontier(root);
    int idx = pick_min_area_idx(root->frontier_pts);
    // pure query for #3 – do not mutate tree sizes (leave #1/#2 unaffected)
    return idx >= 0 ? root->frontier_pts[idx].d : Dim{0,0};
}

// Build frontier, pick min-area, APPLY choices (sizes) to the tree for #4
Dim compute_and_apply_optimal(Node* root) {
    build_frontier(root);
    int idx = pick_min_area_idx(root->frontier_pts);
    if (idx < 0) return Dim{0,0};
    backtrack_apply(root, idx);
    return root->frontier_pts[idx].d;
}
