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

        // Actualizar el bounding box del nodo actual en el padre
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
    // Usar el bounding box cacheado si está disponible
    Mbb nodeBox = node->boundingBox();
    
    // Encuentra el eje con mayor extensión para la división
    size_t dims = nodeBox.dimensions();
    int bestAxis = 0;
    float maxExtent = 0.0f;
    
    for (size_t i = 0; i < dims; ++i) {
        float extent = nodeBox.upper_right[i] - nodeBox.lower_left[i];
        if (extent > maxExtent) {
            maxExtent = extent;
            bestAxis = i;
        }
    }

    std::sort(node->entries.begin(), node->entries.end(),
              [bestAxis](const Entry& a, const Entry& b) {
                  return a.box.lower_left[bestAxis] < b.box.lower_left[bestAxis];
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

    // Invalidar bounding boxes de ambos nodos
    node->invalidateBoundingBox();
    sibling->invalidateBoundingBox();

    return sibling;
}

// Reinserta las entradas más alejadas del centro del nodo
void Rtree::reinsert(Node* node) {
    if (node == root) return;

    int k = (int)(Node::REINSERT_PCT * Node::MAX_E);
    std::vector<std::pair<float, Entry>> distanceEntryPairs;
    distanceEntryPairs.reserve(node->entries.size());

    // Usar el bounding box cacheado
    Mbb nodeBox = node->boundingBox();
    
    // Calcula el centro del nodo en todas las dimensiones
    size_t dims = nodeBox.dimensions();
    vector<float> center_coords(dims);
    for (size_t i = 0; i < dims; ++i) {
        center_coords[i] = (nodeBox.lower_left[i] + nodeBox.upper_right[i]) / 2;
    }
    Point center(center_coords);

    for (const auto& entry : node->entries) {
        // Calcula el centro de la entrada en todas las dimensiones
        vector<float> entry_center_coords(dims);
        for (size_t i = 0; i < dims; ++i) {
            entry_center_coords[i] = (entry.box.lower_left[i] + entry.box.upper_right[i]) / 2;
        }
        Point entry_center(entry_center_coords);
        
        // Calcula la distancia cuadrática al centro
        float dist2 = Point::distance2(center, entry_center);
        distanceEntryPairs.push_back({dist2, entry});
    }

    std::sort(distanceEntryPairs.begin(), distanceEntryPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<Entry> toReinsert;
    toReinsert.reserve(k);
    for (int i = 0; i < k && i < distanceEntryPairs.size(); ++i) {
        toReinsert.push_back(distanceEntryPairs[i].second);
    }

    // Limpiar y reinsertar las entradas restantes
    node->entries.clear();
    for (size_t i = k; i < distanceEntryPairs.size(); ++i) {
        node->add(distanceEntryPairs[i].second);
    }

    // Reinsertar las entradas más alejadas
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


std::vector<Point> Rtree::kNearest(const Point& query, int k) const {
    // Nodo de la cola de búsqueda: almacena puntero a subárbol y dist^2 MBB→query
    struct NodeDist {
        const Node* node;
        float dist2;
    };
    auto cmpNode = [](const NodeDist& a, const NodeDist& b) {
        return a.dist2 > b.dist2;  // menor dist2 primero
    };
    std::priority_queue<NodeDist,
                        std::vector<NodeDist>,
                        decltype(cmpNode)> pq(cmpNode);

    // Inicializamos recorriendo desde la raíz
    pq.push({ root, root->boundingBox().minDist(query) });

    // Estructura para guardar los k mejores candidatos (dist^2, id)
    struct PointDist {
        Point pt;
        float dist2;
    };
    auto cmpPoint = [](const PointDist& a, const PointDist& b) {
        // Queremos que el peor (mayor dist2, o en empate mayor id) esté arriba
        if (a.dist2 != b.dist2)
            return a.dist2 < b.dist2;
        return a.pt.getId() < b.pt.getId();
    };
    std::priority_queue<PointDist,
                        std::vector<PointDist>,
                        decltype(cmpPoint)> best(cmpPoint);

    // radio2 = dist^2 al peor de los k vecinos actuales
    float radius2 = std::numeric_limits<float>::infinity();

    while (!pq.empty()) {
        auto nd = pq.top(); 
        pq.pop();

        // Poda: si este subárbol está más lejos que nuestro peor candidato, cortamos
        if (nd.dist2 > radius2) 
            break;

        const Node* node = nd.node;
        if (node->isLeaf) {
            // Comparamos cada punto de la hoja
            for (auto& entry : node->entries) {
                // 1) Evita devolverte a ti mismo
                if (entry.data.getId() == query.getId()) continue;

                // 2) Calcula dist^2 real
                float d2 = Point::distance2(query, entry.data);

                if (best.size() < (size_t)k) {
                    // Aún no tenemos k vecinos: añadimos directamente
                    best.push({ entry.data, d2 });
                    if (best.size() == (size_t)k)
                        radius2 = best.top().dist2;
                } 
                else {
                    // Ya hay k: solo sustituimos si es mejor
                    auto worst = best.top();
                    if (d2 < worst.dist2 ||
                        (d2 == worst.dist2 && entry.data.getId() < worst.pt.getId())) {
                        best.pop();
                        best.push({ entry.data, d2 });
                        radius2 = best.top().dist2;
                    }
                }
            }
        } 
        else {
            // Nodo interno: propagamos a hijos cuyo MBB esté dentro del radio
            for (auto& entry : node->entries) {
                float childDist2 = entry.box.minDist(query);
                if (childDist2 <= radius2)
                    pq.push({ entry.child, childDist2 });
            }
        }
    }

    // Extraemos resultados ordenados ascendentemente (menor dist^2 primero)
    std::vector<Point> result;
    result.reserve(best.size());
    while (!best.empty()) {
        result.push_back(best.top().pt);
        best.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
