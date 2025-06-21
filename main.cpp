#include <iostream>
#include <fstream>
#include <sstream>
#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
using namespace std;


void build_with_brute_force_knn(const vector<Point>& points, int k) {
    cout << "Construyendo grafo KNN con Brute Force...\n";
    BruteForceKnn bruteForceKnn(points);
    KNNGraphBuilder builder(points, bruteForceKnn, k, false); // mutual: A[]
    builder.construir();

    // const auto& A = builder.getAdyacencia();
    // for (int i = 0; i < A.size(); ++i) {
    //     cout << "Punto " << i+1 << ": ";
    //     for (int j = 0; j < A[i].size(); ++j) {
    //         if (A[i][j]) {
    //             cout << j+1 << " ";
    //         }
    //     }
    //     cout << endl;
    // }

    const auto& knnList = builder.getKNNList();
    cout << "Lista KNN:\n";
    for (int i = 0; i < knnList.size(); ++i) {
        cout << "KNN(" << i+1 << "): ";
        for (int j : knnList[i]) {
            cout << j+1 << " "; // Mostrar los índices de los vecinos
        }
        cout << endl;
    }
}

void build_with_rstar_tree_knn(const vector<Point>& points, int k) {
    cout << "Construyendo grafo KNN con R*Tree...\n";
    Rtree tree;
    for (const auto& p : points) {
        tree.insert(p);
    }
    RStarTreeKNN rstarKnn(tree);
    KNNGraphBuilder builder(points, rstarKnn, k, false); // mutual: A[]
    builder.construir();

    const auto& knnList = builder.getKNNList();
    cout << "Lista KNN:\n";
    for (int i = 0; i < knnList.size(); ++i) {
        cout << "KNN(" << i+1 << "): ";
        for (int j : knnList[i]) {
            cout << j+1 << " "; // Mostrar los índices de los vecinos
        }
        cout << endl;
    }
}

int main() {
    Rtree tree;

    ifstream in("../dataset.txt");
    if (!in) {
        cerr << "No se pudo abrir dataset.txt\n";
        return 1;
    }

    string line;
    int insertCount = 0;
    vector<Point> points;

    cout << "Insertando puntos desde dataset.txt:\n";
    while (getline(in, line)) {
        istringstream ss(line);
        float x, y;
        ss >> x >> y;
        Point p(x, y);
        p.setId(insertCount); // Asignar ID único
        tree.insert(p);
        points.push_back(p);
        // cout << p << endl; // Imprimir el punto insertado
        insertCount++;
    }

    cout << "\nTotal insertados: " << insertCount << endl<<endl;

    // Definir región de búsqueda
    // float x1 = 10, y1 = 10, x2 = 50, y2 = 50;
    // Mbb query(Point(x1, y1), Point(x2, y2));

    // vector<Point> found = tree.rangeSearch(query);

    // cout << "\nPuntos dentro del rango (" << x1 << "," << y1 << ") a (" << x2 << "," << y2 << "):\n";
    // for (auto& p : found)
    //     cout <<  p << " "<<endl;

    // cout << "\nTotal encontrados en rango: " << found.size() << "\n";

    // Ejemplo de búsqueda k-NN
    int n=0,k=2;
    // KNNStrategy* knnStrategy = nullptr;
    Point* p = nullptr;

    p = new Point(points[n]); // Crear un nuevo punto para la consulta KNN
    cout << "Punto de consulta KNN -> " << *p << endl;
    cout << "Buscando " << k << " vecinos mas cercanos...\n";

    build_with_brute_force_knn(points, k);
    cout<<endl;
    build_with_rstar_tree_knn(points, k);
    
    delete p; // Liberar memoria del punto de consulta
    // delete knnStrategy; // Liberar memoria del KNNStrategy
    return 0;
}