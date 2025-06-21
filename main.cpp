#include <iostream>
#include <fstream>
#include <sstream>
#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
using namespace std;

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
    int n=103,k=5;
    KNNStrategy* knnStrategy = nullptr;
    Point* p = nullptr;

    p = new Point(points[n]); // Crear un nuevo punto para la consulta KNN
    cout << "Punto de consulta KNN -> " << *p << endl;
    cout << "Buscando " << k << " vecinos mas cercanos...\n";

    cout<<"R*Tree KNN:\n";
    knnStrategy = new RStarTreeKNN(tree);
    vector<int> ids_1 = knnStrategy->queryKNN(*p, k);
    for (int id : ids_1) cout << points[id] << endl; // Imprimir los puntos encontrados por el KNN
     

    // cout << "\nKNN Brute Force:\n";
    // knnStrategy = new BruteForceKnn(points);
    // vector<int> ids_2 = knnStrategy->queryKNN(*p, k);
    // for (int id : ids_2) cout << points[id] << endl; // Imprimir los puntos encontrados por el KNN
    
    
    delete knnStrategy;


    // Construir el grafo KNN
    cout << "\nConstruyendo grafo KNN...\n";
    KNNGraphBuilder builder(points, *knnStrategy, k); // mutual = true
    builder.construir();  // 👈 ¡IMPORTANTE!

    const auto& A = builder.getAdyacencia();

    for (int i = 0; i < A.size(); ++i) {
        std::cout << "Vecinos de " << i << ": ";
        for (int j = 0; j < A[i].size(); ++j) {
            if (A[i][j])
                std::cout << j << " ";
        }
        std::cout << "\n";
    }

    delete knnStrategy; // Liberar memoria del KNNStrategy
    return 0;
}