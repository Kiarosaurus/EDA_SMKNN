#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "RStarTree/Rtree.h"
#include "Knn_Interface/knn_strategy.h"
#include "Knn_Interface/knn_graph_builder.h"
#include "SMKNN/Splitt.h"
#include "SMKNN/Merge.h"
#include "Frontend/ExportVisualizationFiles.h"

using namespace std;

// Función para dividir una línea CSV
vector<string> splitCSVLine(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    
    while (getline(ss, token, ',')) {
        // Remueve espacios en blanco
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

// Función para parsear un vector de strings a floats
vector<float> parseFloatVector(const vector<string>& stringVector) {
    vector<float> result;
    result.reserve(stringVector.size());
    
    for (const auto& str : stringVector) {
        try {
            result.push_back(stof(str));
        } catch (const exception& e) {
            cerr << "Error al parsear valor: " << str << endl;
            throw;
        }
    }
    
    return result;
}

// Función para cargar datos de rostros desde CSV
vector<Point> loadFaceDataFromCSV(const string& filename) {
    vector<Point> faceData;
    ifstream file(filename);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + filename);
    }
    
    string line;
    int lineNumber = 0;
    
    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;
        
        vector<string> tokens = splitCSVLine(line);
        
        if (tokens.empty()) continue;
        
        try {
            vector<float> values;
            int subjectId = -1;
            
            // La última columna es el ID del sujeto
            subjectId = stoi(tokens.back());
            tokens.pop_back(); // Removemos la columna del sujeto
            
            values = parseFloatVector(tokens);
            
            if (!values.empty()) {
                Point point = Point::fromFaceData(values, subjectId);
                faceData.push_back(point);
            }
        } catch (const exception& e) {
            cerr << "Error en línea " << lineNumber << ": " << e.what() << endl;
            continue;
        }
    }
    
    file.close();
    cout << "Cargados " << faceData.size() << " puntos de rostros desde " << filename << endl;
    return faceData;
}

// Función para cargar datos sintéticos desde CSV (formato 2D)
vector<Point> loadSyntheticDataFromCSV(const string& filename) {
    vector<Point> syntheticData;
    ifstream file(filename);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + filename);
    }
    
    string line;
    int insertCount = 0;
    
    // Saltar la primera línea (encabezado)
    getline(file, line);
    
    while (getline(file, line)) {
        istringstream ss(line);
        string x_str, y_str, subject_str;
        
        if (!getline(ss, x_str, ',')) continue;
        if (!getline(ss, y_str, ',')) continue;
        if (!getline(ss, subject_str, ',')) continue;

        try {
            Point p(stof(x_str), stof(y_str));
            p.setId(insertCount++);
            syntheticData.push_back(p);
        } catch (const exception& e) {
            cerr << "Error al parsear línea: " << line << endl;
            continue;
        }
    }
    
    file.close();
    cout << "Cargados " << syntheticData.size() << " puntos sintéticos desde " << filename << endl;
    return syntheticData;
}

// Función para mostrar estadísticas de los datos
void printDataStats(const vector<Point>& data, const string& dataType) {
    if (data.empty()) {
        cout << "No hay datos para analizar." << endl;
        return;
    }
    
    size_t dims = data[0].dimensions();
    cout << "Estadísticas de datos " << dataType << ":" << endl;
    cout << "  - Número de puntos: " << data.size() << endl;
    cout << "  - Dimensiones: " << dims << endl;
    
    if (dims <= 5) {
        // Para datos de baja dimensionalidad, mostrar todas las dimensiones
        vector<float> minVals(dims, numeric_limits<float>::max());
        vector<float> maxVals(dims, numeric_limits<float>::lowest());
        vector<float> sums(dims, 0.0f);
        
        for (const auto& point : data) {
            for (size_t i = 0; i < dims; ++i) {
                minVals[i] = min(minVals[i], point[i]);
                maxVals[i] = max(maxVals[i], point[i]);
                sums[i] += point[i];
            }
        }
        
        cout << "  - Rango por dimensión:" << endl;
        for (size_t i = 0; i < dims; ++i) {
            float mean = sums[i] / data.size();
            cout << "    Dimensión " << i << ": [" << minVals[i] << ", " << maxVals[i] 
                 << "], Media: " << mean << endl;
        }
    } else {
        // Para datos de alta dimensionalidad, mostrar solo las primeras 5
        vector<float> minVals(5, numeric_limits<float>::max());
        vector<float> maxVals(5, numeric_limits<float>::lowest());
        vector<float> sums(5, 0.0f);
        
        for (const auto& point : data) {
            for (size_t i = 0; i < min(size_t(5), dims); ++i) {
                minVals[i] = min(minVals[i], point[i]);
                maxVals[i] = max(maxVals[i], point[i]);
                sums[i] += point[i];
            }
        }
        
        cout << "  - Rango por dimensión (primeras 5):" << endl;
        for (size_t i = 0; i < min(size_t(5), dims); ++i) {
            float mean = sums[i] / data.size();
            cout << "    Dimensión " << i << ": [" << minVals[i] << ", " << maxVals[i] 
                 << "], Media: " << mean << endl;
        }
        
        cout << "    ... y " << (dims - 5) << " dimensiones más" << endl;
    }
}

