import json
import matplotlib.pyplot as plt

# Leer archivo JSON generado por C++
with open("frontend/visualization.json", "r") as f:
    data = json.load(f)

points = data["points"]
edges = data["edges"]
pivotes = set(data["pivotes"])  # puntos que serán removidos

# ==== Filtrado de puntos activos (no pivotes) ====
# Creamos nuevo índice para puntos no pivote
index_map = {}  # viejo índice → nuevo índice
reverse_map = {}  # nuevo índice → viejo índice
filtered_points = []
new_index = 0
for i, p in enumerate(points):
    if i not in pivotes:
        index_map[i] = new_index
        reverse_map[new_index] = i
        filtered_points.append(p)
        new_index += 1

# ==== Filtrar aristas que no incluyen pivotes ====
filtered_edges = []
for i, j in edges:
    if i not in pivotes and j not in pivotes:
        filtered_edges.append([index_map[i], index_map[j]])

# ==== Graficar ====
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_xlim(-50, 150)
ax.set_ylim(-50, 150)
ax.set_title("Grafo KNN tras remover pivotes")

# Dibujar puntos (no pivotes)
ax.scatter([p[0] for p in filtered_points], [p[1] for p in filtered_points], s=10, color='lightgreen', zorder=1)

# Dibujar aristas filtradas
for i, j in filtered_edges:
    pi = filtered_points[i]
    pj = filtered_points[j]
    ax.plot([pi[0], pj[0]], [pi[1], pj[1]], color='gray', zorder=0)

plt.show()
