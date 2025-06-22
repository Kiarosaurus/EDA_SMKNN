#ifndef SPLITT_H
#define SPLITT_H

#pragma once
#include <vector>
#include <unordered_set>
#include "../RStarTree/Rtree.h"

class SplitterSMKNN {
private:
    const std::vector<Point>& points;
    const std::vector<std::unordered_set<int>>& knn_list;
    std::vector<std::vector<bool>> adyacencia;
    float threshold;

    std::vector<float> localDistanceRatios;
    std::unordered_set<int> pivotes;

    float calcularKDistancia(int pIndex);

public:
    SplitterSMKNN(
        const std::vector<Point>& points_,
        const std::vector<std::unordered_set<int>>& knn_list_,
        const std::vector<std::vector<bool>>& adyacencia_,
        float threshold_ = 1.0f
    );

    void calcularRatios();
    void identificarPivotes();
    void removerPivotes();
    std::vector<std::vector<int>> obtenerComponentes();

    const std::unordered_set<int>& getPivotes() const;
    const std::vector<float>& getRatios() const;
};


#endif //SPLITT_H
