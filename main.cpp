#include <iostream>
#include <fstream>
#include <sstream>
#include "RStarTree/Rtree.h"

int main() {
    Rtree tree;

    std::ifstream in("dataset.txt");
    if (!in) {
        std::cerr << "No se pudo abrir dataset.txt\n";
        return 1;
    }

    std::string line;
    int insertCount = 0;

    std::cout << "Insertando puntos desde dataset.txt:\n";
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        float x, y;
        ss >> x >> y;
        std::cout << "(" << x << ", " << y << ")\n";  // <-- imprime cada punto insertado
        tree.insert(Point(x, y));
        insertCount++;
    }

    std::cout << "\nTotal insertados: " << insertCount << "\n";

    // Definir región de búsqueda
    float x1 = 10, y1 = 10, x2 = 50, y2 = 50;
    Mbb query(Point(x1, y1), Point(x2, y2));

    std::vector<Point> found = tree.rangeSearch(query);

    std::cout << "\nPuntos dentro del rango (" << x1 << "," << y1 << ") a (" << x2 << "," << y2 << "):\n";
    for (auto& p : found)
        std::cout << "(" << p.x << ", " << p.y << ")\n";

    std::cout << "\nTotal encontrados en rango: " << found.size() << "\n";

    return 0;
}
