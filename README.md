
# EDA Proyect: Implementación de Split–Merge Clustering based on KNN (SMKNN)

Este proyecto tiene como objetivo la implementación desde cero del algoritmo **SMKNN**, tal como se detalla en el paper original adjuntado en la bibliografía.

---

## 1. Previas y estructuras

### 1.1 Implementación del Árbol R\*

- [x] Diseñar la estructura básica del **R\*-tree** (inserción, búsqueda de rango).
- [x] Verificar funcionamiento con datos sintéticos simples.

### 1.2 Implementación del k-NN Algorithm

- [ ] Implementar el **algoritmo de búsqueda de k-vecinos más cercanos** apoyado en R\*-tree.
- [ ] Permitir extracción de la matriz de adyacencia según la Definición 1 del paper.
- [ ] Validar que los k-NN sean simétricos si se usa KNN mutuo.

### 1.3 Pruebas Iniciales en el `main`

- [ ] Leer puntos desde archivo o generar datos sintéticos.
- [ ] Mostrar los k-neighbors más cercanos de cada punto.
- [ ] Validar correctitud de la matriz de adyacencia del grafo KNN.

---

## 2. Implementación del Algoritmo SMKNN

### 2.1 Etapa 1: Construcción del Grafo KNN

- [ ] Construir grafo KNN con pesos basados en **distancia euclidiana**.
- [ ] Aplicar la Definición 1 (matriz de adyacencia) y Definición 2 (vecinos conectados).
- [ ] Verificar el grafo visualmente y numéricamente.
- [ ] Dibujar imagen de puntos iniciales con bordes del grafo KNN - en blanco y negro.

### 2.2 Etapa 2: Fase de Splitting

- [ ] Implementar el cálculo del **local distance ratio** (Definición 3).
- [ ] Identificar puntos pivote (`r(x) < thr`, Definición 4).
- [ ] Remover pivotes y obtener subgrafos resultantes.
- [ ] Implementar algoritmo de detección de componentes conexos.
- [ ] Dibujar subgrafos resultantes luego del split (sin pivotes) - a color.

### 2.3 Etapa 3: Fase de Merging

- [ ] Implementar detección de pivotes entre clusters (Definición 5).
- [ ] Calcular vecinos intra-cluster de pivotes (Definición 6).
- [ ] Calcular bordes internos y externos (Definiciones 7 y 8).
- [ ] Calcular similitud entre clusters (Definición 9, Ecuación 22).
- [ ] Realizar merges sucesivos hasta obtener K clusters.
- [ ] Asignar pivotes restantes al cluster más cercano.
- [ ] Dibujar clusters finales luego del merge - a color.

---

## 3. Evaluación, Visualización y Pruebas Finales
- [ ] Validar visualmente el algoritmo con los datasets sintéticos (Two Moons, Blobs, Spirals, etc.).
- [ ] Validar el algoritmo con los datasets reales  usando Accuracy (AC), Precision (PR), Recall (RE) y F1-measure (F1).
- [ ] Realizar análisis de sensibilidad para `k`, `thr` y `σ`.

---

## 4. Ejecución del Proyecto :D
Se tiene previsto que la ejecución sea.... mmm... algo así:

```bash
g++ -std=c++17 main.cpp -o smknn
./smknn < dataset.txt
```

# Bibliografía
- Beckman, N., Kriegel, H., Schneider, R. \& Seeger, B. (1990). The R*-tree: an efficient and robust access method for points and rectangles. *Proceedings of the 1990 ACM SIGMOD International Conference on Management of Data*, 322 - 331. [https://doi.org/10.1145/93597.98741](https://doi.org/10.1145/93597.98741)
- Wang, Y., Ma, Y., Huang, H., Wang, B. \& Prasanna, D. (2022). A split–merge clustering algorithm based on the k-nearest neighbor graph. *Information Systems, 11*(102124), 1-24. [https://doi.org/10.1016/j.is.2022.102124](https://doi.org/10.1016/j.is.2022.102124)
