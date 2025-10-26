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
    x->t = t; x->l = l; x->r = r; x->label = 0; x->size = {0,0}; x->x = x->y = 0;
    return x;
}

Node* new_leaf(int label, int w, int h) {
    Node* x = new Node{};
    x->t = NodeType::LEAF; x->l = x->r = nullptr;
    x->label = label; x->size = {w,h}; x->x = x->y = 0;
    return x;
}

char* trim(char* s) {
    while (*s && std::isspace(static_cast<unsigned char>(*s))) ++s;
    if (*s == 0) return s;
    char* e = s + std::strlen(s) - 1;
    while (e >= s && std::isspace(static_cast<unsigned char>(*e))) *e-- = '\0';
    return s;
}

// Parse label and FIRST "(w,h)" after "(("
void parse_label_and_first_wh(const char* line, int& out_label, int& out_w, int& out_h) {
    const char* s = line;

    // label
    while (*s && std::isspace(static_cast<unsigned char>(*s))) ++s;
    if (!std::isdigit(static_cast<unsigned char>(*s))) die("Leaf line missing label");
    char* end = nullptr;
    long label = std::strtol(s, &end, 10);
    if (end == s) die("Bad label");
    s = end;

    // find "(("
    const char* p = std::strstr(s, "((");
    if (!p) die("Leaf line missing \"((\"");
    p += 2;

    // width
    long w = std::strtol(p, &end, 10);
    if (end == p || *end != ',') die("Bad width/comma in first (w,h)");
    p = end + 1;

    // height
    long h = std::strtol(p, &end, 10);
    if (end == p || *end != ')') die("Bad height/\")\" in first (w,h)");

    out_label = static_cast<int>(label);
    out_w = static_cast<int>(w);
    out_h = static_cast<int>(h);
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
            int label, w, h;
            parse_label_and_first_wh(ln, label, w, h);
            Node* leaf = new_leaf(label, w, h);
            stk.push_back(leaf);
            leaves_out.push_back(leaf); // record input order
        }
    }
    std::fclose(fin);

    if (stk.size() != 1) {
        for (Node* n : stk) free_tree(n); // best-effort cleanup
        die("Malformed input: stack must end with exactly one node");
    }
    return stk.back();
}
