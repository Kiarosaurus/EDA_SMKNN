#include "Node.h"

const float Node::REINSERT_PCT = 0.3f;

Node::Node(bool leaf, int lvl)
    : isLeaf(leaf), level(lvl), parent(nullptr) {}

void Node::add(const Entry& e) {
    entries.push_back(e);
    if (e.child) e.child->parent = this;
}

Mbb Node::boundingBox() const {
    if (entries.empty()) return Mbb();
    std::vector<Mbb> boxes;
    for (const auto& e : entries)
        boxes.push_back(e.box);
    return Mbb::combine(boxes);
}