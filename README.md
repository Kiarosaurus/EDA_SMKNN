
# EDA Proyect: Implementación de Split–Merge Clustering based on KNN (SMKNN)

Este proyecto tiene como objetivo la implementación desde cero del algoritmo **SMKNN**, tal como se detalla en el paper original adjuntado en la bibliografía.

---

# Etapa 1: Construcción de KNN

## 1.1 Implementación del Árbol R\*

- [x] Diseñar la estructura básica del **R\*-tree**.
- [x] Programar métodos básicos inserción, búsqueda de rango.
- [x] Verificar funcionamiento con datos sintéticos simples.

## 1.2 Implementación del k-NN Algorithm

- [x] Implementar el **algoritmo de búsqueda de k-vecinos más cercanos** apoyado en R\*-tree.
- [x] Permitir extracción de la matriz de adyacencia (Definición 1).
- [ ] Obtención de los vecinos conectados (Definición 2).
- [x] Validar que los k-NN sean simétricos si se usa KNN mutuo.

## 1.3 Pruebas Iniciales en el `main`

- [x] Leer puntos desde archivo.
- [x] Mostrar los k-neighbors más cercanos de cada punto.
- [x] Validar correctitud de la matriz de adyacencia del grafo KNN con data.
- [ ] Dibujar imagen de puntos iniciales con bordes del grafo KNN - en blanco y negro.
- [ ] Verificar la correctitud del grafo visualmente y numéricamente.

# Etapa 2: Fase de Splitting

## 2.1 Implementación
- [ ] Implementar el cálculo del **local distance ratio** (Definición 3).
- [ ] Identificar puntos pivote (`r(x) < thr`, Definición 4).
- [ ] Remover pivotes y obtener subgrafos resultantes.
- [ ] Implementar algoritmo de detección de componentes conexos.

## 2.2 Testeo
- [ ] Dibujar subgrafos resultantes luego del split (sin pivotes) - a color.
- [ ] Confirmar que los subgrafos representan clusters coherentes tras eliminar pivotes.

# Etapa 3: Fase de Merging

## 3.1 Implementación
- [ ] Implementar detección de pivotes entre clusters (Definición 5).
- [ ] Calcular vecinos intra-cluster de pivotes (Definición 6).
- [ ] Calcular bordes internos y externos (Definiciones 7 y 8).
- [ ] Calcular similitud entre clusters (Definición 9, Ecuación 22).
- [ ] Realizar merges sucesivos hasta obtener K clusters.
- [ ] Asignar pivotes restantes al cluster más cercano.

## 3. Evaluación, Visualización y Pruebas Finales
- [ ] Dibujar clusters finales luego del merge - a color.
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
