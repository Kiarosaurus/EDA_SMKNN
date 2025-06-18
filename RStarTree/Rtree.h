#ifndef RTREE_H
#define RTREE_H

#pragma once
#include "Node.h"

class Rtree {
public:
    Rtree();
    void insert(const Point& p);
    std::vector<Point> rangeSearch(const Mbb& query) const;
    std::vector<Point> kNearest(const Point& query, int k) const;

private:
    Node* root;

    void insert(const Point& p, bool isReinserting);
    Node* chooseLeaf(Node* n, const Entry& e);
    void adjustTree(Node* n, Node* nn);
    Node* splitNode(Node* n);
    void reinsert(Node* n);

};

#endif // RTREE_H