// Función para procesar datos con RStarTree y SMKNN
void processData(const vector<Point>& data, const string& dataType) {
    cout << "\n=== Procesando datos " << dataType << " ===" << endl;
    
    auto startTime = chrono::high_resolution_clock::now();
    
    // Mostrar estadísticas de los datos
    printDataStats(data, dataType);
    
    auto loadTime = chrono::high_resolution_clock::now();
    auto loadDuration = chrono::duration_cast<chrono::milliseconds>(loadTime - startTime);
    cout << "Tiempo de análisis de datos: " << loadDuration.count() << " ms" << endl;
    
    // Construir RStarTree
    cout << "\n--- Construyendo RStarTree ---" << endl;
    Rtree tree;
    for (const auto& point : data) {
        tree.insert(point);
    }
    
    auto treeTime = chrono::high_resolution_clock::now();
    auto treeDuration = chrono::duration_cast<chrono::milliseconds>(treeTime - loadTime);
    cout << "Tiempo de construcción del árbol: " << treeDuration.count() << " ms" << endl;
    
    // Calcular k automáticamente
    int k = max(1, (int)round(log(data.size())));
    cout << "Valor de k para k-NN: " << k << endl;
    
    // Construir grafo k-NN
    cout << "\n--- Construyendo grafo k-NN ---" << endl;
    RStarTreeKNN rstarKnn(tree);
    KNNGraphBuilder builder(data, rstarKnn, k, false);
    builder.construir();
    
    const auto& knnList = builder.getKNNList();
    const auto& A = builder.getAdyacencia();
    
    auto knnTime = chrono::high_resolution_clock::now();
    auto knnDuration = chrono::duration_cast<chrono::milliseconds>(knnTime - treeTime);
    cout << "Tiempo de construcción k-NN: " << knnDuration.count() << " ms" << endl;
    
    // Aplicar algoritmo SMKNN
    cout << "\n--- Aplicando algoritmo SMKNN ---" << endl;
    float threshold = 1.0f;
    SplitterSMKNN splitter(data, knnList, A, threshold);
    splitter.calcularRatios();
    splitter.identificarPivotes();
    splitter.removerPivotes();
    auto componentes = splitter.obtenerComponentes();
    
    cout << "Pivotes detectados: " << splitter.getPivotes().size() << endl;
    cout << "Componentes después de remover pivotes: " << componentes.size() << endl;
    
    // Mostrar información de componentes
    cout << "\n--- Información de componentes ---" << endl;
    for (size_t i = 0; i < componentes.size(); ++i) {
        cout << "Componente " << (i + 1) << ": " << componentes[i].size() << " puntos" << endl;
    }
    
    auto totalTime = chrono::high_resolution_clock::now();
    auto totalDuration = chrono::duration_cast<chrono::milliseconds>(totalTime - startTime);
    cout << "\nTiempo total de procesamiento: " << totalDuration.count() << " ms" << endl;
    
    // Ejemplo de búsqueda k-NN
    cout << "\n--- Ejemplo de búsqueda k-NN ---" << endl;
    if (!data.empty()) {
        Point queryPoint = data[0];  // Usar el primer punto como consulta
        int queryK = min(5, (int)data.size() - 1);
        
        auto searchStart = chrono::high_resolution_clock::now();
        vector<Point> neighbors = tree.kNearest(queryPoint, queryK);
        auto searchEnd = chrono::high_resolution_clock::now();
        auto searchDuration = chrono::duration_cast<chrono::microseconds>(searchEnd - searchStart);
        
        cout << "k-NN para punto " << queryPoint.getId() << " (k=" << queryK << "):" << endl;
        for (size_t i = 0; i < neighbors.size(); ++i) {
            float dist = Point::distance(queryPoint, neighbors[i]);
            cout << "  " << (i + 1) << ". Punto " << neighbors[i].getId() 
                 << " (distancia: " << fixed << setprecision(4) << dist << ")" << endl;
        }
        cout << "Tiempo de búsqueda: " << searchDuration.count() << " μs" << endl;

        // Guardar resultado k-NN en JSON solo para Olivetti
        if (dataType.find("Olivetti") != string::npos) {
            ofstream jsonOut("knn_result.json");
            jsonOut << "{\n";
            jsonOut << "  \"query\": " << queryPoint.getId() << ",\n";
            jsonOut << "  \"neighbors\": [";
            for (size_t i = 0; i < neighbors.size(); ++i) {
                jsonOut << neighbors[i].getId();
                if (i + 1 < neighbors.size()) jsonOut << ", ";
            }
            jsonOut << "]\n";
            jsonOut << "}";
            jsonOut.close();
            cout << "\nResultado k-NN guardado en knn_result.json para visualización." << endl;
        }
    }
    
    // Exportar visualización (solo para datos 2D)
    if (data[0].dimensions() <= 3) {
        cout << "\n--- Exportando visualización ---" << endl;
        try {
            exportVisualizationKnnGraph(data, A, splitter.getPivotes(), componentes);
            cout << "Archivos de visualización exportados exitosamente" << endl;
        } catch (const exception& e) {
            cout << "Error al exportar visualización: " << e.what() << endl;
        }
    } else {
        cout << "\n--- Visualización omitida (datos de alta dimensionalidad) ---" << endl;
    }
    
    // Mostrar ratios y pivotes
    const auto& ratios = splitter.getRatios();
    cout << "\n--- Local Distance Ratios r(x) (primeros 10) ---" << endl;
    for (int i = 0; i < min(10, (int)ratios.size()); ++i) {
        cout << "Punto " << i + 1 << ": r = " << ratios[i] << endl;
    }
    
    cout << "\n--- Pivotes detectados (r(x) < " << threshold << ") ---" << endl;
    for (int i : splitter.getPivotes()) {
        cout << "Punto " << i + 1 << " es pivote" << endl;
    }
}

