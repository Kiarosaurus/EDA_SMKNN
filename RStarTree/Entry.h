#ifndef ENTRY_H
#define ENTRY_H

#include "Mbb.h"

struct Node;

struct Entry {
    Mbb box; // bounding box del punto o del hijo
    Node* child; // si no es nullptr, apunta a un nodo hijo (interno)
    Point data; // si child es nullptr, este es un dato (punto real)

    Entry(const Point& p);
    Entry(Node* c, const Mbb& b);
};

#endif // ENTRY_H
