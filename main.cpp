#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <cmath>

#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
#include "SMKNN/Splitt.h"
#include "SMKNN/Merge.h"
#include "Frontend/ExportVisualizationFiles.h"

using namespace std;

int main() {
    const string dataFile = "2spiral.csv";
    Rtree tree;
    ifstream in(dataFile);
    if (!in) {
        cerr << "No se pudo abrir " << dataFile << '\n';
        return 1;
    }

    vector<Point> points;
    string line;
    int insertCount = 0;

    getline(in, line);
    while (getline(in, line)) {
        istringstream ss(line);
        string x_str, y_str, subject_str;
        if (!getline(ss, x_str, ',')) continue;
        if (!getline(ss, y_str, ',')) continue;
        if (!getline(ss, subject_str, ',')) continue;

        Point p(stof(x_str), stof(y_str));
        p.setId(insertCount++);
        tree.insert(p);
        points.push_back(p);
    }

    cout << "\nTotal insertados n: " << insertCount << "\n";

    
    int k = std::round(std::log(insertCount));
    if (k < 1) k = 1;
    cout << "Valor de k = log(n): " << k << '\n';

    float threshold = 1.0f; 

    RStarTreeKNN      rstarKnn(tree);
    KNNGraphBuilder   builder(points, rstarKnn, k, /*directed=*/false);
    builder.construir();

    const auto& knnList = builder.getKNNList();
    const auto& A       = builder.getAdyacencia();

    SplitterSMKNN splitter(points, knnList, A, threshold);
    splitter.calcularRatios();
    splitter.identificarPivotes();
    splitter.removerPivotes();
    auto componentes = splitter.obtenerComponentes();


    exportVisualizationKnnGraph(points, A, splitter.getPivotes(), componentes);
    system("python3 frontend/render_knn_graph.py");
    system("python3 frontend/render_knn_split.py");

    system("python3 frontend/render_knn_intercluster_edges.py");

    int K_final;
    cout << "\nNúmero deseado de clusters finales K (≥2). "
            "Si presionas 0 usaré el #componentes actual → "
         << componentes.size() << ": ";
    if (!(cin >> K_final) || K_final <= 0)
        K_final = static_cast<int>(componentes.size());


    MergerSMKNN merger(points, A, componentes,
                       splitter.getPivotes(), K_final);
    merger.run();
    const auto& finalClusters = merger.getClusters();

    cout << "\n--- Clusters finales tras el merge ---\n";
    int cid = 1;
    for (const auto& c : finalClusters) {
        cout << "Cluster " << cid++ << ": ";
        for (int idx : c) cout << idx + 1 << ' ';
        cout << '\n';
    }
    exportVisualizationKnnGraph(points, A, splitter.getPivotes(),
                                finalClusters);
    system("python3 frontend/render_knn_merge.py");

    
    const auto& ratios = splitter.getRatios();
    cout << "\n--- Local Distance Ratios r(x) ---\n";
    for (int i = 0; i < ratios.size(); ++i)
        cout << "Punto " << i + 1 << ": r = " << ratios[i] << '\n';

    cout << "\n--- Pivotes detectados (r(x) < " << threshold << ") ---\n";
    for (int i : splitter.getPivotes())
        cout << "Punto " << i + 1 << " es pivote\n";

    cout << "\n--- Componentes tras remover pivotes ---\n";
    int compId = 1;
    for (const auto& comp : componentes) {
        cout << "Componente " << compId++ << ": ";
        for (int i : comp) cout << i + 1 << ' ';
        cout << '\n';
    }

    return 0;
}
