#pragma once

#include <vector>
#include "../RStarTree/Point.h"
#include "../RStarTree/Rtree.h"

class KnnStrategy {
public:
    virtual ~KnnStrategy() = default;
    virtual std::vector<int> queryKNN(const Point& query, int k) const = 0;
};

class BruteForceKnn : public KnnStrategy {
private:
    const std::vector<Point>& data_;
public:
    BruteForceKnn(const std::vector<Point>& points) :  data_(points) {}
    std::vector<int> queryKNN(const Point& query, int k) const override;
};

class RStarTreeKNN : public KnnStrategy {
private:
    const Rtree& tree;

public:
    explicit RStarTreeKNN(const Rtree& t) : tree(t) {}

    std::vector<int> queryKNN(const Point& query, int k) const override;
};