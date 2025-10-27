#include "pa2.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

[[noreturn]] void die(const char* msg) {
    std::fprintf(stderr, "%s\n", msg);
    std::exit(1);
}

Node* new_internal(NodeType t, Node* l, Node* r) {
    Node* x = new Node{};
    x->t = t; x->l = l; x->r = r; x->label = 0;
    x->size = {0,0}; x->x = x->y = 0;
    return x;
}

Node* new_leaf(int label, const std::vector<Dim>& impls) {
    Node* x = new Node{};
    x->t = NodeType::LEAF; x->l = x->r = nullptr;
    x->label = label; x->x = x->y = 0;
    x->impls = impls;
    // First impl drives Output #1/#2 sizes
    if (!impls.empty()) x->size = impls.front();
    return x;
}

char* trim(char* s) {
    while (*s && std::isspace(static_cast<unsigned char>(*s))) ++s;
    if (*s == 0) return s;
    char* e = s + std::strlen(s) - 1;
    while (e >= s && std::isspace(static_cast<unsigned char>(*e))) *e-- = '\0';
    return s;
}

int parse_label(const char*& s) {
    while (*s && std::isspace(static_cast<unsigned char>(*s))) ++s;
    if (!std::isdigit(static_cast<unsigned char>(*s))) die("Leaf line missing label");
    char* end = nullptr;
    long label = std::strtol(s, &end, 10);
    if (end == s) die("Bad label");
    s = end;
    return static_cast<int>(label);
}

// Parse all "(w,h)" inside the "((" ... "))" list (robust to whitespace).
std::vector<Dim> parse_all_impls(const char* s_after_label) {
    // Find the first '(' after the label: list opener.
    const char* q = std::strchr(s_after_label, '(');
    if (!q) die("Leaf line missing '(' after label");

    ++q;                 // after first '('
    if (*q == '(') ++q;  // if we have "((", move inside first pair to the first digit

    auto skip_ws = [](const char*& p) {
        while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
    };
    auto parse_int = [&](const char*& p) -> long {
        skip_ws(p);
        char* end = nullptr;
        long v = std::strtol(p, &end, 10);
        if (end == p) die("Expected integer in (w,h)");
        p = end;
        return v;
    };

    std::vector<Dim> out;

    while (true) {
        skip_ws(q);

        // End of list? (final ')')
        if (*q == ')') {
            ++q; // consume list-closing ')'
            break;
        }

        // Parse a pair. For the first pair we may already be at the first digit;
        // for subsequent pairs we expect an explicit '('.
        if (*q == '(') ++q;

        long w = parse_int(q);

        skip_ws(q);
        if (*q != ',') die("Expected ',' in (w,h)");
        ++q;

        long h = parse_int(q);

        skip_ws(q);
        if (*q != ')') die("Expected ')' after (w,h)");
        ++q; // close this pair

        out.push_back(Dim{ static_cast<int>(w), static_cast<int>(h) });

        // Next: either another '(' (next pair) or ')' (close list) or whitespace.
        skip_ws(q);
        if (*q == '(' || *q == ')') {
            // handled in next loop iteration
            continue;
        }
        // Otherwise, advance to next significant token.
        while (*q && *q != '(' && *q != ')') ++q;
    }

    if (out.empty()) die("Leaf has empty implementation list");
    return out;
}

} // namespace

Node* parse_postorder_file(const char* path, std::vector<Node*>& leaves_out) {
    std::FILE* fin = std::fopen(path, "r");
    if (!fin) { std::perror(path); return nullptr; }

    std::vector<Node*> stk;
    stk.reserve(256);

    char buf[8192];
    while (std::fgets(buf, sizeof(buf), fin)) {
        char* ln = trim(buf);
        if (*ln == '\0') continue;

        if (ln[0]=='V' && ln[1]=='\0') {
            if (stk.size() < 2) die("Malformed input: V needs two children");
            Node* r = stk.back(); stk.pop_back();
            Node* l = stk.back(); stk.pop_back();
            stk.push_back(new_internal(NodeType::V, l, r));
        } else if (ln[0]=='H' && ln[1]=='\0') {
            if (stk.size() < 2) die("Malformed input: H needs two children");
            Node* r = stk.back(); stk.pop_back();
            Node* l = stk.back(); stk.pop_back();
            stk.push_back(new_internal(NodeType::H, l, r));
        } else {
            const char* s = ln;
            int label = parse_label(s);
            std::vector<Dim> impls = parse_all_impls(s);
            Node* leaf = new_leaf(label, impls);
            stk.push_back(leaf);
            leaves_out.push_back(leaf); // keep input order
        }
    }
    std::fclose(fin);

    if (stk.size() != 1) {
        for (Node* n : stk) free_tree(n);
        die("Malformed input: stack must end with exactly one node");
    }
    return stk.back();
}
