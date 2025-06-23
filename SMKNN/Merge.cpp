#include "Merge.h"
#include <cmath>
#include <limits>
#include <algorithm>

MergerSMKNN::MergerSMKNN(
    const std::vector<Point>& points_,
    const std::vector<std::vector<bool>>& ady_,
    const std::vector<std::vector<int>>& initialClusters,
    const std::unordered_set<int>& pivotes_,
    int finalK,
    float sigma_
)
: points(points_), adyacencia(ady_), clusters(initialClusters),
  pivotes(pivotes_), K(finalK), sigma(sigma_) { }

std::vector<int> MergerSMKNN::findPivotsBetween(int ci, int cj) {
    std::vector<int> pg;
    for (int v : pivotes) {
        bool e1=false, e2=false;
        for (int i : clusters[ci]) if (adyacencia[v][i]) { e1=true; break; }
        for (int j : clusters[cj]) if (adyacencia[v][j]) { e2=true; break; }
        if (e1 && e2) pg.push_back(v);
    }
    return pg;
}

std::vector<int> MergerSMKNN::findIntraNeighbors(
    const std::vector<int>& pg, const std::vector<int>& cluster) {

    std::unordered_set<int> inCluster(cluster.begin(), cluster.end());
    std::unordered_set<int> result;
    for (int v : pg) {
        for (int i : cluster)
            if (adyacencia[v][i])
                result.insert(i);
    }
    return { result.begin(), result.end() };
}

float MergerSMKNN::averageEdgeWeight(
    const std::vector<std::pair<int,int>>& edges) {

    if (edges.empty()) return 0.0f;
    float sum = 0;
    for (auto &e : edges)
        sum += Point::distance(points[e.first], points[e.second]);
    return sum / edges.size();
}

float MergerSMKNN::computeSimilarity(int ci, int cj) {
    auto pg = findPivotsBetween(ci, cj);
    if (pg.empty()) return 0.0f;

    // vecinos intra-cluster
    auto ng1 = findIntraNeighbors(pg, clusters[ci]);
    auto ng2 = findIntraNeighbors(pg, clusters[cj]);

    // construir conjuntos de aristas según Def.7 y Def.8
    std::vector<std::pair<int,int>> ext1, int1, ext2, int2;
    for (int v : pg) {
        for (int i : ng1)
            if (adyacencia[i][v]) ext1.emplace_back(i, v);
        for (int i : ng1)
            for (int u : clusters[ci])
                if (adyacencia[i][u] &&
                   std::find(ng1.begin(), ng1.end(), u)==ng1.end())
                    int1.emplace_back(i, u);

        for (int v2 : pg)
            for (int j : ng2)
                if (adyacencia[j][v2]) ext2.emplace_back(j, v2);
        for (int j : ng2)
            for (int u : clusters[cj])
                if (adyacencia[j][u] &&
                   std::find(ng2.begin(), ng2.end(), u)==ng2.end())
                    int2.emplace_back(j, u);
    }

    float aE1 = averageEdgeWeight(ext1),
          aI1 = averageEdgeWeight(int1),
          aE2 = averageEdgeWeight(ext2),
          aI2 = averageEdgeWeight(int2);

    float sim1 = std::exp(-std::pow(aE1 - aI1,2)/(2*sigma*sigma));
    float sim2 = std::exp(-std::pow(aE2 - aI2,2)/(2*sigma*sigma));

    return std::min(sim1, sim2);
}

void MergerSMKNN::run() {
    // merges sucesivos
    while ((int)clusters.size() > K) {
        float bestSim = -1;
        int bi=-1, bj=-1;
        for (int i = 0; i < (int)clusters.size(); ++i) {
            for (int j = i+1; j < (int)clusters.size(); ++j) {
                float sim = computeSimilarity(i,j);
                if (sim > bestSim) {
                    bestSim=sim; bi=i; bj=j;
                }
            }
        }
        if (bi<0) break;
        
        auto merged = clusters[bi];
        merged.insert(merged.end(),
                      clusters[bj].begin(), clusters[bj].end());
        clusters.erase(clusters.begin()+bj);
        clusters.erase(clusters.begin()+bi);
        clusters.push_back(merged);
    }

    for (int p : pivotes) {
        float bestD = std::numeric_limits<float>::infinity();
        int bestC = 0;
        for (int ci=0; ci<(int)clusters.size(); ++ci) {
            for (int i : clusters[ci]) {
                float d = Point::distance(points[p], points[i]);
                if (d < bestD) {
                    bestD = d;
                    bestC = ci;
                }
            }
        }
        clusters[bestC].push_back(p);
    }
}
