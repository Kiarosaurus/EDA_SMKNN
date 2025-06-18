#include "knn_strategy.h"
#include <queue>
#include <algorithm>

std::vector<int> BruteForceKnn::queryKNN(const Point& query, int k) const {
    using DistId = std::pair<float, int>;
    std::priority_queue<DistId> maxHeap;

    for (int i = 0; i < data_.size(); ++i) {
        float dist = Point::distance(query, data_[i]);
        if (maxHeap.size() < (size_t)k) {
            maxHeap.emplace(dist, i);
        } else if (dist < maxHeap.top().first) {
            maxHeap.pop();
            maxHeap.emplace(dist, i);
        }
    }

    std::vector<int> resultado;
    while (!maxHeap.empty()) {
        resultado.push_back(maxHeap.top().second);
        maxHeap.pop();
    }

    std::reverse(resultado.begin(), resultado.end());
    return resultado;
}

std::vector<int> RStarTreeKNN::queryKNN(const Point& query, int k) const {
    std::vector<Point> vecinos = tree.kNearest(query, k);
    std::vector<int> ids;
    ids.reserve(vecinos.size());
    for (const auto& p : vecinos)
        ids.push_back(p.getId()); // ← Point debe tener atributo id
    return ids;
}
