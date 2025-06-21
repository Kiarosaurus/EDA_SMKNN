#include "Rtree.h"
#include <algorithm>
#include <queue>
#include <random>

// Constructor: crea una raíz vacía que es hoja
Rtree::Rtree() {
    root = new Node(true, 0);
}

// Inserción pública
void Rtree::insert(const Point& point) {
    insert(point, false);
}

// Inserta un punto; si hay overflow intenta reinserción o división
void Rtree::insert(const Point& point, bool isReinserting) {
    Entry entry(point);
    Node* leafNode = chooseLeaf(root, entry);
    leafNode->add(entry);

    Node* newNode = nullptr;

    if ((int)leafNode->entries.size() > Node::MAX_E) {
        if (!isReinserting) {
            reinsert(leafNode);
        }
        if ((int)leafNode->entries.size() > Node::MAX_E) {
            newNode = splitNode(leafNode);
        }
    }

    adjustTree(leafNode, newNode);
}

// Elige hoja adecuada para insertar una entrada
Node* Rtree::chooseLeaf(Node* node, const Entry& entry) {
    if (node->isLeaf) return node;

    Node* bestChild = nullptr;
    float bestIncrease = std::numeric_limits<float>::max();
    float bestArea = std::numeric_limits<float>::max();

    for (auto& childEntry : node->entries) {
        float increase = childEntry.box.enlarge(entry.box);
        float area = childEntry.box.area();
        if (increase < bestIncrease || (increase == bestIncrease && area < bestArea)) {
            bestIncrease = increase;
            bestArea = area;
            bestChild = childEntry.child;
        }
    }

    return chooseLeaf(bestChild, entry);
}

// Ajusta los nodos desde la hoja hacia arriba
void Rtree::adjustTree(Node* node, Node* newNode) {
    while (node != root) {
        Node* parent = node->parent;

        for (auto& entry : parent->entries) {
            if (entry.child == node) {
                entry.box = node->boundingBox();
                break;
            }
        }

        if (newNode) {
            newNode->parent = parent;
            parent->add(Entry(newNode, newNode->boundingBox()));
        }

        if ((int)parent->entries.size() > Node::MAX_E) {
            Node* splitParent = splitNode(parent);
            node = parent;
            newNode = splitParent;
        } else {
            newNode = nullptr;
            node = parent;
        }
    }

    if (newNode) {
        Node* newRoot = new Node(false, root->level + 1);
        root->parent = newRoot;
        newNode->parent = newRoot;
        newRoot->add(Entry(root, root->boundingBox()));
        newRoot->add(Entry(newNode, newNode->boundingBox()));
        root = newRoot;
    }
}

// Divide un nodo en dos hermanos
Node* Rtree::splitNode(Node* node) {
    auto dx = node->boundingBox().upper_right[0] - node->boundingBox().lower_left[0];
    auto dy = node->boundingBox().upper_right[1] - node->boundingBox().lower_left[1];
    int axis = dx > dy ? 0 : 1;

    std::sort(node->entries.begin(), node->entries.end(),
              [axis](const Entry& a, const Entry& b) {
                  return axis ? a.box.lower_left[1] < b.box.lower_left[1]
                              : a.box.lower_left[0] < b.box.lower_left[0];
              });

    Node* sibling = new Node(node->isLeaf, node->level);
    int mid = node->entries.size() / 2;
    sibling->entries.assign(node->entries.begin() + mid, node->entries.end());
    node->entries.erase(node->entries.begin() + mid, node->entries.end());

    if (!node->isLeaf) {
        for (auto& entry : sibling->entries) {
            entry.child->parent = sibling;
        }
    }

    return sibling;
}

