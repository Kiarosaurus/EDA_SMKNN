#include "Splitt.h"
#include <queue>
#include <limits>

SplitterSMKNN::SplitterSMKNN(
    const std::vector<Point>& points_,
    const std::vector<std::unordered_set<int>>& knn_list_,
    const std::vector<std::vector<bool>>& adyacencia_,
    float threshold_
)
    : points(points_), knn_list(knn_list_), adyacencia(adyacencia_), threshold(threshold_) {}


float SplitterSMKNN::calcularKDistancia(int idx) {
    const Point& p = points[idx];
    std::vector<float> distancias;
    for (int neighbor : knn_list[idx])
        distancias.push_back(Point::distance(p, points[neighbor]));

    if (distancias.size() < knn_list[idx].size()) return 0;
    std::sort(distancias.begin(), distancias.end());
    return distancias.back();
}


void SplitterSMKNN::calcularRatios() {
    int n = points.size();
    localDistanceRatios.assign(n, 0.0);

    for (int i = 0; i < n; ++i) {
        std::vector<int> vecinos;
        for (int j = 0; j < n; ++j)
            if (adyacencia[i][j]) vecinos.push_back(j);

        if (vecinos.empty()) continue;

        float sum_kd = 0;
        for (int j : vecinos)
            sum_kd += calcularKDistancia(j);

        float dk_i = calcularKDistancia(i);
        if (dk_i != 0)
            localDistanceRatios[i] = sum_kd / (vecinos.size() * dk_i);
        else
            localDistanceRatios[i] = std::numeric_limits<float>::max();
    }
}


void SplitterSMKNN::identificarPivotes() {
    pivotes.clear();
    for (int i = 0; i < localDistanceRatios.size(); ++i)
        if (localDistanceRatios[i] < threshold)
            pivotes.insert(i);
}


void SplitterSMKNN::removerPivotes() {
    for (int i : pivotes)
        for (int j = 0; j < adyacencia.size(); ++j)
            adyacencia[i][j] = adyacencia[j][i] = false;
}


std::vector<std::vector<int>> SplitterSMKNN::obtenerComponentes() {
    int n = points.size();
    std::vector<bool> visitado(n, false);
    std::vector<std::vector<int>> componentes;

    for (int i = 0; i < n; ++i) {
        if (visitado[i] || pivotes.count(i)) continue;

        std::vector<int> componente;
        std::queue<int> q;
        q.push(i);
        visitado[i] = true;

        while (!q.empty()) {
            int u = q.front(); q.pop();
            componente.push_back(u);

            for (int v = 0; v < n; ++v) {
                if (adyacencia[u][v] && !visitado[v] && !pivotes.count(v)) {
                    visitado[v] = true;
                    q.push(v);
                }
            }
        }
        componentes.push_back(componente);
    }

    return componentes;
}

const std::unordered_set<int>& SplitterSMKNN::getPivotes() const { return pivotes; }
const std::vector<float>& SplitterSMKNN::getRatios() const { return localDistanceRatios; }
