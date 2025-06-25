#include "Node.h"

const float Node::REINSERT_PCT = 0.3f;

Node::Node(bool leaf, int lvl)
    : isLeaf(leaf), level(lvl), parent(nullptr), boundingBoxValid(false) {}

void Node::add(const Entry& e) {
    entries.push_back(e);
    if (e.child) e.child->parent = this;
    
    // Actualizar bounding box incrementalmente
    if (boundingBoxValid) {
        cachedBoundingBox.expand(e.box);
    } else {
        invalidateBoundingBox();
    }
}

void Node::remove(const Entry& e) {
    // Encontrar y remover la entrada
    auto it = std::find_if(entries.begin(), entries.end(),
                          [&e](const Entry& entry) {
                              return entry.box == e.box && entry.child == e.child;
                          });
    if (it != entries.end()) {
        entries.erase(it);
        invalidateBoundingBox(); // El bounding box necesita ser recalculado
    }
}

Mbb Node::boundingBox() const {
    if (entries.empty()) return Mbb();
    
    // Si el bounding box cacheado es válido, usarlo
    if (boundingBoxValid) {
        return cachedBoundingBox;
    }
    
    // Calcular el bounding box desde cero
    if (entries.size() == 1) {
        const_cast<Node*>(this)->cachedBoundingBox = entries[0].box;
        const_cast<Node*>(this)->boundingBoxValid = true;
        return cachedBoundingBox;
    }
    
    // Combinar todos los bounding boxes
    std::vector<Mbb> boxes;
    boxes.reserve(entries.size());
    for (const auto& e : entries) {
        boxes.push_back(e.box);
    }
    
    const_cast<Node*>(this)->cachedBoundingBox = Mbb::combine(boxes);
    const_cast<Node*>(this)->boundingBoxValid = true;
    return cachedBoundingBox;
}

void Node::invalidateBoundingBox() {
    boundingBoxValid = false;
    // Propagar la invalidación hacia arriba
    if (parent) {
        parent->invalidateBoundingBox();
    }
}

void Node::updateBoundingBox() {
    if (!boundingBoxValid) {
        boundingBox(); // Esto actualizará el cache
    }
}