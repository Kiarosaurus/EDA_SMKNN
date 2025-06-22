import json
import matplotlib.pyplot as plt

# Leer datos
with open("frontend/visualization.json", "r") as f:
    data = json.load(f)

points = data["points"]
edges = data["edges"]
pivotes = set(data["pivotes"])

# Crear una sola figura
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_xlim(-50, 150)
ax.set_ylim(-50, 150)
ax.set_title("Grafo KNN")

# Dibujar puntos y aristas
ax.scatter([p[0] for p in points], [p[1] for p in points], s=10, color='lightblue', zorder=1)
for i, j in edges:
    x_coords = [points[i][0], points[j][0]]
    y_coords = [points[i][1], points[j][1]]
    ax.plot(x_coords, y_coords, color='gray', zorder=0)

# === Agregar pivotes sin cerrar ===
pivote_coords = [(points[i][0], points[i][1]) for i in pivotes]
if pivote_coords:
    xs, ys = zip(*pivote_coords)
    ax.scatter(xs, ys, s=20, color='red', edgecolors='black', zorder=2)

ax.set_title("Grafo KNN con pivotes resaltados")
plt.show()
