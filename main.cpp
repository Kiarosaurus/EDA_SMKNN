#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
#include "SMKNN/Splitt.h"
#include "Frontend/ExportVisualizationFiles.h"

using namespace std;


int main() {
    Rtree tree;
    ifstream in("dataset.txt");
    if (!in) {
        cerr << "No se pudo abrir dataset1.txt\n";
        return 1;
    }

    vector<Point> points;
    string line;
    int insertCount = 0;

    cout << "Insertando puntos desde dataset1.txt:\n";
    while (getline(in, line)) {
        istringstream ss(line);
        float x, y;
        ss >> x >> y;
        Point p(x, y);
        p.setId(insertCount++);
        tree.insert(p);
        points.push_back(p);
    }

    cout << "\nTotal insertados n: " << insertCount << "\n\n";

    // VALOR DE K log(n) del n puntos del dataset
    int k = std::round(std::log(insertCount));  // loge(n)
    if (k < 1) k = 1;
    cout << "Valor de k = log(n) : " << k << "\n";
    float threshold = 1.0;

    // Construcción KNN
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

    // SPLIT - SMKNN
    SplitterSMKNN splitter(points, knnList, A, threshold);
    splitter.calcularRatios();
    splitter.identificarPivotes();
    splitter.removerPivotes();
    auto componentes = splitter.obtenerComponentes();

    cout << "\n--- Local Distance Ratios r(x) ---\n";
    const auto& ratios = splitter.getRatios();
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

    // Exportar a JSON y mostrar animación
    exportVisualizationKnnGraph(points, builder.getAdyacencia(), splitter.getPivotes(), componentes);
    // Mostrar KNN sin animacion
    system("python frontend/render_knn_graph.py");
    //TIENEN QUE CERRAR LA PESTAÑÑA DEL RENDER ANTERIOR PARA VER EL SPLIT
    system("python frontend/render_knn_split.py");
    // Mostrar KNN split
    // Mostrar animación de construcción del grafo KNN (USAR CON UN DATASET PEQUEÑO, SINO LA COMPU SE LAGUEA :'v)
    // SOLO DEBEN PRESIONAR ENTER PARA VER LOS PIVOTES Y EL SPLIT DESPUÉS
    //system("python frontend/render_knn_graph_animated.py");

    return 0;
}
