#include "pa2.hpp"
#include <algorithm>
#include <limits>

// Dominance: a dominates b if a.w <= b.w && a.h <= b.h and (a != b)
static bool dominates(const Dim& a, const Dim& b) {
    return (a.w <= b.w && a.h <= b.h) && (a.w < b.w || a.h < b.h);
}

// Remove dominated points; keep non-dominated "skyline"
static void pareto_prune(std::vector<Dim>& v) {
    if (v.empty()) return;
    // Sort by w asc, then h asc
    std::sort(v.begin(), v.end(), [](const Dim& a, const Dim& b){
        if (a.w != b.w) return a.w < b.w;
        return a.h < b.h;
    });
    // Remove duplicates
    v.erase(std::unique(v.begin(), v.end(), [](const Dim& a, const Dim& b){
        return a.w == b.w && a.h == b.h;
    }), v.end());

    // Classic skyline: keep strictly decreasing h as w increases
    std::vector<Dim> out;
    out.reserve(v.size());
    int best_h = std::numeric_limits<int>::max();
    for (const Dim& p : v) {
        if (p.h < best_h) {
            out.push_back(p);
            best_h = p.h;
        }
        // else dominated by an earlier point with <= w and <= h
    }
    v.swap(out);
}

// Combine two frontiers under a V node
static std::vector<Dim> combine_V(const std::vector<Dim>& L, const std::vector<Dim>& R) {
    std::vector<Dim> out;
    out.reserve(L.size() * R.size());
    for (const Dim& a : L)
        for (const Dim& b : R)
            out.push_back(Dim{ a.w + b.w, std::max(a.h, b.h) });
    pareto_prune(out);
    return out;
}

// Combine two frontiers under an H node
static std::vector<Dim> combine_H(const std::vector<Dim>& L, const std::vector<Dim>& R) {
    std::vector<Dim> out;
    out.reserve(L.size() * R.size());
    for (const Dim& a : L)
        for (const Dim& b : R)
            out.push_back(Dim{ std::max(a.w, b.w), a.h + b.h });
    pareto_prune(out);
    return out;
}

// Build frontier bottom-up
static void build_frontier(Node* x) {
    if (!x) return;
    if (x->t == NodeType::LEAF) {
        x->frontier = x->impls;      // all (w,h) are feasible
        pareto_prune(x->frontier);   // prune leaf duplicates / dominated (rare but safe)
        return;
    }
    build_frontier(x->l);
    build_frontier(x->r);
    if (x->t == NodeType::V) {
        x->frontier = combine_V(x->l->frontier, x->r->frontier);
    } else { // H
        x->frontier = combine_H(x->l->frontier, x->r->frontier);
    }
}

// Public entry: compute best (W,H) by minimum area at root
Dim compute_optimal_room_size(Node* root) {
    build_frontier(root);
    // pick the (W,H) with minimum W*H (tie-break arbitrary)
    long best_area = std::numeric_limits<long>::max();
    Dim best{0,0};
    for (const Dim& p : root->frontier) {
        long area = 1L * p.w * p.h;
        if (area < best_area) { best_area = area; best = p; }
    }
    return best;
}
