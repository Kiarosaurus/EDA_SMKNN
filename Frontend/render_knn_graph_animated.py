import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Leer datos
with open("frontend/visualization.json", "r") as f:
    data = json.load(f)

points = data["points"]
edges = data["edges"]
pivotes = list(data["pivotes"])

fig, ax = plt.subplots(figsize=(6, 6))
ax.set_xlim(0, 20)
ax.set_ylim(0, 20)
ax.set_title("Construcción del grafo KNN + Pivotes")

# Fase 1: puntos normales
ax.scatter([p[0] for p in points], [p[1] for p in points], s=60, color='lightblue', zorder=1)

# Fase 1: preparar pivote overlay
pivote_scat = ax.scatter([], [], s=120, color='red', edgecolors='black', zorder=2)
pivote_coords = [(points[i][0], points[i][1]) for i in pivotes]

lines = []
enter_1 = False
enter_2 = False
total_frames = len(edges) + len(pivote_coords) + 1  # +1 extra para permitir transición a fase final

def update(frame):
    global enter_1, enter_2

    if frame < len(edges):
        i, j = edges[frame]
        x_coords = [points[i][0], points[j][0]]
        y_coords = [points[i][1], points[j][1]]
        line = ax.plot(x_coords, y_coords, color='gray', zorder=0)[0]
        lines.append(line)

    elif frame < len(edges) + len(pivote_coords):
        if not enter_1:
            print("\nConstrucción del grafo terminada.")
            input("Presiona ENTER para mostrar los pivotes...")
            enter_1 = True

        step = frame - len(edges)
        current = pivote_coords[:step + 1]
        xs, ys = zip(*current) if current else ([], [])
        pivote_scat.set_offsets(list(zip(xs, ys)))

    elif not enter_2:
        input("\nPresiona ENTER para ver el grafo con pivotes removidos...")
        enter_2 = True

        # Limpiar el gráfico y volver a dibujar solo nodos no pivotes
        ax.clear()
        ax.set_xlim(0, 20)
        ax.set_ylim(0, 20)
        ax.set_title("Grafo KNN sin pivotes")

        non_pivot_indices = [i for i in range(len(points)) if i not in pivotes]
        non_pivot_points = [points[i] for i in non_pivot_indices]

        # Mapear índices originales a nuevos índices
        index_map = {old: idx for idx, old in enumerate(non_pivot_indices)}
        filtered_edges = [[i, j] for i, j in edges if i in index_map and j in index_map]

        # Dibujar puntos
        ax.scatter([p[0] for p in non_pivot_points], [p[1] for p in non_pivot_points], s=60, color='lightgreen', zorder=1)

        # Dibujar aristas filtradas
        for i, j in filtered_edges:
            pi = points[i]
            pj = points[j]
            ax.plot([pi[0], pj[0]], [pi[1], pj[1]], color='gray', zorder=0)

ani = FuncAnimation(fig, update, frames=total_frames, interval=700, repeat=False)
plt.show()
