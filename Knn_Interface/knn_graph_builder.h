#pragma once

#include "knn_strategy.h"
#include <vector>
#include <unordered_set>

class KNNGraphBuilder {
private:
    const std::vector<Point>& puntos;
    const KNNStrategy& knn;
    int k;
    bool mutual;

    std::vector<std::unordered_set<int>> knn_list; // KNN(i)
    std::vector<std::vector<bool>> adyacencia;     // A[i][j] = 1 si i-j conectados

public:
    KNNGraphBuilder(const std::vector<Point>& puntos_, const KNNStrategy& knn_, int k_, bool mutual_ = false)
        : puntos(puntos_), knn(knn_), k(k_), mutual(mutual_) {}

    void construir() {
        int n = puntos.size();
        knn_list.resize(n);
        adyacencia.assign(n, std::vector<bool>(n, false));

        // Paso 1: obtener lista KNN(i)
        for (int i = 0; i < n; ++i) {
            auto vecinos = knn.queryKNN(puntos[i], k);
            knn_list[i].insert(vecinos.begin(), vecinos.end());
        }

        // Paso 2: construir A[i][j] según Definición 1
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;

                bool iVecinoDeJ = knn_list[i].count(j);
                bool jVecinoDeI = knn_list[j].count(i);

                if (mutual) {
                    if (iVecinoDeJ && jVecinoDeI)
                        adyacencia[i][j] = adyacencia[j][i] = true;
                } else {
                    if (iVecinoDeJ || jVecinoDeI)
                        adyacencia[i][j] = adyacencia[j][i] = true;
                }
            }
        }
    }

    const std::vector<std::unordered_set<int>>& getKNNList() const { return knn_list; }
    const std::vector<std::vector<bool>>& getAdyacencia() const { return adyacencia; }
};
