#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>
#include "Entry.h"

struct Node {
    static const int MAX_E = 4; // entradas máximas antes de hacer split
    static const int MIN_E = 2; // entradas mínimas después de split
    static const float REINSERT_PCT; // % de entradas que se reinsertan

    bool isLeaf; // true si es hoja
    int level; // nivel en el árbol (0 = hoja)
    std::vector<Entry> entries; // entradas de puntos o hijos
    Node* parent; // puntero al nodo padre
    Mbb cachedBoundingBox; // bounding box cacheado para evitar recálculos
    bool boundingBoxValid; // indica si el bounding box cacheado es válido

    explicit Node(bool leaf, int lvl = 0);

    void add(const Entry& e); // agrega una entrada al nodo
    void remove(const Entry& e); // remueve una entrada del nodo
    Mbb boundingBox() const; // calcula el MBB que cubre todas las entradas
    void invalidateBoundingBox(); // marca el bounding box como inválido
    void updateBoundingBox(); // actualiza el bounding box cacheado
};

#endif // NODE_H


