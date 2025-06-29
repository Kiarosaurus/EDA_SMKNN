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

vector<Point> loadCSV(const string& filename) {
    vector<Point> data;
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("No se pudo abrir: " + filename);

    string line;
    getline(file, line); // encabezado

    int id = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string x_str, y_str, _id;
        if (!getline(ss, x_str, ',')) continue;
        if (!getline(ss, y_str, ',')) continue;
        Point p(stof(x_str), stof(y_str));
        p.setId(id++);
        data.push_back(p);
    }
    cout << "Cargados " << data.size() << " puntos desde " << filename << endl;
    return data;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Uso: " << argv[0] << " <csv_path> <k> <threshold> <finalK>\n";
        return 1;
    }

    string csv_path = argv[1];
    int k           = stoi(argv[2]);
    float threshold = stof(argv[3]);
    int finalK      = stoi(argv[4]);

    try {
        vector<Point> points = loadCSV(csv_path);
        Rtree tree;
        for (const auto& p : points) tree.insert(p);

        if (k <= 0) k = max(1, (int)round(log(points.size())));
        cout << "Usando k = " << k << ", threshold = " << threshold << ", finalK = " << finalK << endl;

        RStarTreeKNN rstarKnn(tree);
        KNNGraphBuilder builder(points, rstarKnn, k, false);
        builder.construir();

        const auto& knnList = builder.getKNNList();
        const auto& A = builder.getAdyacencia();

        SplitterSMKNN splitter(points, knnList, A, threshold);
        splitter.calcularRatios();
        splitter.identificarPivotes();
        splitter.removerPivotes();
        auto componentes = splitter.obtenerComponentes();

        exportVisualizationKnnGraph(points, A, splitter.getPivotes(), componentes);

        if (finalK <= 0) finalK = static_cast<int>(componentes.size());
        MergerSMKNN merger(points, A, componentes, splitter.getPivotes(), finalK);
        merger.run();
        const auto& finalClusters = merger.getClusters();
        exportVisualizationKnnGraph(points, A, splitter.getPivotes(), finalClusters);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
