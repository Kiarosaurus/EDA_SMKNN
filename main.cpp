#include <iostream>
#include <fstream>
#include <sstream>
#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
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
        cout << p << endl; // Imprimir el punto insertado
        insertCount++;
    }

    cout << "\nTotal insertados: " << insertCount << "\n";

    // Definir región de búsqueda
    // float x1 = 10, y1 = 10, x2 = 50, y2 = 50;
    // Mbb query(Point(x1, y1), Point(x2, y2));

    // vector<Point> found = tree.rangeSearch(query);

    // cout << "\nPuntos dentro del rango (" << x1 << "," << y1 << ") a (" << x2 << "," << y2 << "):\n";
    // for (auto& p : found)
    //     cout <<  p << " "<<endl;

    // cout << "\nTotal encontrados en rango: " << found.size() << "\n";

    // Ejemplo de búsqueda k-NN
    int k = 5;
    Point q(40, 40);
    // auto knn = tree.kNearest(q, k);
    cout << "\n" << k << " vecinos mas cercanos a (40,40):\n";
    // for (auto& p : knn)
    //     cout << p << endl;

    vector<int> ids = RStarTreeKNN(tree).queryKNN(q, k);
    for (int id : ids) {
        cout << points[id]<<endl; // Imprimir los puntos encontrados por el KNN>>
    }

    vector<int> idsBrute = BruteForceKnn(points).queryKNN(q, k);
    cout << "\nKNN Brute Force:\n";
    for (int id : idsBrute) {
        cout << points[id] << endl; // Imprimir los puntos encontrados por el KNN
    }
    return 0;
}