// Reinserta las entradas más alejadas del centro del nodo
void Rtree::reinsert(Node* node) {
    if (node == root) return;

    int k = (int)(Node::REINSERT_PCT * Node::MAX_E);
    std::vector<std::pair<float, Entry>> distanceEntryPairs;
    distanceEntryPairs.reserve(node->entries.size());

    Point center(
        (node->boundingBox().lower_left[0] + node->boundingBox().upper_right[0]) / 2,
        (node->boundingBox().lower_left[1] + node->boundingBox().upper_right[1]) / 2
    );

    for (const auto& entry : node->entries) {
        float cx = (entry.box.lower_left[0] + entry.box.upper_right[0]) / 2;
        float cy = (entry.box.lower_left[1] + entry.box.upper_right[1]) / 2;
        float dx = cx - center[0];
        float dy = cy - center[1];
        distanceEntryPairs.push_back({dx * dx + dy * dy, entry});
    }

    std::sort(distanceEntryPairs.begin(), distanceEntryPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<Entry> toReinsert;
    toReinsert.reserve(k);
    for (int i = 0; i < k && i < distanceEntryPairs.size(); ++i) {
        toReinsert.push_back(distanceEntryPairs[i].second);
    }

    node->entries.clear();
    for (size_t i = k; i < distanceEntryPairs.size(); ++i) {
        node->add(distanceEntryPairs[i].second);
    }

    for (auto& entry : toReinsert) {
        insert(entry.data, true);
    }
}

// Busca puntos dentro de una región rectangular (MBB)
std::vector<Point> Rtree::rangeSearch(const Mbb& queryBox) const {
    std::vector<Point> result;
    std::queue<const Node*> queue;
    queue.push(root);

    while (!queue.empty()) {
        const Node* node = queue.front(); queue.pop();
        if (node == nullptr) continue;

        for (const auto& entry : node->entries) {
            if (!entry.box.intersects(queryBox)) continue;

            if (node->isLeaf) {
                if (queryBox.intersects(entry.box)) {
                    result.push_back(entry.data);
                }
            } else {
                queue.push(entry.child);
            }
        }
    }

    return result;
}


// Búsqueda k-NN usando un recorrido "best first" del R*-tree
std::vector<Point> Rtree::kNearest(const Point& query, int k) const {
    // Cola de prioridad para explorar nodos ordenados por distancia
    struct NodeDist {
        const Node* node; // nodo a procesar
        float dist;       // distancia mínima de su MBB al query
    };
    auto cmpNode = [](const NodeDist& a, const NodeDist& b) { return a.dist > b.dist; };
    std::priority_queue<NodeDist, std::vector<NodeDist>, decltype(cmpNode)> pq(cmpNode);
    // comenzamos con la raíz
    pq.push({root, root->boundingBox().minDist(query)});

    // Mantiene los k mejores puntos encontrados hasta el momento
    struct PointDist {
        Point pt;
        float dist;
    };
    auto cmpPoint = [](const PointDist& a, const PointDist& b) { return a.dist < b.dist; };
    std::priority_queue<PointDist, std::vector<PointDist>, decltype(cmpPoint)> best(cmpPoint);

    // Distancia cuadrada al peor de los mejores vecinos encontrados
    float radius2 = std::numeric_limits<float>::infinity();
    float radius = std::numeric_limits<float>::infinity(); // solo para reportar si se desea

    while (!pq.empty()) {
        NodeDist nd = pq.top();
        pq.pop();

        // Si el nodo más cercano está más lejos que el radio actual, terminamos
        if (nd.dist > radius2)
            break;

        const Node* node = nd.node;
        if (node->isLeaf) {
            // Comparar cada punto en la hoja
            for (const auto& entry : node->entries) {
                if (entry.data.getId() == query.getId()) continue; // Evita el mismo punto

                float d = Point::distance(query, entry.data);
                if (best.size() < (size_t)k) {
                    best.push({entry.data, d});
                    // Una vez alcanzados k, el top tiene el peor dist
                    if (best.size() == (size_t)k) {
                        radius = best.top().dist;
                        radius2 = radius * radius;
                    }
                } else if (d < best.top().dist) {
                    best.pop();
                    best.push({entry.data, d});
                    radius = best.top().dist;
                    radius2 = radius * radius;
                }
            }
        } else {
            // Añade hijos potencialmente útiles
            for (const auto& entry : node->entries) {
                float d = entry.box.minDist(query);
                if (d <= radius2)
                    pq.push({entry.child, d});
            }
        }
    }

    // Extraer los puntos ordenados por distancia ascendente
    std::vector<Point> res;
    res.reserve(best.size());
    while (!best.empty()) {
        res.push_back(best.top().pt);
        best.pop();
    }
    std::reverse(res.begin(), res.end());
    return res;
}