int main() {
    cout << "=== RStarTree con Pruebas de Datos ===" << endl;
    cout << "Selecciona el tipo de datos a procesar:" << endl;
    cout << "1. Datos de rostros (Olivetti - 4096 dimensiones)" << endl;
    cout << "2. Datos sintéticos 2D (DS1.csv)" << endl;
    cout << "3. Datos sintéticos 2D (DS2.csv)" << endl;
    cout << "4. Datos sintéticos 2D (DS3.csv)" << endl;
    cout << "5. Datos sintéticos 2D (DS4.csv)" << endl;
    cout << "6. Datos sintéticos 2D (DS5.csv)" << endl;
    cout << "7. Datos sintéticos 2D (DS6.csv)" << endl;
    cout << "8. Datos sintéticos 2D (DS7.csv)" << endl;
    cout << "9. Datos sintéticos 2D (DS8.csv)" << endl;
    cout << "10. Datos sintéticos 2D (DS9.csv)" << endl;
    cout << "11. Datos sintéticos 2D (DS10.csv)" << endl;
    cout << "12. Procesar TODOS los datasets" << endl;
    
    int choice;
    cout << "\nIngresa tu elección (1-12): ";
    cin >> choice;
    
    try {
        if (choice == 1) {
            // Datos de rostros
            const string dataFile = "datasets/olivetti/olivetti.csv";
            cout << "\nProcesando datos de rostros..." << endl;
            vector<Point> faceData = loadFaceDataFromCSV(dataFile);
            if (!faceData.empty()) {
                processData(faceData, "de rostros (Olivetti)");
            }
        } else if (choice >= 2 && choice <= 11) {
            // Datos sintéticos
            string dataFile = "datasets/Synthetic/DS" + to_string(choice - 1) + ".csv";
            cout << "\nProcesando datos sintéticos..." << endl;
            vector<Point> syntheticData = loadSyntheticDataFromCSV(dataFile);
            if (!syntheticData.empty()) {
                processData(syntheticData, "sintéticos 2D (DS" + to_string(choice - 1) + ")");
            }
        } else if (choice == 12) {
            // Procesar todos los datasets
            cout << "\nProcesando TODOS los datasets..." << endl;
            
            // Datos de rostros
            try {
                vector<Point> faceData = loadFaceDataFromCSV("datasets/olivetti/olivetti.csv");
                if (!faceData.empty()) {
                    processData(faceData, "de rostros (Olivetti)");
                }
            } catch (const exception& e) {
                cout << "Error con datos de rostros: " << e.what() << endl;
            }
            
            // Datos sintéticos
            for (int i = 1; i <= 10; ++i) {
                try {
                    string dataFile = "datasets/Synthetic/DS" + to_string(i) + ".csv";
                    cout << "\n" << string(50, '=') << endl;
                    vector<Point> syntheticData = loadSyntheticDataFromCSV(dataFile);
                    if (!syntheticData.empty()) {
                        processData(syntheticData, "sintéticos 2D (DS" + to_string(i) + ")");
                    }
                } catch (const exception& e) {
                    cout << "Error con DS" << i << ": " << e.what() << endl;
                }
            }
        } else {
            cout << "Opción inválida. Ejecutando con datos de rostros por defecto." << endl;
            vector<Point> faceData = loadFaceDataFromCSV("datasets/olivetti/olivetti.csv");
            if (!faceData.empty()) {
                processData(faceData, "de rostros (Olivetti)");
            }
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
