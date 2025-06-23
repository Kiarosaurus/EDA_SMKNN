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
#include "Frontend/ExportVisualizationFiles.h"

using namespace std;

int main() {
    Rtree tree;
    ifstream in("DS1.csv");
    if (!in) {
        cerr << "No se pudo abrir DS1.csv\n";
        return 1;
    }

    vector<Point> points;
    string line;
    int insertCount = 0;

    // Saltar encabezado: x,y,subject
    getline(in, line);

    cout << "Insertando puntos desde DS1.csv:\n";
    while (getline(in, line)) {
        istringstream ss(line);
        string x_str, y_str, subject_str;
        if (!getline(ss, x_str, ',')) continue;
        if (!getline(ss, y_str, ',')) continue;
        if (!getline(ss, subject_str, ',')) continue;

        float x = stof(x_str);
        float y = stof(y_str);

        Point p(x, y);
        p.setId(insertCount++);
        tree.insert(p);
        points.push_back(p);
    }

    cout << "\nTotal insertados n: " << insertCount << "\n\n";

    int k = std::round(std::log(insertCount));
    if (k < 1) k = 1;
    cout << "Valor de k = log(n) : " << k << "\n";
    float threshold = 1.0;

    RStarTreeKNN rstarKnn(tree);
    KNNGraphBuilder builder(points, rstarKnn, k, false);
    builder.construir();

    const auto& knnList = builder.getKNNList();
    const auto& A = builder.getAdyacencia();

    cout << "Lista KNN:\n";
    for (int i = 0; i < knnList.size(); ++i) {
        cout << "KNN(" << i + 1 << "): ";
        for (int j : knnList[i]) cout << j + 1 << " ";
        cout << "\n";
    }

    cout << "\nMatriz de adyacencia A[i][j]:\n";
    for (int i = 0; i < A.size(); ++i) {
        cout << "Punto " << i + 1 << ": ";
        for (int j = 0; j < A[i].size(); ++j)
            if (A[i][j]) cout << j + 1 << " ";
        cout << "\n";
    }

    SplitterSMKNN splitter(points, knnList, A, threshold);
    splitter.calcularRatios();
    splitter.identificarPivotes();
    splitter.removerPivotes();
    auto componentes = splitter.obtenerComponentes();

    const auto& ratios = splitter.getRatios();
    cout << "\n--- Local Distance Ratios r(x) ---\n";
    for (int i = 0; i < ratios.size(); ++i)
        cout << "Punto " << i + 1 << ": r = " << ratios[i] << "\n";

    cout << "\n--- Pivotes detectados (r(x) < " << threshold << ") ---\n";
    for (int i : splitter.getPivotes())
        cout << "Punto " << i + 1 << " es pivote\n";

    cout << "\n--- Componentes conectados (subgrafos) tras remover pivotes ---\n";
    int compId = 1;
    for (const auto& comp : componentes) {
        cout << "Componente " << compId++ << ": ";
        for (int i : comp)
            cout << i + 1 << " ";
        cout << "\n";
    }

    exportVisualizationKnnGraph(points, builder.getAdyacencia(), splitter.getPivotes(), componentes);
    system("python frontend/render_knn_graph.py");
    system("python frontend/render_knn_split.py");

    return 0;
}
