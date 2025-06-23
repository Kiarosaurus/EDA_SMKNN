#ifndef MERGER_SMKNN_H
#define MERGER_SMKNN_H

#pragma once
#include <vector>
#include <unordered_set>
#include "../RStarTree/Point.h"

class MergerSMKNN {
private:
    const std::vector<Point>& points;
    std::vector<std::vector<bool>> adyacencia;
    std::vector<std::vector<int>> clusters;
    std::unordered_set<int> pivotes;
    int K; 
    float sigma;

    std::vector<int> findPivotsBetween(int ci, int cj);

    std::vector<int> findIntraNeighbors(const std::vector<int>& pg,
                                        const std::vector<int>& cluster);

    float averageEdgeWeight(const std::vector<std::pair<int,int>>& edges);

    float computeSimilarity(int ci, int cj);

public:
    MergerSMKNN(
      const std::vector<Point>& points_,
      const std::vector<std::vector<bool>>& ady_,
      const std::vector<std::vector<int>>& initialClusters,
      const std::unordered_set<int>& pivotes_,
      int finalK,
      float sigma_ = 1.0f / std::sqrt(2.0f)
    );
    void run();

    const std::vector<std::vector<int>>& getClusters() const { return clusters; }
};

#endif // MERGER_SMKNN_H
