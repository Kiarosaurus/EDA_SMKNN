#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>

#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
#include "SMKNN/Splitt.h"
#include "SMKNN/Merge.h"
#include "Frontend/ExportVisualizationFiles.h"

using namespace std;

// ============ Estructura para métricas ============
struct Metrics {
    float accuracy;
    float precision_macro;
    float recall_macro;
    float f1_macro;
};

float safe_div(float a, float b) {
    return b == 0 ? 0 : a / b;
}

Metrics compute_metrics(const vector<int>& pred, const vector<string>& etiquetas) {
    unordered_map<string, int> label_to_id;
    int next_id = 0;
    for (const auto& e : etiquetas)
        if (!label_to_id.count(e))
            label_to_id[e] = next_id++;

    int n = etiquetas.size();
    int k = label_to_id.size();

    vector<int> y_true(n);
    for (int i = 0; i < n; ++i)
        y_true[i] = label_to_id[etiquetas[i]];

    vector<vector<int>> confusion(k, vector<int>(k, 0));
    for (int i = 0; i < n; ++i)
        confusion[y_true[i]][pred[i]]++;

    float precision_sum = 0, recall_sum = 0, f1_sum = 0, correct = 0;
    for (int c = 0; c < k; ++c) {
        int tp = confusion[c][c];
        int fp = 0, fn = 0;
        for (int j = 0; j < k; ++j) {
            if (j != c) {
                fp += confusion[j][c];
                fn += confusion[c][j];
            }
        }
        float prec = safe_div(tp, tp + fp);
        float rec  = safe_div(tp, tp + fn);
        float f1   = safe_div(2 * prec * rec, prec + rec);

        precision_sum += prec;
        recall_sum += rec;
        f1_sum += f1;
        correct += tp;
    }

    Metrics m;
    m.accuracy = correct / n;
    m.precision_macro = precision_sum / k;
    m.recall_macro = recall_sum / k;
    m.f1_macro = f1_sum / k;
    return m;
}

// ============ Cargar CSV ============
vector<Point> loadCSV(const string& filename, vector<string>& etiquetas, unordered_set<string>& clases_unicas) {
    vector<Point> data;
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("No se pudo abrir: " + filename);

    string line;
    getline(file, line); // encabezado
    stringstream ss(line);
    vector<string> headers;
    string col;
    int class_index = -1, col_idx = 0;
    while (getline(ss, col, ',')) {
        headers.push_back(col);
        if (col == "class" or col == "Class"
            or col == "Cluster"
            or col == "Classes"
            or col == "Type_of_glass"
            or col == "survival_status"
            or col == "label"
            or col == "subject") class_index = col_idx;
        col_idx++;
    }

    if (class_index == -1)
        throw runtime_error("No se encontró la columna 'class' en el CSV.");

    int id = 0;
    while (getline(file, line)) {
        stringstream ls(line);
        vector<string> tokens;
        string tok;
        while (getline(ls, tok, ',')) tokens.push_back(tok);

        if (tokens.size() != headers.size()) {
            cerr << "⚠️ Fila con columnas incompletas (esperado " << headers.size() << ", obtenido " << tokens.size() << "). Saltando.\n";
            continue;
        }

        vector<float> coords;
        bool fila_valida = true;
        for (int i = 0; i < tokens.size(); ++i) {
            if (i == class_index) continue;
            try {
                coords.push_back(stof(tokens[i]));
            } catch (...) {
                cerr << "⚠️ Error en 'stof': valor='" << tokens[i] << "' (col " << i << ", fila " << id << ")\n";
                fila_valida = false;
                break;
            }
        }

        if (!fila_valida || coords.empty()) continue;

        string clase = tokens[class_index];
        etiquetas.push_back(clase);
        clases_unicas.insert(clase);

        Point p(coords);
        p.setId(id++);
        data.push_back(p);
    }

    cout << "Cargados " << data.size() << " puntos desde " << filename << endl;
    return data;
}

// ============ MAIN ============
int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Uso: " << argv[0] << " <csv_path> <k> <threshold> <finalK>\n";
        return 1;
    }

    string csv_path = argv[1];
    int k = stoi(argv[2]);
    float threshold = stof(argv[3]);
    int finalK = stoi(argv[4]);

    try {
        vector<string> etiquetas;
        unordered_set<string> clases_unicas;
        vector<Point> points = loadCSV(csv_path, etiquetas, clases_unicas);

        if (k <= 0) k = max(1, (int)round(log(points.size())));
        if (finalK <= 0) finalK = (int)clases_unicas.size();

        cout << "Usando k = " << k << ", threshold = " << threshold << ", finalK = " << finalK << endl;

        Rtree tree;
        for (auto& p : points)
            tree.insert(p);

        RStarTreeKNN knn(tree);
        KNNGraphBuilder builder(points, knn, k, false);
        builder.construir();

        auto& knnList = builder.getKNNList();
        auto& A = builder.getAdyacencia();

        SplitterSMKNN splitter(points, knnList, A, threshold);
        splitter.calcularRatios();
        splitter.identificarPivotes();
        splitter.removerPivotes();
        auto componentes = splitter.obtenerComponentes();

        exportVisualizationKnnGraph(points, A, splitter.getPivotes(), componentes);

        MergerSMKNN merger(points, A, componentes, splitter.getPivotes(), finalK);
        merger.run();
        auto& clusters = merger.getClusters();
        exportVisualizationKnnGraph(points, A, splitter.getPivotes(), clusters);

        cout << "Etiquetas detectadas: " << etiquetas.size() << " | Clases únicas: " << clases_unicas.size() << endl;

        if (!etiquetas.empty()) {
            vector<int> predicted(points.size());
            for (int cid = 0; cid < clusters.size(); ++cid)
                for (int pid : clusters[cid])
                    predicted[pid] = cid;

            Metrics m = compute_metrics(predicted, etiquetas);
            cout << "🔍 Evaluación:\n";
            cout << " - Accuracy      (AC): " << m.accuracy << '\n';
            cout << " - Precision     (PR): " << m.precision_macro << '\n';
            cout << " - Recall        (RE): " << m.recall_macro << '\n';
            cout << " - F1 Score      (F1): " << m.f1_macro << '\n';
        }

    } catch (const exception& e) {
        cerr << "❌ Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
