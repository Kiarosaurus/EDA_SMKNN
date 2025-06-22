


#include <vector>
// Exporta puntos, aristas, pivotes y componentes a un JSON para render.py
void exportVisualizationKnnGraph(

    const std::vector<Point>& points,
    const vector<vector<bool>>& adjMatrix,
    const unordered_set<int>& pivotes,
    const vector<vector<int>>& componentes
) {
    ofstream out("visualization.json");
    out << "{\n";

    out << "\"points\": [\n";
    for (int i = 0; i < points.size(); ++i) {
        out << "[" << points[i][0] << "," << points[i][1] << "]";
        if (i + 1 < points.size()) out << ",\n";
    }
    out << "],\n";

    out << "\"edges\": [\n";
    bool first = true;
    for (int i = 0; i < adjMatrix.size(); ++i)
        for (int j = i + 1; j < adjMatrix[i].size(); ++j)
            if (adjMatrix[i][j]) {
                if (!first) out << ",\n";
                out << "[" << i << "," << j << "]";
                first = false;
            }
    out << "],\n";

    out << "\"pivotes\": [";
    for (auto it = pivotes.begin(); it != pivotes.end(); ++it) {
        out << *it;
        if (next(it) != pivotes.end()) out << ",";
    }
    out << "],\n";

    out << "\"componentes\": [\n";
    for (int i = 0; i < componentes.size(); ++i) {
        out << "[";
        for (int j = 0; j < componentes[i].size(); ++j) {
            out << componentes[i][j];
            if (j + 1 < componentes[i].size()) out << ",";
        }
        out << "]";
        if (i + 1 < componentes.size()) out << ",";
        out << "\n";
    }
    out << "]\n";

    out << "}\n";
    out.close();
}