#ifndef RTREE_H
#define RTREE_H

#pragma once
#include "Node.h"

class Rtree {
public:
    Rtree(); // crea el árbol con raíz vacía
    void insert(const Point& p); // inserta un punto en el árbol
    std::vector<Point> rangeSearch(const Mbb& query) const; // búsqueda de rango

private:
    Node* root; // raíz del árbol

    Node* chooseLeaf(Node* n, const Entry& e); // elige hoja donde insertar
    void adjustTree(Node* n); // actualiza MBBs hacia arriba
    void splitNode(Node* n); // divide un nodo lleno
    void reinsert(Node* n); // reinsertar algunas entradas
};

#endif // RTREE_H
