#include "Rtree.h"
#include <algorithm>
#include <queue>
#include <random>

Rtree::Rtree() { root = new Node(true, 0); }

void Rtree::insert(const Point& p) {
    Entry e(p);
    Node* leaf = chooseLeaf(root, e);
    leaf->add(e);

    if ((int)leaf->entries.size() > Node::MAX_E) {
        reinsert(leaf); // R*-Tree: intento de reinserción
        if ((int)leaf->entries.size() > Node::MAX_E)
            splitNode(leaf);
    }
    adjustTree(leaf);
}

Node* Rtree::chooseLeaf(Node* n, const Entry& e) {
    if (n->isLeaf) return n;

    Node* best = nullptr;
    float bestInc = std::numeric_limits<float>::max();
    float bestArea = std::numeric_limits<float>::max();

    for (auto& childEntry : n->entries) {
        float inc = childEntry.box.enlarge(e.box);
        float area = childEntry.box.area();
        if (inc < bestInc || (inc == bestInc && area < bestArea)) {
            bestInc = inc;
            bestArea = area;
            best = childEntry.child;
        }
    }

    return chooseLeaf(best, e);
}

void Rtree::adjustTree(Node* n) {
    while (n != root) {
        n = n->parent;
        for (auto& e : n->entries)
            e.box = e.child->boundingBox();
    }

    // Si la raíz también se desborda, crear nueva raíz
    if ((int)root->entries.size() > Node::MAX_E) {
        splitNode(root);
        Node* newRoot = new Node(false, root->level + 1);
        newRoot->add(Entry(root, root->boundingBox()));
        Node* sibling = root->parent; // creado en split
        newRoot->add(Entry(sibling, sibling->boundingBox()));
        root = newRoot;
    }
}

void Rtree::splitNode(Node* n) {
    // Elegir eje con mayor extensión (x o y) y dividir a la mitad
    auto dx = n->boundingBox().upper_right.x - n->boundingBox().lower_left.x;
    auto dy = n->boundingBox().upper_right.y - n->boundingBox().lower_left.y;
    auto axis = dx > dy ? 0 : 1;

    std::sort(n->entries.begin(), n->entries.end(),
              [axis](const Entry& a, const Entry& b) {
                  return axis ? a.box.lower_left.y < b.box.lower_left.y
                              : a.box.lower_left.x < b.box.lower_left.x;
              });

    Node* sibling = new Node(n->isLeaf, n->level);
    int splitPoint = n->entries.size() / 2;
    sibling->entries.assign(n->entries.begin() + splitPoint, n->entries.end());
    n->entries.erase(n->entries.begin() + splitPoint, n->entries.end());

    if (n->parent == nullptr)
        n->parent = new Node(false, n->level + 1);

    n->parent->add(Entry(sibling, sibling->boundingBox()));
}

void Rtree::reinsert(Node* n) {
    if (n == root) return; // no reinserción en raíz

    const int k = (int)(Node::REINSERT_PCT * Node::MAX_E);

    // Centro aproximado del nodo
    Point center(
        (n->boundingBox().lower_left.x + n->boundingBox().upper_right.x) / 2,
        (n->boundingBox().lower_left.y + n->boundingBox().upper_right.y) / 2
    );

    std::vector<std::pair<float, Entry>> dist;
    for (auto& e : n->entries) {
        float cx = (e.box.lower_left.x + e.box.upper_right.x) / 2;
        float cy = (e.box.lower_left.y + e.box.upper_right.y) / 2;
        float dx = cx - center.x;
        float dy = cy - center.y;
        dist.push_back({dx * dx + dy * dy, e});
    }

    std::sort(dist.begin(), dist.end(),
              [](auto& a, auto& b) { return a.first > b.first; });

    std::vector<Entry> toReinsert;
    toReinsert.reserve(k);

    for (int i = 0; i < k && !dist.empty(); ++i) {
        toReinsert.push_back(dist.front().second);
        n->entries.erase(std::find_if(n->entries.begin(), n->entries.end(),
            [&](const Entry& en) {
                return &en == &dist.front().second;
            }));
        dist.erase(dist.begin());
    }

    // Reinsertar desde la raíz
    for (auto& e : toReinsert)
        insert(e.data);
}

std::vector<Point> Rtree::rangeSearch(const Mbb& q) const {
    std::vector<Point> res;
    std::queue<const Node*> Q;
    Q.push(root);

    while (!Q.empty()) {
        const Node* n = Q.front(); Q.pop();
        for (const auto& e : n->entries) {
            if (!e.box.intersects(q)) continue;
            if (n->isLeaf) res.push_back(e.data);
            else Q.push(e.child);
        }
    }

    return res;
}
